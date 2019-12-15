#ifndef TIGER_FRAME_FRAME_H_
#define TIGER_FRAME_FRAME_H_

#include <string>

#include "tiger/codegen/assem.h"
#include "tiger/translate/tree.h"
#include "tiger/util/util.h"

namespace F {
  
extern const int wordSize;
extern const int keepNum;

class Access {
 public:
  enum Kind { INFRAME, INREG };

  Kind kind;

  Access(Kind kind) : kind(kind) {}

  // Hints: You may add interface like
  //        `virtual T::Exp* ToExp(T::Exp* framePtr) const = 0`
  virtual T::Exp *ToExp(T::Exp *framePtr) const = 0;
};

class InFrameAccess : public Access {
 public:
  int offset;

  InFrameAccess(int offset) : Access(INFRAME), offset(offset) {}
  T::Exp *ToExp(T::Exp *framePtr) const;
};

class InRegAccess : public Access {
 public:
  TEMP::Temp* reg;

  InRegAccess(TEMP::Temp *reg) : Access(INREG), reg(reg) {}
  T::Exp *ToExp(T::Exp *framePtr) const;
};

class AccessList {
 public:
  Access *head;
  AccessList *tail;

  AccessList(Access *head, AccessList *tail) : head(head), tail(tail) {}
};

class Frame {
  // Base class
public:
  F::AccessList *formals;
  // F::AccessList *locals;
  int localCount;
  TEMP::Label *label;

  Frame(TEMP::Label *name) : label(name), localCount(0), formals(nullptr) {}
  Access *AllocLocal(bool escape);
};

/*
 * Fragments
 */

class Frag {
 public:
  enum Kind { STRING, PROC };

  Kind kind;

  Frag(Kind kind) : kind(kind) {}
};

class StringFrag : public Frag {
 public:
  TEMP::Label *label;
  std::string str;

  StringFrag(TEMP::Label *label, std::string str)
      : Frag(STRING), label(label), str(str) {}
};

class ProcFrag : public Frag {
 public:
  T::Stm *body;
  Frame *frame;

  ProcFrag(T::Stm *body, Frame *frame) : Frag(PROC), body(body), frame(frame) {}
};

class FragList {
 public:
  Frag *head;
  FragList *tail;

  FragList(Frag *head, FragList *tail) : head(head), tail(tail) {}
};

TEMP::Temp *RAX(void);
TEMP::Temp *RBX(void);
TEMP::Temp *RCX(void);
TEMP::Temp *RDX(void);
TEMP::Temp *RSI(void);
TEMP::Temp *RDI(void);
TEMP::Temp *RBP(void);
TEMP::Temp *RSP(void);
TEMP::Temp *R8(void);
TEMP::Temp *R9(void);
TEMP::Temp *R10(void);
TEMP::Temp *R11(void);
TEMP::Temp *R12(void);
TEMP::Temp *R13(void);
TEMP::Temp *R14(void);
TEMP::Temp *R15(void);

TEMP::Temp *FP();

T::Exp *ExternalCall(std::string s, T::ExpList *args);

F::Frame *NewFrame(TEMP::Label *name, U::BoolList *formals);

AS::Proc *procEntryExit3(Frame *f, AS::InstrList *il);

}  // namespace F

#endif