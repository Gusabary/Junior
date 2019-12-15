#include "tiger/translate/translate.h"

#include <cstdio>
#include <set>
#include <string>
#include <fstream>

#include "tiger/errormsg/errormsg.h"
#include "tiger/frame/temp.h"
#include "tiger/semant/semant.h"
#include "tiger/semant/types.h"
#include "tiger/util/util.h"

extern EM::ErrorMsg errormsg;

using VEnvType = S::Table<E::EnvEntry> *;
using TEnvType = S::Table<TY::Ty> *;

namespace {
static TY::TyList *make_formal_tylist(TEnvType tenv, A::FieldList *params) {
  if (params == nullptr) {
    return nullptr;
  }

  TY::Ty *ty = tenv->Look(params->head->typ);
  if (ty == nullptr) {
    errormsg.Error(params->head->pos, "undefined type %s",
                   params->head->typ->Name().c_str());
    return new TY::TyList(TY::VoidTy::Instance(), make_formal_tylist(tenv, params->tail));
  }

  return new TY::TyList(ty->ActualTy(), make_formal_tylist(tenv, params->tail));
}

static U::BoolList *make_formal_boollist(A::FieldList *params) {
  U::BoolList *head = nullptr, *tail = nullptr;
  while (params) {
    if (head) {
      tail->tail = new U::BoolList(params->head->escape, nullptr);
      tail = tail->tail;
    }
    else {
      head = new U::BoolList(params->head->escape, nullptr);
      tail = head;
    }
    params = params->tail;
  }
  return head;
}

static TY::FieldList *make_fieldlist(TEnvType tenv, A::FieldList *fields) {
  if (fields == nullptr) {
    return nullptr;
  }

  TY::Ty *ty = tenv->Look(fields->head->typ);
  if (!ty) {
    errormsg.Error(fields->head->pos, "undefined type %s", fields->head->typ->Name().c_str());
    ty = TY::VoidTy::Instance();
  }
  return new TY::FieldList(new TY::Field(fields->head->name, ty),
                           make_fieldlist(tenv, fields->tail));
}

}

namespace TR {

static F::FragList *fragListHead = nullptr;
static F::FragList *fragListTail = nullptr;

class Access {
 public:
  Level *level;
  F::Access *access;

  Access(Level *level, F::Access *access) : level(level), access(access) {}
  // static Access *AllocLocal(Level *level, bool escape) { return nullptr; }
};

class AccessList {
 public:
  Access *head;
  AccessList *tail;

  AccessList(Access *head, AccessList *tail) : head(head), tail(tail) {}
};

class Level {
 public:
  F::Frame *frame;
  Level *parent;

  Level(F::Frame *frame, Level *parent) : frame(frame), parent(parent) {}
  AccessList *Formals() {
    AccessList *head = nullptr, *tail = nullptr;
    for (F::AccessList *al = this->frame->formals; al; al = al->tail) {
      Access *a = new Access(this, al->head);
      if (tail) {
        tail->tail = new AccessList(a, nullptr);
        tail = tail->tail;
      }
      else {
        head = new AccessList(a, nullptr);
        tail = head;
      }
    }
    return head;
  }

  static Level *NewLevel(Level *parent, TEMP::Label *name,
                         U::BoolList *formals) {
    F::Frame *f = F::NewFrame(name, new U::BoolList(true, formals));
    Level *l = new Level(f, parent);
    return l;
  }
};

class PatchList {
 public:
  TEMP::Label **head;
  PatchList *tail;

  PatchList(TEMP::Label **head, PatchList *tail) : head(head), tail(tail) {}
};

void do_patch(PatchList *tList, TEMP::Label *label) {
  for (; tList; tList = tList->tail) *(tList->head) = label;
}

PatchList *join_patch(PatchList *first, PatchList *second) {
  if (!first) return second;
  for (; first->tail; first = first->tail)
    ;
  first->tail = second;
  return first;
}

class Cx {
 public:
  PatchList *trues;
  PatchList *falses;
  T::Stm *stm;

  Cx(PatchList *trues, PatchList *falses, T::Stm *stm)
      : trues(trues), falses(falses), stm(stm) {}
};

class Exp {
 public:
  enum Kind { EX, NX, CX };

  Kind kind;

  Exp(Kind kind) : kind(kind) {}

  virtual T::Exp *UnEx() const = 0;
  virtual T::Stm *UnNx() const = 0;
  virtual Cx UnCx() const = 0;
};

class ExpList {
  public:
    Exp *head;
    ExpList *tail;

    ExpList(Exp *head, ExpList *tail) : head(head), tail(tail) {}
    T::ExpList *ToTreeExpList() {
      ExpList *cnt = this;
      T::ExpList *head = nullptr, *tail = nullptr;
      for (; cnt; cnt = cnt->tail) {
        if (head) {
          tail->tail = new T::ExpList(cnt->head->UnEx(), nullptr);
          tail = tail->tail;
        }
        else {
          head = new T::ExpList(cnt->head->UnEx(), nullptr);
          tail = head;
        }
      }
      return head;
    }
};

class ExpAndTy
{
public:
  TR::Exp *exp;
  TY::Ty *ty;

  ExpAndTy() {}
  ExpAndTy(TR::Exp *exp, TY::Ty *ty) : exp(exp), ty(ty) {}
};

class ExExp : public Exp {
 public:
  T::Exp *exp;

  ExExp(T::Exp *exp) : Exp(EX), exp(exp) {}

  T::Exp *UnEx() const override { return this->exp; }
  T::Stm *UnNx() const override { return new T::ExpStm(this->exp); }
  Cx UnCx() const override {
    T::Stm *s = new T::CjumpStm(T::NE_OP, this->exp, new T::ConstExp(0), nullptr, nullptr);
    PatchList *t = new PatchList(&(((T::CjumpStm *)s)->true_label), nullptr);
    PatchList *f = new PatchList(&(((T::CjumpStm *)s)->false_label), nullptr);
    return Cx(t, f, s);
  }
};

class NxExp : public Exp {
 public:
  T::Stm *stm;

  NxExp(T::Stm *stm) : Exp(NX), stm(stm) {}

  T::Exp *UnEx() const override { return new T::EseqExp(this->stm, new T::ConstExp(0)); }
  T::Stm *UnNx() const override { return this->stm; }
  Cx UnCx() const override {
    assert(0);
    return Cx(nullptr, nullptr, nullptr);
  }
};

class CxExp : public Exp {
 public:
  Cx cx;

  CxExp(struct Cx cx) : Exp(CX), cx(cx) {}
  CxExp(PatchList *trues, PatchList *falses, T::Stm *stm)
      : Exp(CX), cx(trues, falses, stm) {}

  T::Exp *UnEx() const override {
    TEMP::Temp *r = TEMP::Temp::NewTemp();
    TEMP::Label *t = TEMP::NewLabel();
    TEMP::Label *f = TEMP::NewLabel();
    do_patch(this->cx.trues, t);
    do_patch(this->cx.falses, f);
    return new T::EseqExp(new T::MoveStm(new T::TempExp(r), new T::ConstExp(1)),
            new T::EseqExp(this->cx.stm,
              new T::EseqExp(new T::LabelStm(f),
                new T::EseqExp(new T::MoveStm(new T::TempExp(r), new T::ConstExp(0)),
                  new T::EseqExp(new T::LabelStm(t),
                    new T::TempExp(r))))));
  }
  T::Stm *UnNx() const override {
    TEMP::Label *label = TEMP::NewLabel();
    do_patch(this->cx.trues, label);
    do_patch(this->cx.falses, label);
    return new T::SeqStm(this->cx.stm, new T::LabelStm(label));
  }
  Cx UnCx() const override { return this->cx; }
};

Level *Outermost() {
  static Level *lv = nullptr;
  if (lv != nullptr) return lv;

  lv = new Level(nullptr, nullptr);
  return lv;
}

F::FragList *TranslateProgram(A::Exp *root) {
  errormsg.Error(0, "Translate Program");
  TEMP::Label *func_label = TEMP::NamedLabel("tigermain");
  // F::Frame *main_frame = new F::Frame(func_label);
  // TR::Level *main_level = new TR::Level(main_frame, Outermost());
  TR::Level *main_level = TR::Level::NewLevel(Outermost(), func_label, /*new U::BoolList(true, nullptr)*/nullptr);
  E::EnvEntry *fun_entry = new E::FunEntry(main_level, func_label, nullptr, TY::VoidTy::Instance());

  ExpAndTy rootet = root->Translate(E::BaseVEnv(), E::BaseTEnv(), main_level, nullptr);
  errormsg.Error(0, "Translate Program root ok");

  procEntryExit(main_level, rootet.exp, nullptr);

  return fragListHead;
}

T::Exp *traceLink(TR::Level *callee, TR::Level *target) {
  // handle built-in function specially
  T::Exp *e = new T::TempExp(F::FP());
  if (callee == target) {
    return e;
    // return new T::BinopExp(T::PLUS_OP, new T::ConstExp(-8), e);
  }
  while (callee != target) {
    // static link info is in first arg
    F::Access *ac = callee->frame->formals->head;
    e = ac->ToExp(e);
    callee = callee->parent;
  }
  // e = new T::BinopExp(T::PLUS_OP, new T::ConstExp(8), e);
  return e;
}

Access *AllocLocal(Level *level, bool escape) {
  return new Access(level, level->frame->AllocLocal(escape));
}

static void AddFrag(F::Frag *frag) {
  if (fragListTail) {
    fragListTail->tail = new F::FragList(frag, nullptr);
    fragListTail = fragListTail->tail;
  }
  else {
    fragListHead = new F::FragList(frag, nullptr);
    fragListTail = fragListHead;
  }
}

void procEntryExit(Level *level, Exp *body, AccessList *formals) {
  // F::Frag *frag = new F::ProcFrag(body->UnNx(), level->frame);
  F::Frag *frag = new F::ProcFrag(new T::MoveStm(new T::TempExp(F::RAX()), body->UnEx()), level->frame);
  AddFrag(frag);
}

Exp *Null() {
  return new ExExp(new T::ConstExp(0));
}

Exp *SimpleVar(Access *acc, Level *lev) {
  return new ExExp(acc->access->ToExp(traceLink(lev, acc->level)));
}

Exp *FieldVar(Exp *base, int offset) {
  return new ExExp(new T::MemExp(new T::BinopExp(T::PLUS_OP, base->UnEx(), new T::ConstExp(offset * F::wordSize))));
}

Exp *SubscriptVar(Exp *base, Exp *index) {
  return new TR::ExExp(new T::MemExp(new T::BinopExp(T::PLUS_OP, base->UnEx(), new T::BinopExp(
      T::MUL_OP, index->UnEx(), new T::ConstExp(F::wordSize)))));
}

Exp *NilExp() {
  return nullptr;
}

Exp *IntExp(int val) {
  return new ExExp(new T::ConstExp(val));
}

Exp *StringExp(std::string str) {
  TEMP::Label *label = TEMP::NewLabel();
  F::Frag *frag = new F::StringFrag(label, str);
  AddFrag(frag);
  return new ExExp(new T::NameExp(label));
}

// lev: level of callee,  call: caller
Exp *CallExp(S::Symbol *fun, ExpList *el, Level *lev, Level *call) {
  T::ExpList *args = el->ToTreeExpList();

  if (lev != Outermost()) {
    // user-defined function, make lev mean the level where callee is defined
    lev = lev->parent;
    // add static link info to first
    // args = new T::ExpList(new T::BinopExp(T::PLUS_OP, new T::ConstExp(-8), traceLink(call, lev)), args);
    args = new T::ExpList(traceLink(call, lev), args);
    return new ExExp(new T::CallExp(new T::NameExp(fun), args));
  }

  // built-in function doesn't consider static link
  return new ExExp(new T::CallExp(new T::NameExp(fun), args));
}

Exp *ArithExp(A::Oper oper, Exp *left, Exp *right) {
  T::BinOp op;
  switch (oper) {
    case A::PLUS_OP:    op = T::PLUS_OP; break;
    case A::MINUS_OP:   op = T::MINUS_OP; break;
    case A::TIMES_OP:   op = T::MUL_OP; break;
    case A::DIVIDE_OP:  op = T::DIV_OP; break;
  }
  return new ExExp(new T::BinopExp(op, left->UnEx(), right->UnEx()));
}

Exp *RelExp(A::Oper oper, Exp *left, Exp *right) {
  T::RelOp op;
  switch (oper) {
    case A::EQ_OP:    op = T::EQ_OP; break;
    case A::NEQ_OP:   op = T::NE_OP; break;
    case A::LT_OP:    op = T::LT_OP; break;
    case A::LE_OP:    op = T::LE_OP; break;
    case A::GT_OP:    op = T::GT_OP; break;
    case A::GE_OP:    op = T::GE_OP; break;
  }
  T::Stm *stm = new T::CjumpStm(op, left->UnEx(), right->UnEx(), nullptr, nullptr);
  PatchList *trues = new PatchList(&((T::CjumpStm *)stm)->true_label, nullptr);
  PatchList *falses = new PatchList(&((T::CjumpStm *)stm)->false_label, nullptr);
  return new CxExp(trues, falses, stm);
}

Exp *EqStrExp(A::Oper oper, Exp *left, Exp *right) {
  T::Exp *ans = F::ExternalCall(std::string("stringEqual"), new T::ExpList(left->UnEx(),
        new T::ExpList(right->UnEx(), nullptr)));
  if (oper == A::EQ_OP)
    return new ExExp(ans);
  else
    return new ExExp(new T::BinopExp(T::MINUS_OP, new T::ConstExp(1), ans));
}

Exp *RecordExp(int num, ExpList *l) {
  TEMP::Temp *r = TEMP::Temp::NewTemp();
  T::Stm *alloc = new T::MoveStm(new T::TempExp(r),
    F::ExternalCall(std::string("allocRecord"), new T::ExpList(new T::ConstExp(num * F::wordSize), nullptr)));

  int i = num - 1;
  T::Stm *seq = new T::MoveStm(new T::MemExp(
    new T::BinopExp(T::PLUS_OP, new T::TempExp(r), new T::ConstExp(i * F::wordSize))), l->head->UnEx());
  i--;
  for (l = l->tail; l; l = l->tail, i--)
  {
    seq = new T::SeqStm(new T::MoveStm(new T::MemExp(
      new T::BinopExp(T::PLUS_OP, new T::TempExp(r), new T::ConstExp(i * F::wordSize))), l->head->UnEx()), seq);
  }
  return new ExExp(new T::EseqExp(new T::SeqStm(alloc, seq), new T::TempExp(r)));
}

Exp *ArrayExp(Exp *size, Exp *init) {
  return new ExExp(F::ExternalCall(std::string("initArray"), new T::ExpList(size->UnEx(),
    new T::ExpList(init->UnEx(), nullptr))));
}

Exp *SeqExp(ExpList *l) {
  errormsg.Error(0, "Tree SeqExp");
  T::Exp *seq = l->head->UnEx();
  // errormsg.Error(0, "Tree SeqExp kind: %d", l->head->UnEx()->kind);
  // errormsg.Error(0, "Tree SeqExp addr: %x", l->head);

  for (TR::ExpList *p = l->tail; p; p = p->tail) {
  // errormsg.Error(0, "Tree SeqExp inloop");
  // errormsg.Error(0, "Tree SeqExp kind: %d", p->head->UnNx()->kind);
  // errormsg.Error(0, "Tree SeqExp addr: %x", p->head);
  
    seq = new T::EseqExp(p->head->UnNx(), seq);
  }
  // errormsg.Error(0, "Tree SeqExp ok");
  return new ExExp(seq);
}

Exp *AssignExp(Exp *left, Exp *right) {
  return new NxExp(new T::MoveStm(left->UnEx(), right->UnEx()));
}

Exp *IfExp(Exp *test, Exp *then, Exp *elsee) {
  Cx cond = test->UnCx();
  TEMP::Label *t = TEMP::NewLabel();
  TEMP::Label *f = TEMP::NewLabel();
  do_patch(cond.trues, t);
  do_patch(cond.falses, f);

  if (!elsee) {
    return new NxExp(new T::SeqStm(cond.stm,
      new T::SeqStm(new T::LabelStm(t),
        new T::SeqStm(then->UnNx(), new T::LabelStm(f)))));
  }
  else {
    TEMP::Label *join = TEMP::NewLabel();
    T::Stm *joinJump = new T::JumpStm(new T::NameExp(join), new TEMP::LabelList(join, nullptr));

    if (then->kind == Exp::NX || elsee->kind == Exp::NX) {
      return new NxExp(new T::SeqStm(cond.stm,
        new T::SeqStm(new T::LabelStm(t),
          new T::SeqStm(then->UnNx(),
            new T::SeqStm(joinJump,
              new T::SeqStm(new T::LabelStm(f),
                new T::SeqStm(elsee->UnNx(), new T::LabelStm(join))))))));
    }
    else {
      TEMP::Temp *r = TEMP::Temp::NewTemp();
      return new ExExp(new T::EseqExp(cond.stm,
        new T::EseqExp(new T::LabelStm(t),
          new T::EseqExp(new T::MoveStm(new T::TempExp(r), then->UnEx()),
            new T::EseqExp(joinJump,
              new T::EseqExp(new T::LabelStm(f),
                new T::EseqExp(new T::MoveStm(new T::TempExp(r), elsee->UnEx()),
                  new T::EseqExp(new T::LabelStm(join), new T::TempExp(r)))))))));
    }
  }
}

Exp *WhileExp(Exp *test, Exp *body, Exp *done) {
  Cx cond = test->UnCx();
  TEMP::Label *testLabel = TEMP::NewLabel();
  TEMP::Label *bodyLabel = TEMP::NewLabel();
  TEMP::Label *doneLabel = ((T::NameExp *)(done->UnEx()))->name;
  do_patch(cond.trues, bodyLabel);
  do_patch(cond.falses, doneLabel);

  return new NxExp(new T::SeqStm(new T::LabelStm(testLabel),
          new T::SeqStm(cond.stm,
            new T::SeqStm(new T::LabelStm(bodyLabel),
              new T::SeqStm(body->UnNx(),
                new T::SeqStm(new T::JumpStm(new T::NameExp(testLabel), new TEMP::LabelList(testLabel, nullptr)),
                  new T::LabelStm(doneLabel)))))));
}

Exp *ForExp(Level *lev, Access *iac, Exp *lo, Exp *hi, Exp *body, Exp *done) {
  Exp *ex_i = SimpleVar(iac, lev);
  T::Stm *stm_i = AssignExp(ex_i, lo)->UnNx();
  Access *ac_lim = AllocLocal(lev, false);
  Exp *ex_lim = SimpleVar(ac_lim, lev);
  T::Stm *stm_lim = AssignExp(ex_lim, hi)->UnNx();

  T::Stm *stm_while = new T::CjumpStm(T::LE_OP, ex_i->UnEx(), ex_lim->UnEx(), nullptr, nullptr);
  PatchList *trues = new PatchList(&(((T::CjumpStm *)stm_while)->true_label), nullptr);
  PatchList *falses = new PatchList(&(((T::CjumpStm *)stm_while)->false_label), nullptr);
  Exp *cond_while = new CxExp(trues, falses, stm_while);

  Exp *dobody = new NxExp(new T::SeqStm(body->UnNx(), new T::MoveStm(ex_i->UnEx(), 
    new T::BinopExp(T::PLUS_OP, ex_i->UnEx(), new T::ConstExp(1)))));  //shouldn't be lim
  T::Stm *loop = ((NxExp *)(WhileExp(cond_while, dobody, done)))->stm;

  return new NxExp(new T::SeqStm(new T::SeqStm(stm_i, stm_lim), loop));
}

Exp *DoneExp() {
  return new ExExp(new T::NameExp(TEMP::NewLabel()));
}

Exp *BreakExp(TEMP::Label *label) {
  return new NxExp(new T::JumpStm(new T::NameExp(label), new TEMP::LabelList(label, nullptr)));
}

}  // namespace TR

namespace A {

TR::ExpAndTy SimpleVar::Translate(S::Table<E::EnvEntry> *venv,
                                  S::Table<TY::Ty> *tenv, TR::Level *level,
                                  TEMP::Label *label) const {
  errormsg.Error(0, "Translate SimpleVar");
  E::EnvEntry *e = venv->Look(this->sym);

  if (e && e->kind == E::EnvEntry::VAR) {
    TR::Access *godKnowsWhy = ((E::VarEntry *)e)->access;
    TR::Exp *exp = TR::SimpleVar(godKnowsWhy, level);
    return TR::ExpAndTy(exp, ((E::VarEntry *)e)->ty);
  }

  errormsg.Error(this->pos, "undefined variable %s", this->sym->Name().c_str());
  return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
}

TR::ExpAndTy FieldVar::Translate(S::Table<E::EnvEntry> *venv,
                                 S::Table<TY::Ty> *tenv, TR::Level *level,
                                 TEMP::Label *label) const {
  errormsg.Error(0, "Translate FieldVar");
  TR::ExpAndTy et = this->var->Translate(venv, tenv, level, label);

  if (et.ty->kind != TY::Ty::RECORD) {
    errormsg.Error(this->pos, "not a record type");
    return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
  }

  TY::FieldList *fl = ((TY::RecordTy *)et.ty)->fields;
  int i = 0;
  while (fl)
  {
    if (fl->head->name == this->sym) {
      TR::Exp *e = TR::FieldVar(et.exp, i);
      return TR::ExpAndTy(e, fl->head->ty);
    }
    fl = fl->tail;
    i++;
  }

  errormsg.Error(this->pos, "field %s doesn't exist", this->sym->Name().c_str());
  return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
}

TR::ExpAndTy SubscriptVar::Translate(S::Table<E::EnvEntry> *venv,
                                     S::Table<TY::Ty> *tenv, TR::Level *level,
                                     TEMP::Label *label) const {
  errormsg.Error(0, "Translate SubscriptVar");
  TR::ExpAndTy varet = this->var->Translate(venv, tenv, level, label);
  if (varet.ty->kind != TY::Ty::ARRAY) {
    errormsg.Error(this->pos, "array type required");
    return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
  }

  TR::ExpAndTy expet = this->subscript->Translate(venv, tenv, level, label);
  if (expet.ty->kind != TY::Ty::INT) {
    errormsg.Error(this->pos, "Subscript was not an integer");
    return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
  }

  TR::Exp *e = TR::SubscriptVar(varet.exp, expet.exp);
  return TR::ExpAndTy(e, ((TY::ArrayTy *)(varet.ty))->ty);
}

TR::ExpAndTy VarExp::Translate(S::Table<E::EnvEntry> *venv,
                               S::Table<TY::Ty> *tenv, TR::Level *level,
                               TEMP::Label *label) const {
  errormsg.Error(0, "Translate VarExp");
  return this->var->Translate(venv, tenv, level, label);
}

TR::ExpAndTy NilExp::Translate(S::Table<E::EnvEntry> *venv,
                               S::Table<TY::Ty> *tenv, TR::Level *level,
                               TEMP::Label *label) const {
  errormsg.Error(0, "Translate NilExp");
  return TR::ExpAndTy(TR::Null(), TY::NilTy::Instance());
}

TR::ExpAndTy IntExp::Translate(S::Table<E::EnvEntry> *venv,
                               S::Table<TY::Ty> *tenv, TR::Level *level,
                               TEMP::Label *label) const {
  errormsg.Error(0, "Translate IntExp");
  TR::Exp *e = TR::IntExp(this->i);
  return TR::ExpAndTy(e, TY::IntTy::Instance());
}

TR::ExpAndTy StringExp::Translate(S::Table<E::EnvEntry> *venv,
                                  S::Table<TY::Ty> *tenv, TR::Level *level,
                                  TEMP::Label *label) const {
  errormsg.Error(0, "Translate StringExp");
  TR::Exp *e = TR::StringExp(this->s);
  return TR::ExpAndTy(e, TY::StringTy::Instance());
}

TR::ExpAndTy CallExp::Translate(S::Table<E::EnvEntry> *venv,
                                S::Table<TY::Ty> *tenv, TR::Level *level,
                                TEMP::Label *label) const {
  errormsg.Error(0, "Translate CallExp");
  E::EnvEntry *callee = venv->Look(this->func);
  if (!callee) {
    errormsg.Error(this->pos, "undefined function %s", this->func->Name().c_str());
    return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
  }
  if (callee->kind != E::EnvEntry::FUN) {
    errormsg.Error(this->pos, "'%s' was a variable", this->func->Name().c_str());
    return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
  }

  A::ExpList *actuals = this->args;
  TY::TyList *formals = ((E::FunEntry *)callee)->formals;
  TR::ExpList *head = nullptr, *tail = nullptr;

  while (actuals && formals) {
    TR::ExpAndTy actualset = actuals->head->Translate(venv, tenv, level, label);
    if (actualset.ty != formals->head)
      errormsg.Error(this->pos, "para type mismatch");

    if (head) {
      tail->tail = new TR::ExpList(actualset.exp, nullptr);
      tail = tail->tail;
    }
    else {
      head = new TR::ExpList(actualset.exp, nullptr);
      tail = head;
    }
    actuals = actuals->tail;
    formals = formals->tail;
  }

  if (formals) {
    errormsg.Error(this->pos, "too few params in function %s", this->func->Name().c_str());
    return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
  }
  if (actuals) {
    errormsg.Error(this->pos, "too many params in function %s", this->func->Name().c_str());
    return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
  }

  TR::Exp *e = TR::CallExp(this->func, head, ((E::FunEntry *)callee)->level, level);
  return TR::ExpAndTy(e, ((E::FunEntry *)callee)->result);
}

TR::ExpAndTy OpExp::Translate(S::Table<E::EnvEntry> *venv,
                              S::Table<TY::Ty> *tenv, TR::Level *level,
                              TEMP::Label *label) const {
  errormsg.Error(0, "Translate OpExp");
  TR::ExpAndTy left = this->left->Translate(venv, tenv, level, label);
  TR::ExpAndTy right = this->right->Translate(venv, tenv, level, label);

  switch (this->oper) {
    case A::PLUS_OP:
    case A::MINUS_OP:
    case A::TIMES_OP:
    case A::DIVIDE_OP: {
      if (left.ty->kind == TY::Ty::INT && right.ty->kind == TY::Ty::INT) {
        // divide 0 check ?
        TR::Exp *e = TR::ArithExp(this->oper, left.exp, right.exp);
        return TR::ExpAndTy(e, TY::IntTy::Instance());
      }
      errormsg.Error(this->pos, "integer required");
      return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
    }
    case A::GT_OP:
    case A::GE_OP:
    case A::LT_OP:
    case A::LE_OP: {
      if (left.ty->kind != TY::Ty::INT && left.ty->kind != TY::Ty::STRING) {
        errormsg.Error(this->pos, "string or integer required");  
      }
      else if (left.ty->kind != right.ty->kind) {
        errormsg.Error(this->pos, "same type required");
      }
      else {
        TR::Exp *e = TR::RelExp(this->oper, left.exp, right.exp);
        return TR::ExpAndTy(e, TY::IntTy::Instance());
      }
      return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
    }
    case A::EQ_OP:
    case A::NEQ_OP: {
      if (left.ty->kind == TY::Ty::VOID || right.ty->kind == TY::Ty::VOID) {
        errormsg.Error(this->pos, "expression had no value");
      }
      else if (left.ty->kind != right.ty->kind && left.ty->kind != TY::Ty::NIL && right.ty->kind != TY::Ty::NIL) {
        errormsg.Error(this->pos, "same type required");
      }
      else if (left.ty->kind == TY::Ty::STRING) {
        TR::Exp *e = TR::EqStrExp(this->oper, left.exp, right.exp);
        return TR::ExpAndTy(e, TY::IntTy::Instance());
      }
      else {
        TR::Exp *e = TR::RelExp(this->oper, left.exp, right.exp);
        return TR::ExpAndTy(e, TY::IntTy::Instance());
      }
      return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
    }
    default:
      break;
  }
  return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
}

TR::ExpAndTy RecordExp::Translate(S::Table<E::EnvEntry> *venv,
                                  S::Table<TY::Ty> *tenv, TR::Level *level,
                                  TEMP::Label *label) const {
  errormsg.Error(0, "Translate RecordExp");
  TY::Ty *t = tenv->Look(this->typ);
  if (!t) {
    errormsg.Error(this->pos, "undefined type %s", this->typ->Name().c_str());
    return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
  }
  if (t->kind != TY::Ty::RECORD) {
    errormsg.Error(this->pos, "'%s' was not a record type", this->typ->Name().c_str());
    return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
  }

  A::EFieldList *actuals = this->fields;
  TY::FieldList *formals = ((TY::RecordTy *)t)->fields;
  TR::ExpList *el = nullptr;
  int num = 0;
  while (actuals && formals)
  {
    if (actuals->head->name != formals->head->name) {
      errormsg.Error(this->pos, "need member '%s' but '%s'", formals->head->name->Name().c_str(), actuals->head->name->Name().c_str());
      return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
    }
    TR::ExpAndTy et = actuals->head->exp->Translate(venv, tenv, level, label);
    el = new TR::ExpList(et.exp, el);
    num++;
    if (et.ty != formals->head->ty->ActualTy())
    {
      errormsg.Error(this->pos, "member '%s' type mismatch", formals->head->name->Name().c_str());
      return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
    }
    actuals = actuals->tail;
    formals = formals->tail;
  }

  if (formals) {
    errormsg.Error(this->pos, "too few initializers for '%s'", this->typ->Name().c_str());
    return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
  }
  if (actuals) {
    errormsg.Error(this->pos, "too many initializers for '%s'", this->typ->Name().c_str());
    return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
  }

  if (num > 0) {
    TR::Exp *e = TR::RecordExp(num, el);
    return TR::ExpAndTy(e, t);
  }
  return TR::ExpAndTy(TR::Null(), t);  // diff
}

TR::ExpAndTy SeqExp::Translate(S::Table<E::EnvEntry> *venv,
                               S::Table<TY::Ty> *tenv, TR::Level *level,
                               TEMP::Label *label) const {
  errormsg.Error(0, "Translate SeqExp");
  A::ExpList *seqs = this->seq;
  TR::ExpAndTy seqet;
  TR::ExpList *tl = nullptr;

  while (seqs) {
    seqet = seqs->head->Translate(venv, tenv, level, label);

    tl = new TR::ExpList(seqet.exp, tl);
    seqs = seqs->tail;
  }
  return TR::ExpAndTy(TR::SeqExp(tl), seqet.ty);
}

TR::ExpAndTy AssignExp::Translate(S::Table<E::EnvEntry> *venv,
                                  S::Table<TY::Ty> *tenv, TR::Level *level,
                                  TEMP::Label *label) const {
  errormsg.Error(0, "Translate AssignExp");
  TR::ExpAndTy varet = this->var->Translate(venv, tenv, level, label);
  TR::ExpAndTy expet = this->exp->Translate(venv, tenv, level, label);
  if (varet.ty != expet.ty)
  {
    errormsg.Error(this->pos, "unmatched assign exp");
  }

  E::EnvEntry *entry = venv->Look(((A::SimpleVar *)this->var)->sym);
  if (entry && entry->readonly) {
    errormsg.Error(this->pos, "loop variable can't be assigned");
  }

  TR::Exp *e = TR::AssignExp(varet.exp, expet.exp);
  return TR::ExpAndTy(e, TY::VoidTy::Instance());
}

TR::ExpAndTy IfExp::Translate(S::Table<E::EnvEntry> *venv,
                              S::Table<TY::Ty> *tenv, TR::Level *level,
                              TEMP::Label *label) const {
  errormsg.Error(0, "Translate IfExp");
  TR::ExpAndTy testet = this->test->Translate(venv, tenv, level, label);
  if (testet.ty->kind != TY::Ty::INT)
  {
    errormsg.Error(this->pos, "if-exp was not an integer");
  }

  TR::ExpAndTy thenet = this->then->Translate(venv, tenv, level, label);
  if (this->elsee) {
    TR::ExpAndTy elseet = this->elsee->Translate(venv, tenv, level, label);
    if (thenet.ty != elseet.ty && !(thenet.ty->kind == TY::Ty::NIL || elseet.ty->kind == TY::Ty::NIL))
    {
      errormsg.Error(this->pos, "then exp and else exp type mismatch");
    }
    // for &, | shortcut
    // if (thenet.ty->kind != TY::Ty::VOID || elseet.ty->kind != TY::Ty::VOID) {
    //   errormsg.Error(this->pos, "if-then exp's body must produce no value");
    // }
    TR::Exp *e = TR::IfExp(testet.exp, thenet.exp, elseet.exp);
    return TR::ExpAndTy(e, thenet.ty);
  }

  if (thenet.ty->kind != TY::Ty::VOID) {
    errormsg.Error(this->pos, "if-then exp's body must produce no value");
    return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
  }
  TR::Exp *e = TR::IfExp(testet.exp, thenet.exp, nullptr);
  return TR::ExpAndTy(e, thenet.ty);
}

TR::ExpAndTy WhileExp::Translate(S::Table<E::EnvEntry> *venv,
                                 S::Table<TY::Ty> *tenv, TR::Level *level,
                                 TEMP::Label *label) const {
  errormsg.Error(0, "Translate WhileExp");
  TR::ExpAndTy testet = this->test->Translate(venv, tenv, level, label);
  if (testet.ty->kind != TY::Ty::INT) {
    errormsg.Error(this->pos, "while-exp was not an integer");
  }

  TR::Exp *done = TR::DoneExp();
  TEMP::Label *doneLabel = ((T::NameExp *)(((TR::ExExp *)done)->exp))->name;
  TR::ExpAndTy bodyet = this->body->Translate(venv, tenv, level, doneLabel);

  if (bodyet.ty->kind != TY::Ty::VOID) {
    errormsg.Error(this->pos, "while body must produce no value");
  }

  TR::Exp *e = TR::WhileExp(testet.exp, bodyet.exp, done);
  return TR::ExpAndTy(e, TY::VoidTy::Instance());
}

TR::ExpAndTy ForExp::Translate(S::Table<E::EnvEntry> *venv,
                               S::Table<TY::Ty> *tenv, TR::Level *level,
                               TEMP::Label *label) const {
  errormsg.Error(0, "Translate ForExp");
  TR::ExpAndTy loet = this->lo->Translate(venv, tenv, level, label);
  TR::ExpAndTy hiet = this->hi->Translate(venv, tenv, level, label);
  if (loet.ty->kind != TY::Ty::INT || hiet.ty->kind != TY::Ty::INT) {
    errormsg.Error(this->pos, "for exp's range type is not integer");
  }

  venv->BeginScope();

  TR::Access *iac = TR::AllocLocal(level, this->escape);
  E::VarEntry *ve = new E::VarEntry(iac, TY::IntTy::Instance(), true);
  venv->Enter(this->var, ve);
  TR::Exp *done = TR::DoneExp();
  TEMP::Label *doneLabel = ((T::NameExp *)(((TR::ExExp *)done)->exp))->name;
  TR::ExpAndTy bodyet = this->body->Translate(venv, tenv, level, doneLabel);

  venv->EndScope();

  if (bodyet.ty->kind != TY::Ty::VOID) {
    errormsg.Error(this->pos, "body exp shouldn't return a value");
  }

  TR::Exp *e = TR::ForExp(level, iac, loet.exp, hiet.exp, bodyet.exp, done);
  return TR::ExpAndTy(e, TY::VoidTy::Instance());
}

TR::ExpAndTy BreakExp::Translate(S::Table<E::EnvEntry> *venv,
                                 S::Table<TY::Ty> *tenv, TR::Level *level,
                                 TEMP::Label *label) const {
  errormsg.Error(0, "Translate BreakExp");
  if (!label) {
    errormsg.Error(this->pos, "break statement not within loop");
    return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
  }
  return TR::ExpAndTy(TR::BreakExp(label), TY::VoidTy::Instance());
}

TR::ExpAndTy LetExp::Translate(S::Table<E::EnvEntry> *venv,
                               S::Table<TY::Ty> *tenv, TR::Level *level,
                               TEMP::Label *label) const {
  errormsg.Error(0, "Translate LetExp");
  A::DecList *decs = this->decs;
  venv->BeginScope();
  tenv->BeginScope();

  TR::Exp *e;
  TR::ExpList *el = nullptr;
  while (decs)
  {
    e = decs->head->Translate(venv, tenv, level, label);
    el = new TR::ExpList(e, el);
    decs = decs->tail;
  }
  TR::ExpAndTy bodyet = this->body->Translate(venv, tenv, level, label);
  errormsg.Error(0, "Translate LetExp SEQ OK");

  el = new TR::ExpList(bodyet.exp, el);

  venv->EndScope();
  tenv->EndScope();
  return TR::ExpAndTy(TR::SeqExp(el), bodyet.ty);
}

TR::ExpAndTy ArrayExp::Translate(S::Table<E::EnvEntry> *venv,
                                 S::Table<TY::Ty> *tenv, TR::Level *level,
                                 TEMP::Label *label) const {
  errormsg.Error(0, "Translate ArrayExp");
  TY::Ty *t = tenv->Look(this->typ);
  if (!t) {
    errormsg.Error(this->pos, "undefined type %s", this->typ->Name().c_str());
    return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
  }
  if (t->kind != TY::Ty::ARRAY) {
    errormsg.Error(this->pos, "'%s' was not a array type", this->typ->Name().c_str());
    return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
  }

  TR::ExpAndTy sizeet = this->size->Translate(venv, tenv, level, label);
  TR::ExpAndTy initet = this->init->Translate(venv, tenv, level, label);

  if (sizeet.ty->kind != TY::Ty::INT) {
    errormsg.Error(this->pos, "array size was not an integer value");
  }

  if (((TY::ArrayTy *)t)->ty != initet.ty) {
    errormsg.Error(this->pos, "array init type mismatch");
  }

  TR::Exp *e = TR::ArrayExp(sizeet.exp, initet.exp);
  return TR::ExpAndTy(e, t);
}

TR::ExpAndTy VoidExp::Translate(S::Table<E::EnvEntry> *venv,
                                S::Table<TY::Ty> *tenv, TR::Level *level,
                                TEMP::Label *label) const {
  errormsg.Error(0, "Translate VoidExp");
  return TR::ExpAndTy(nullptr, TY::VoidTy::Instance());
}

TR::Exp *FunctionDec::Translate(S::Table<E::EnvEntry> *venv,
                                S::Table<TY::Ty> *tenv, TR::Level *level,
                                TEMP::Label *label) const {
  errormsg.Error(0, "Translate FunctionDec");
  A::FunDecList *list = this->functions;
  std::set<S::Symbol *> s;

  TY::TyList *formalst = nullptr;
  while (list) {
    if (s.find(list->head->name) != s.end()) {
      errormsg.Error(this->pos, "two functions have the same name", list->head->name);
      list = list->tail;
      continue;
    }
    s.insert(list->head->name);

    TY::Ty *result = nullptr;
    if (list->head->result) {
      result = tenv->Look(list->head->result);
      if (!result) {
        errormsg.Error(this->pos, "function result: undefined type %s", list->head->result->Name().c_str());
      }
    }
    else {
      result = TY::VoidTy::Instance();
    }

    formalst = make_formal_tylist(tenv, list->head->params);
    // TEMP::Label *la = TEMP::NewLabel();
    TEMP::Label *la = TEMP::NamedLabel(list->head->name->Name());
    TR::Level *le = TR::Level::NewLevel(level, la, make_formal_boollist(list->head->params));
    venv->Enter(list->head->name, new E::FunEntry(le, la, formalst, result));
    list = list->tail;
  }

  s.clear();
  list = this->functions;
  while (list) {
    if (s.find(list->head->name) != s.end()) {
      list = list->tail;
      continue;
    }
    s.insert(list->head->name);

    E::EnvEntry *ent = venv->Look(list->head->name);
    venv->BeginScope();
    A::FieldList *formals = list->head->params;
    TR::AccessList *al = ((E::FunEntry *)ent)->level->Formals()->tail;
    while (formals)
    {
      TY::Ty *t = tenv->Look(formals->head->typ);
      if (!t)
        t = TY::IntTy::Instance();
      venv->Enter(formals->head->name, new E::VarEntry(al->head, t));
      formals = formals->tail;
      al = al->tail;
    }

    TR::ExpAndTy result = list->head->body->Translate(venv, tenv, ((E::FunEntry *)ent)->level, label);
    TR::procEntryExit(((E::FunEntry *)ent)->level, result.exp, al);
    if (result.ty->kind != TY::Ty::VOID && ((E::FunEntry *)ent)->result->kind == TY::Ty::VOID)
    {
      errormsg.Error(this->pos, "procedure returns value", list->head->name->Name());
    }
    if (result.ty != ((E::FunEntry *)venv->Look(list->head->name))->result) {
      errormsg.Error(this->pos, "body result type mismatch");
    }

    venv->EndScope();
    list = list->tail;
  }
  return TR::Null();
}

TR::Exp *VarDec::Translate(S::Table<E::EnvEntry> *venv, S::Table<TY::Ty> *tenv,
                           TR::Level *level, TEMP::Label *label) const {
  errormsg.Error(0, "Translate VarDec");
  TR::ExpAndTy initet = this->init->Translate(venv, tenv, level, label);
  TR::Access *ac = TR::AllocLocal(level, this->escape);
  if (this->typ)
  {
    TY::Ty *typt = tenv->Look(this->typ);
    if (!typt) {
      errormsg.Error(this->pos, "undefined type %s", this->typ->Name().c_str());
    }
    if (typt != initet.ty) {
      errormsg.Error(this->pos, "var init type mismatch");
      venv->Enter(this->var, new E::VarEntry(typt));
      return nullptr;
    }
  }
  if (initet.ty->kind == TY::Ty::VOID) {
    errormsg.Error(this->pos, "initialize with no value");
  }
  else if (initet.ty->kind == TY::Ty::NIL) {
    errormsg.Error(this->pos, "init should not be nil without type specified");
  }
  venv->Enter(this->var, new E::VarEntry(ac, initet.ty));
  return TR::AssignExp(TR::SimpleVar(ac, level), initet.exp);
}

TR::Exp *TypeDec::Translate(S::Table<E::EnvEntry> *venv, S::Table<TY::Ty> *tenv,
                            TR::Level *level, TEMP::Label *label) const {
  errormsg.Error(0, "Translate TypeDec");
  A::NameAndTyList *list = this->types;
  std::set<S::Symbol *> s;

  while (list)
  {
    if (s.find(list->head->name) != s.end()) {
      errormsg.Error(this->pos, "two types have the same name", list->head->name->Name());
      list = list->tail;
      continue;
    }
    s.insert(list->head->name);
    tenv->Enter(list->head->name, new TY::NameTy(list->head->name, nullptr));
    list = list->tail;
  }

  s.clear();
  list = this->types;
  while (list) {
    if (s.find(list->head->name) != s.end()) {
      list = list->tail;
      continue;
    }
    s.insert(list->head->name);
    TY::Ty *t = tenv->Look(list->head->name);
    ((TY::NameTy *)t)->ty = list->head->ty->Translate(tenv);
    list = list->tail;
  }

  for (list = this->types; list; list = list->tail) {
    TY::Ty *t = tenv->Look(list->head->name);
    if (t->kind == TY::Ty::VOID) {
      return nullptr;
    }
    t = ((TY::NameTy *)t)->ty;
    s.clear();
    while (t && t->kind == TY::Ty::NAME) {
    // while (((TY::NameTy *)t)->ty && ((TY::NameTy *)t)->ty->kind == TY::Ty::NAME) {
      // t = ((TY::NameTy *)t)->ty;
      if (s.find(((TY::NameTy *)t)->sym) != s.end())
      {
        errormsg.Error(this->pos, "illegal type cycle");
        ((TY::NameTy *)t)->ty = TY::VoidTy::Instance(); // critical!!
        break;
      }
      s.insert(((TY::NameTy *)t)->sym);

      t = ((TY::NameTy *)t)->ty;
      if (t->kind == TY::Ty::NAME)
        t = ((TY::NameTy *)t)->ty;
    }
    tenv->Enter(list->head->name, t);
  }
  return TR::Null();
}

TY::Ty *NameTy::Translate(S::Table<TY::Ty> *tenv) const {
  errormsg.Error(0, "Translate NameTy");
  TY::Ty *t = tenv->Look(this->name);
  errormsg.Error(this->pos, "&* %s", this->name->Name().c_str());
  if (t)
  {
    return new TY::NameTy(this->name, t);
  }
  errormsg.Error(this->pos, "undefined type %s", this->name->Name().c_str());
  return TY::VoidTy::Instance();
}

TY::Ty *RecordTy::Translate(S::Table<TY::Ty> *tenv) const {
  errormsg.Error(0, "Translate RecordTy");
  return new TY::RecordTy(make_fieldlist(tenv, this->record));
}

TY::Ty *ArrayTy::Translate(S::Table<TY::Ty> *tenv) const {
  errormsg.Error(0, "Translate ArrayTy");
  TY::Ty *t = tenv->Look(this->array);
  if (t) {
    return new TY::ArrayTy(t);
  }
  errormsg.Error(this->pos, "undefined type %s", this->array->Name().c_str());
  return TY::VoidTy::Instance();
}

}  // namespace A
