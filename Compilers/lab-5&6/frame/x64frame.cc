#include "tiger/frame/frame.h"

#include <string>
#include <stdlib.h>

namespace F {

const int wordSize = 8;
const int keepNum = 6;

TEMP::Temp *FP() {
  static TEMP::Temp *t = nullptr;
  if (!t)
    t = TEMP::Temp::NewTemp();
  return t;
}

T::Exp *ExternalCall(std::string s, T::ExpList *args) {
  return new T::CallExp(new T::NameExp(TEMP::NamedLabel(s)), args);
}

F::Frame *NewFrame(TEMP::Label *name, U::BoolList *formals) {
  Frame *f = new F::Frame(name);
  AccessList *head = nullptr;
  AccessList *tail = nullptr;

  int rn = 0;
  for (U::BoolList *ptr = formals; ptr; ptr = ptr->tail) {
    Access *ac = nullptr;
    if (rn < keepNum && !(ptr->head)) {
      ac = new InRegAccess(TEMP::Temp::NewTemp());
      rn++;
    } 
    else {
      f->localCount++;
      ac = new InFrameAccess(-(f->localCount) * wordSize);
    }

    if (head) {
      tail->tail = new AccessList(ac, nullptr);
      tail = tail->tail;
    }
    else {
      head = new AccessList(ac, nullptr);
      tail = head;
    }
  }
  f->formals = head;
  return (F::Frame *)f;
}


AS::Proc *procEntryExit3(Frame *f, AS::InstrList *il) {
  int fsize = f->localCount * 8;
  // AS::Proc proc =
  std::string prolog;
  prolog += "subq $";
  prolog += std::to_string(fsize);
  prolog += ", %rsp\n";

  std::string epilog;
  epilog += "addq $";
  epilog += std::to_string(fsize);
  epilog += ", %rsp\nret\n";

  AS::Proc *proc = new AS::Proc(prolog, il, epilog);
  return proc;
}

Access *Frame::AllocLocal(bool escape) {
  if (escape) {
    this->localCount++;
    return new InFrameAccess(-wordSize * (this->localCount));
    // maybe append to f->formals
  }
  else {
    return new InRegAccess(TEMP::Temp::NewTemp());
  }
}

T::Exp *InFrameAccess::ToExp(T::Exp *framePtr) const {
  return new T::MemExp(new T::BinopExp(T::PLUS_OP, new T::ConstExp(this->offset), framePtr));
}

T::Exp *InRegAccess::ToExp(T::Exp *framePtr) const {
  return new T::TempExp(this->reg);
}

TEMP::Temp *RAX(void) { static TEMP::Temp *t = nullptr; if (!t) t = TEMP::Temp::NewTemp(); return t; }
TEMP::Temp *RBX(void) { static TEMP::Temp *t = nullptr; if (!t) t = TEMP::Temp::NewTemp(); return t; }
TEMP::Temp *RCX(void) { static TEMP::Temp *t = nullptr; if (!t) t = TEMP::Temp::NewTemp(); return t; }
TEMP::Temp *RDX(void) { static TEMP::Temp *t = nullptr; if (!t) t = TEMP::Temp::NewTemp(); return t; }
TEMP::Temp *RSI(void) { static TEMP::Temp *t = nullptr; if (!t) t = TEMP::Temp::NewTemp(); return t; }
TEMP::Temp *RDI(void) { static TEMP::Temp *t = nullptr; if (!t) t = TEMP::Temp::NewTemp(); return t; }
TEMP::Temp *RBP(void) { static TEMP::Temp *t = nullptr; if (!t) t = TEMP::Temp::NewTemp(); return t; }
TEMP::Temp *RSP(void) { static TEMP::Temp *t = nullptr; if (!t) t = TEMP::Temp::NewTemp(); return t; }
TEMP::Temp *R8(void) { static TEMP::Temp *t = nullptr; if (!t) t = TEMP::Temp::NewTemp(); return t; }
TEMP::Temp *R9(void) { static TEMP::Temp *t = nullptr; if (!t) t = TEMP::Temp::NewTemp(); return t; }
TEMP::Temp *R10(void) { static TEMP::Temp *t = nullptr; if (!t) t = TEMP::Temp::NewTemp(); return t; }
TEMP::Temp *R11(void) { static TEMP::Temp *t = nullptr; if (!t) t = TEMP::Temp::NewTemp(); return t; }
TEMP::Temp *R12(void) { static TEMP::Temp *t = nullptr; if (!t) t = TEMP::Temp::NewTemp(); return t; }
TEMP::Temp *R13(void) { static TEMP::Temp *t = nullptr; if (!t) t = TEMP::Temp::NewTemp(); return t; }
TEMP::Temp *R14(void) { static TEMP::Temp *t = nullptr; if (!t) t = TEMP::Temp::NewTemp(); return t; }
TEMP::Temp *R15(void) { static TEMP::Temp *t = nullptr; if (!t) t = TEMP::Temp::NewTemp(); return t; }

}  // namespace F