#include "tiger/semant/semant.h"
#include "tiger/errormsg/errormsg.h"

#include <set>

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

static E::EnvEntry *loopv[100];
} // namespace

namespace A {

TY::Ty *SimpleVar::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  E::EnvEntry *e = venv->Look(this->sym);

  if (e && e->kind == E::EnvEntry::VAR) {
    return ((E::VarEntry *)e)->ty;
  }

  errormsg.Error(this->pos, "undefined variable %s", this->sym->Name().c_str());
  return TY::VoidTy::Instance();
}

TY::Ty *FieldVar::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  TY::Ty *t = this->var->SemAnalyze(venv, tenv, labelcount);

  if (t->kind != TY::Ty::RECORD) {
    errormsg.Error(this->pos, "not a record type");
    return TY::VoidTy::Instance();
  }

  TY::FieldList *fl = ((TY::RecordTy *)t)->fields;
  while (fl) {
    if (fl->head->name == this->sym) {
      return fl->head->ty;
    }
    fl = fl->tail;
  }

  errormsg.Error(this->pos, "field %s doesn't exist", this->sym->Name().c_str());
  return TY::VoidTy::Instance();
}

TY::Ty *SubscriptVar::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  TY::Ty *vart = this->var->SemAnalyze(venv, tenv, labelcount);
  if (vart->kind != TY::Ty::ARRAY) {
    errormsg.Error(this->pos, "array type required");
    return TY::VoidTy::Instance();
  }

  TY::Ty *expt = this->subscript->SemAnalyze(venv, tenv, labelcount);
  if (expt->kind != TY::Ty::INT) {
    errormsg.Error(this->pos, "Subscript was not an integer");
    return TY::VoidTy::Instance();
  }

  return ((TY::ArrayTy *)vart)->ty;
}

TY::Ty *VarExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  return this->var->SemAnalyze(venv, tenv, labelcount);
}

TY::Ty *NilExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  return TY::NilTy::Instance();
}

TY::Ty *IntExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  return TY::IntTy::Instance();
}

TY::Ty *StringExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  return TY::StringTy::Instance();
}

TY::Ty *CallExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  E::EnvEntry *e = venv->Look(this->func);
  if (!e) {
    errormsg.Error(this->pos, "undefined function %s", this->func->Name().c_str());
    return TY::VoidTy::Instance();
  }
  if (e->kind != E::EnvEntry::FUN) {
    errormsg.Error(this->pos, "'%s' was a variable", this->func->Name().c_str());
    return TY::VoidTy::Instance();
  }

  A::ExpList *actuals = this->args;
  TY::TyList *formals = ((E::FunEntry *)e)->formals;

  while (actuals && formals) {
    if (actuals->head->SemAnalyze(venv, tenv, labelcount) != formals->head) {
      errormsg.Error(this->pos, "para type mismatch");
      return TY::VoidTy::Instance();
    }
    actuals = actuals->tail;
    formals = formals->tail;
  }

  if (formals) {
    errormsg.Error(this->pos, "too few params in function %s", this->func->Name().c_str());
    return TY::VoidTy::Instance();
  }
  if (actuals) {
    errormsg.Error(this->pos, "too many params in function %s", this->func->Name().c_str());
    return TY::VoidTy::Instance();
  }

  return ((E::FunEntry *)e)->result;
}

TY::Ty *OpExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  TY::Ty *left = this->left->SemAnalyze(venv, tenv, labelcount);
  TY::Ty *right = this->right->SemAnalyze(venv, tenv, labelcount);

  switch (this->oper) {
    case A::PLUS_OP:
    case A::MINUS_OP:
    case A::TIMES_OP:
    case A::DIVIDE_OP: {
      if (left->kind == TY::Ty::INT && right->kind == TY::Ty::INT) {
        // divide 0 check ?
        return TY::IntTy::Instance();
      }
      errormsg.Error(this->pos, "integer required");
      return TY::VoidTy::Instance();
    }
    case A::GT_OP:
    case A::GE_OP:
    case A::LT_OP:
    case A::LE_OP: {
      if (left->kind != TY::Ty::INT && left->kind != TY::Ty::STRING) {
        errormsg.Error(this->pos, "string or integer required");  
      }
      else if (left->kind != right->kind) {
        errormsg.Error(this->pos, "same type required");
      }
      return TY::IntTy::Instance();
    }
    case A::EQ_OP:
    case A::NEQ_OP: {
      if (left->kind == TY::Ty::VOID || right->kind == TY::Ty::VOID) {
        errormsg.Error(this->pos, "expression had no value");
      }
      else if (left->kind != right->kind) {
        errormsg.Error(this->pos, "same type required");
      }
      return TY::IntTy::Instance();
    }
    default:
      break;
  }
  return TY::VoidTy::Instance();
}

TY::Ty *RecordExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  TY::Ty *t = tenv->Look(this->typ);
  if (!t) {
    errormsg.Error(this->pos, "undefined type %s", this->typ->Name().c_str());
    return TY::VoidTy::Instance();
  }
  if (t->kind != TY::Ty::RECORD) {
    errormsg.Error(this->pos, "'%s' was not a record type", this->typ->Name().c_str());
    return TY::VoidTy::Instance();
  }

  A::EFieldList *actuals = this->fields;
  TY::FieldList *formals = ((TY::RecordTy *)t)->fields;

  while (actuals && formals) {
    if (actuals->head->name != formals->head->name) {
      errormsg.Error(this->pos, "need member '%s' but '%s'", formals->head->name->Name().c_str(), actuals->head->name->Name().c_str());
      return TY::VoidTy::Instance();
    }
    if (actuals->head->exp->SemAnalyze(venv, tenv, labelcount) != formals->head->ty) {
      errormsg.Error(this->pos, "member '%s' type mismatch", formals->head->name->Name().c_str());
      return TY::VoidTy::Instance();
    }
    actuals = actuals->tail;
    formals = formals->tail;
  }

  if (formals) {
    errormsg.Error(this->pos, "too few initializers for '%s'", this->typ->Name().c_str());
    return TY::VoidTy::Instance();
  }
  if (actuals) {
    errormsg.Error(this->pos, "too many initializers for '%s'", this->typ->Name().c_str());
    return TY::VoidTy::Instance();
  }

  return t;
}

TY::Ty *SeqExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  A::ExpList *seqs = this->seq;

  while (seqs->tail) {
    seqs->head->SemAnalyze(venv, tenv, labelcount);
    seqs = seqs->tail;
  }
  return seqs->head->SemAnalyze(venv, tenv, labelcount);
}

TY::Ty *AssignExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  if (this->var->SemAnalyze(venv, tenv, labelcount) != this->exp->SemAnalyze(venv, tenv, labelcount)) {
    errormsg.Error(this->pos, "unmatched assign exp");
  }

  if (loopv[labelcount] && loopv[labelcount] == venv->Look(((A::SimpleVar *)this->var)->sym)) {
    errormsg.Error(this->pos, "loop variable can't be assigned");
  }

  return TY::VoidTy::Instance();
}

TY::Ty *IfExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  if (this->test->SemAnalyze(venv, tenv, labelcount)->kind != TY::Ty::INT) {
    errormsg.Error(this->pos, "if-exp was not an integer");
    return TY::VoidTy::Instance();
  }

  TY::Ty *thent = this->then->SemAnalyze(venv, tenv, labelcount);
  if (this->elsee) {
    TY::Ty *elset = this->elsee->SemAnalyze(venv, tenv, labelcount);
    if (thent != elset) {
      errormsg.Error(this->pos, "then exp and else exp type mismatch");
    }
    if (thent->kind != TY::Ty::VOID || elset->kind != TY::Ty::VOID) {
      errormsg.Error(this->pos, "if-then exp's body must produce no value");
    }
    return thent;
  }

  if (thent->kind != TY::Ty::VOID) {
    errormsg.Error(this->pos, "if-then exp's body must produce no value");
    return TY::VoidTy::Instance();
  }
  return thent;
}

TY::Ty *WhileExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  if (this->test->SemAnalyze(venv, tenv, labelcount)->kind != TY::Ty::INT) {
    errormsg.Error(this->pos, "while-exp was not an integer");
  }

  if (this->body->SemAnalyze(venv, tenv, labelcount + 1)->kind != TY::Ty::VOID) {
    errormsg.Error(this->pos, "while body must produce no value");
    return TY::VoidTy::Instance();
  }

  return TY::VoidTy::Instance();
}

TY::Ty *ForExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  if (this->lo->SemAnalyze(venv, tenv, labelcount)->kind != TY::Ty::INT || this->hi->SemAnalyze(venv, tenv, labelcount)->kind != TY::Ty::INT) {
    errormsg.Error(this->pos, "for exp's range type is not integer");
  }

  venv->BeginScope();
  tenv->BeginScope();

  E::VarEntry *e = new E::VarEntry(TY::IntTy::Instance());
  loopv[labelcount + 1] = e;
  venv->Enter(this->var, e);
  TY::Ty *b = this->body->SemAnalyze(venv, tenv, labelcount + 1);

  venv->EndScope();
  tenv->EndScope();

  if (b->kind != TY::Ty::VOID) {
    errormsg.Error(this->pos, "body exp shouldn't return a value");
  }
  return TY::VoidTy::Instance();
}

TY::Ty *BreakExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  if (labelcount == 0) {
    errormsg.Error(this->pos, "break statement not within loop");
  }
  return TY::VoidTy::Instance();
}

TY::Ty *LetExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  A::DecList *decs = this->decs;
  venv->BeginScope();
  tenv->BeginScope();

  while (decs) {
    decs->head->SemAnalyze(venv, tenv, labelcount);
    decs = decs->tail;
  }
  TY::Ty *result = this->body->SemAnalyze(venv, tenv, labelcount);

  venv->EndScope();
  tenv->EndScope();
  return result;
}

TY::Ty *ArrayExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  TY::Ty *t = tenv->Look(this->typ);
  if (!t) {
    errormsg.Error(this->pos, "undefined type %s", this->typ->Name().c_str());
    return TY::VoidTy::Instance();
  }
  if (t->kind != TY::Ty::ARRAY) {
    errormsg.Error(this->pos, "'%s' was not a array type", this->typ->Name().c_str());
    return TY::VoidTy::Instance();
  }

  if (this->size->SemAnalyze(venv, tenv, labelcount)->kind != TY::Ty::INT) {
    errormsg.Error(this->pos, "array size was not an integer value");
  }

  if (((TY::ArrayTy *)t)->ty != this->init->SemAnalyze(venv, tenv, labelcount)) {
    errormsg.Error(this->pos, "array init type mismatch");
  }

  return t;
}

TY::Ty *VoidExp::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  return TY::VoidTy::Instance();
}

void FunctionDec::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
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
    venv->Enter(list->head->name, new E::FunEntry(formalst, result));
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

    venv->BeginScope();

    A::FieldList *formals = list->head->params;
    while (formals) {
      TY::Ty *t = tenv->Look(formals->head->typ);
      if (!t)
        t = TY::IntTy::Instance();
      venv->Enter(formals->head->name, new E::VarEntry(t));
      formals = formals->tail;
    }

    TY::Ty *result = list->head->body->SemAnalyze(venv, tenv, labelcount);
    if (result->kind != TY::Ty::VOID && ((E::FunEntry *)venv->Look(list->head->name))->result->kind == TY::Ty::VOID) {
      errormsg.Error(this->pos, "procedure returns value", list->head->name->Name());
    }
    if (result != ((E::FunEntry *)venv->Look(list->head->name))->result) {
      errormsg.Error(this->pos, "body result type mismatch");
    }

    venv->EndScope();
    list = list->tail;
  }
}

void VarDec::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
  TY::Ty *initt = this->init->SemAnalyze(venv, tenv, labelcount);
  if (this->typ) {
    TY::Ty *typt = tenv->Look(this->typ);
    if (!typt) {
      errormsg.Error(this->pos, "undefined type %s", this->typ->Name().c_str());
    }
    if (typt != initt) {
      errormsg.Error(this->pos, "var init type mismatch");
      venv->Enter(this->var, new E::VarEntry(typt));
      return;
    }
  }
  if (initt->kind == TY::Ty::VOID) {
    errormsg.Error(this->pos, "initialize with no value");
  }
  else if (initt->kind == TY::Ty::NIL) {
    errormsg.Error(this->pos, "init should not be nil without type specified");
  }
  venv->Enter(this->var, new E::VarEntry(initt));
}

void TypeDec::SemAnalyze(VEnvType venv, TEnvType tenv, int labelcount) const {
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
    ((TY::NameTy *)t)->ty = list->head->ty->SemAnalyze(tenv);
    list = list->tail;
  }

  for (list = this->types; list; list = list->tail) {
    TY::Ty *t = tenv->Look(list->head->name);
    if (t->kind == TY::Ty::VOID) {
      return;
    }
    t = ((TY::NameTy *)t)->ty;
    s.clear();
    while (t && t->kind == TY::Ty::NAME) {
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
}

TY::Ty *NameTy::SemAnalyze(TEnvType tenv) const {
  TY::Ty *t = tenv->Look(this->name);
  errormsg.Error(this->pos, "&* %s", this->name->Name().c_str());
  if (t)
  {
    return new TY::NameTy(this->name, t);
  }
  errormsg.Error(this->pos, "undefined type %s", this->name->Name().c_str());
  return TY::VoidTy::Instance();
}

TY::Ty *RecordTy::SemAnalyze(TEnvType tenv) const {
  return new TY::RecordTy(make_fieldlist(tenv, this->record));
}

TY::Ty *ArrayTy::SemAnalyze(TEnvType tenv) const {
  TY::Ty *t = tenv->Look(this->array);
  if (t) {
    return new TY::ArrayTy(t);
  }
  errormsg.Error(this->pos, "undefined type %s", this->array->Name().c_str());
  return TY::VoidTy::Instance();
}

}  // namespace A

namespace SEM {
void SemAnalyze(A::Exp *root) {
  if (root) root->SemAnalyze(E::BaseVEnv(), E::BaseTEnv(), 0);
}

}  // namespace SEM
