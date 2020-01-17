#ifndef TIGER_CODEGEN_CODEGEN_H_
#define TIGER_CODEGEN_CODEGEN_H_

#include "tiger/codegen/assem.h"
#include "tiger/frame/frame.h"
#include "tiger/translate/tree.h"

namespace CG {

AS::InstrList* Codegen(F::Frame* f, T::StmList* stmList);
void emit(AS::Instr *instr);
void munchStm(T::Stm *stm);
TEMP::Temp *munchExp(T::Exp *exp);
int munchArgs(T::ExpList *l, bool reg);

}
#endif