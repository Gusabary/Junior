#ifndef TIGER_TRANSLATE_TRANSLATE_H_
#define TIGER_TRANSLATE_TRANSLATE_H_

#include "tiger/absyn/absyn.h"
#include "tiger/frame/frame.h"

/* Forward Declarations */
namespace A {
class Exp;
// enum Oper;
// using Oper;
enum Oper : unsigned int;
} // namespace A

namespace TR {

class Access;
class AccessList;
class Exp;
class ExpList;
class ExpAndTy;
class Level;

Level* Outermost();
T::Exp *traceLink(Level *callee, Level *target);
Access *AllocLocal(Level *level, bool escape);
void procEntryExit(Level *level, Exp *body, AccessList *formals);
// T::ExpList *transExpList(TR::)

F::FragList* TranslateProgram(A::Exp*);

Exp *Null();
Exp *SimpleVar(Access *acc, Level *lev);
Exp *FieldVar(Exp *base, int offset);
Exp *SubscriptVar(Exp *base, Exp *index);
Exp *NilExp();
Exp *IntExp(int val);
Exp *StringExp(std::string str);
Exp *CallExp(S::Symbol *fun, ExpList *el, Level *lev, Level *call);
Exp *ArithExp(A::Oper oper, Exp *left, Exp *right);
Exp *RelExp(A::Oper oper, Exp *left, Exp *right);
Exp *EqStrExp(A::Oper oper, Exp *left, Exp *right);
// Exp *EqRefExp(A::Oper oper, Exp *left, Exp *right);
Exp *RecordExp(int num, ExpList *l);
Exp *ArrayExp(Exp *size, Exp *init);
Exp *SeqExp(ExpList *l);
Exp *AssignExp(Exp *left, Exp *right);
Exp *IfExp(Exp *test, Exp *then, Exp *elsee);
Exp *WhileExp(Exp *test, Exp *body, Exp *done);
Exp *ForExp(Level *lev, Access *iac, Exp *lo, Exp *hi, Exp *body, Exp *done);
Exp *DoneExp();
Exp *BreakExp(Exp *done);

}  // namespace TR

#endif
