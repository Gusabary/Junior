#include "tiger/escape/escape.h"

namespace ESC {

class EscapeEntry {
 public:
  int depth;
  bool* escape;

  EscapeEntry(int depth, bool* escape) : depth(depth), escape(escape) {}
};

static void traverseExp(S::Table<EscapeEntry> *env, int depth, A::Exp *e);
static void traverseDec(S::Table<EscapeEntry> *env, int depth, A::Dec *d);
static void traverseVar(S::Table<EscapeEntry> *env, int depth, A::Var *v);

void FindEscape(A::Exp* exp) {
  S::Table<EscapeEntry> *env = new S::Table<EscapeEntry>();
  traverseExp(env, 0, exp);
}

static void traverseExp(S::Table<EscapeEntry> *env, int depth, A::Exp *e) {
  switch (e->kind) {
    case A::Exp::VAR: {
      traverseVar(env, depth, ((A::VarExp *)e)->var);
      return;
    }
    case A::Exp::CALL: {
      A::ExpList *el = ((A::CallExp *)e)->args;
      for (; el; el = el->tail) {
        traverseExp(env, depth, el->head);
      }
      return;
    }
    case A::Exp::OP: {
      traverseExp(env, depth, ((A::OpExp *)e)->left);
      traverseExp(env, depth, ((A::OpExp *)e)->right);
      return;
    }
    case A::Exp::RECORD: {
      A::EFieldList *efl = ((A::RecordExp *)e)->fields;
      for (; efl; efl = efl->tail) {
        traverseExp(env, depth, efl->head->exp);
      }
      return;
    }
    case A::Exp::SEQ: {
      A::ExpList *el = ((A::SeqExp *)e)->seq;
      for (; el; el = el->tail) {
        traverseExp(env, depth, el->head);
      }
      return;
    }
    case A::Exp::ASSIGN: {
      traverseVar(env, depth, ((A::AssignExp *)e)->var);
      traverseExp(env, depth, ((A::AssignExp *)e)->exp);
      return;
    }
    case A::Exp::IF: {
      traverseExp(env, depth, ((A::IfExp *)e)->test);
      traverseExp(env, depth, ((A::IfExp *)e)->then);
      if (((A::IfExp *)e)->elsee)
        traverseExp(env, depth, ((A::IfExp *)e)->elsee);
      return;
    }
    case A::Exp::WHILE: {
      traverseExp(env, depth, ((A::WhileExp *)e)->test);
      traverseExp(env, depth, ((A::WhileExp *)e)->body);
      return;
    }
    case A::Exp::FOR: {
      ((A::ForExp *)e)->escape = false;
      env->Enter(((A::ForExp *)e)->var, new EscapeEntry(depth, &(((A::ForExp *)e)->escape)));
      traverseExp(env, depth, ((A::ForExp *)e)->lo);
      traverseExp(env, depth, ((A::ForExp *)e)->hi);
      env->BeginScope();
      traverseExp(env, depth, ((A::ForExp *)e)->body);
      env->EndScope();
      return;
    }
    case A::Exp::LET: {
      env->BeginScope();
      A::DecList *lt = ((A::LetExp *)e)->decs;
      for (; lt; lt = lt->tail) {
        traverseDec(env, depth, lt->head);
      }
      traverseExp(env, depth, ((A::LetExp *)e)->body);
      env->EndScope();
      return;
    }
    case A::Exp::ARRAY: {
      traverseExp(env, depth, ((A::ArrayExp *)e)->size);
      traverseExp(env, depth, ((A::ArrayExp *)e)->init);
      return;
    }
    default:
      return;
  }
  assert(0);
}

static void traverseDec(S::Table<EscapeEntry> *env, int depth, A::Dec *d) {
  switch (d->kind) {
    case A::Dec::VAR: {
      traverseExp(env, depth, ((A::VarDec *)d)->init);
      ((A::VarDec *)d)->escape = false;
      env->Enter(((A::VarDec *)d)->var, new EscapeEntry(depth, &(((A::VarDec *)d)->escape)));
      return;
    }
    case A::Dec::TYPE:
      return;
    case A::Dec::FUNCTION: {
      A::FunDecList *fl = ((A::FunctionDec *)d)->functions;
      for (; fl; fl = fl->tail) {
        A::FunDec *fun = fl->head;
        A::FieldList *el = fun->params;
        env->BeginScope();
        for (; el; el = el->tail) {
          el->head->escape = false;
          env->Enter(el->head->name, new EscapeEntry(depth + 1, &(el->head->escape)));
        }
        traverseExp(env, depth + 1, fun->body);
        env->EndScope();
      }
      return;
    }
  }
  assert(0);
}

static void traverseVar(S::Table<EscapeEntry> *env, int depth, A::Var *v) {
  switch (v->kind) {
    case A::Var::SIMPLE: {
      EscapeEntry *e = env->Look(((A::SimpleVar *)v)->sym);
      if (e->depth < depth)
        *(e->escape) = true;
      return;
    }
    case A::Var::FIELD: {
      traverseVar(env, depth, ((A::FieldVar *)v)->var);
      return;
    }
    case A::Var::SUBSCRIPT: {
      traverseVar(env, depth, ((A::SubscriptVar *)v)->var);
      traverseExp(env, depth, ((A::SubscriptVar *)v)->subscript);
      return;
    }
    assert(0);
  }
}

}  // namespace ESC