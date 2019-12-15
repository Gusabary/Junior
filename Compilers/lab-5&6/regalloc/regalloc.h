#ifndef TIGER_REGALLOC_REGALLOC_H_
#define TIGER_REGALLOC_REGALLOC_H_

#include "tiger/codegen/assem.h"
#include "tiger/frame/frame.h"
#include "tiger/frame/temp.h"
#include "tiger/regalloc/color.h"

namespace RA {

class Result {
 public:
  TEMP::Map* coloring;
  AS::InstrList* il;

  Result(AS::InstrList *il) : il(il), coloring(nullptr) {};
};

Result RegAlloc(F::Frame* f, AS::InstrList* il);

bool hasTemp(TEMP::TempList *list, TEMP::Temp *temp);

void replaceTemp(TEMP::TempList *list, TEMP::Temp *old, TEMP::Temp *neww);

AS::InstrList *rewriteProgram(F::Frame *f, AS::InstrList *il, TEMP::TempList *spills);

}  // namespace RA

#endif