#include "tiger/codegen/codegen.h"
#include <cstdio>
#include <string>
namespace CG {


static AS::InstrList *cur = nullptr;
static const int MAXLINE = 100;
static F::Frame *frame;

static TEMP::TempList *argregs = new TEMP::TempList(F::RDI(), new TEMP::TempList(F::RSI(), new TEMP::TempList(F::RDX(),
    new TEMP::TempList(F::RCX(), new TEMP::TempList(F::R8(), new TEMP::TempList(F::R9(), nullptr))))));

static TEMP::TempList *caller_save = new TEMP::TempList(F::RAX(), new TEMP::TempList(F::R10(), new TEMP::TempList(F::R11(), argregs)));

static TEMP::TempList *callee_save = new TEMP::TempList(F::RBX(), new TEMP::TempList(F::RBP(), new TEMP::TempList(F::R12(),
    new TEMP::TempList(F::R13(), new TEMP::TempList(F::R14(), new TEMP::TempList(F::R15(), nullptr))))));

static const int fkeep = 6;

AS::InstrList* Codegen(F::Frame* f, T::StmList* stmList) {
  // TODO: Put your codes here (lab6).
  cur = new AS::InstrList(nullptr, nullptr);
  AS::InstrList *instr_list = cur;
  frame = f;

  // callee saved registers
  TEMP::TempList *csr = callee_save;
  TEMP::TempList *csr_cur = new TEMP::TempList(TEMP::Temp::NewTemp(), nullptr);
  TEMP::TempList *csr_bak = csr_cur;

  for (; csr; csr = csr->tail) {
    emit(new AS::MoveInstr("movq `s0, `d0  #1 callee save", new TEMP::TempList(csr_cur->head, nullptr), new TEMP::TempList(csr->head, nullptr)));
    csr_cur->tail = new TEMP::TempList(TEMP::Temp::NewTemp(), nullptr);
    csr_cur = csr_cur->tail;
  }

  F::AccessList *formals = f->formals;
  int cn = 0;
  for (int rn = 0; rn < 6 && formals; rn++, formals = formals->tail) {
    TEMP::Temp *st;
    switch (rn) {
      case 0:
        st = F::RDI(); break;
      case 1:
        st = F::RSI(); break;
      case 2:
        st = F::RDX(); break;
      case 3:
        st = F::RCX(); break;
      case 4:
        st = F::R8(); break;
      case 5:
        st = F::R9(); break;
    }
    if (formals->head->kind == F::Access::INFRAME) {
      TEMP::Temp *d = TEMP::Temp::NewTemp();
      emit(new AS::OperInstr("movq `s0, `d0  #2", new TEMP::TempList(d, nullptr), new TEMP::TempList(F::RSP(), nullptr), new AS::Targets(nullptr)));
      std::string instr;
      instr += "addq $";
      instr += TEMP::LabelString(frame->label);
      instr += "framesize, `d0  #3";
      emit(new AS::OperInstr(instr, new TEMP::TempList(d, nullptr), nullptr, new AS::Targets(nullptr)));

      cn++;
      instr.clear();
      instr += "movq `s0, ";
      instr += std::to_string(-cn * F::wordSize);
      instr += "(`d0)  #4";
      emit(new AS::OperInstr(instr, new TEMP::TempList(d, nullptr), new TEMP::TempList(st, nullptr), new AS::Targets(nullptr)));
    }
    else {
      emit(new AS::MoveInstr("movq `s0, `d0  #5", new TEMP::TempList(((F::InRegAccess *)(formals->head))->reg, nullptr), new TEMP::TempList(st, nullptr)));
    }
  }

  while (stmList) {
    munchStm(stmList->head);
    stmList = stmList->tail;
  }

  csr = callee_save;
  csr_cur = csr_bak;
  for (; csr; csr = csr->tail) {
    emit(new AS::MoveInstr("movq `s0, `d0  #6", new TEMP::TempList(csr->head, nullptr), new TEMP::TempList(csr_cur->head, nullptr)));
    csr_cur = csr_cur->tail;
  }

  return instr_list->tail;
}

void emit(AS::Instr *instr) {
  if (!cur)
  {
    cur = new AS::InstrList(instr, nullptr);
  }
  else {
    cur->tail = new AS::InstrList(instr, nullptr);
    cur = cur->tail;
  }
}

void munchStm(T::Stm *stm) {
  switch (stm->kind)
  {
  case T::Stm::SEQ:
    munchStm(((T::SeqStm *)stm)->left);
    munchStm(((T::SeqStm *)stm)->right);
    break;
  case T::Stm::LABEL:
    emit(new AS::LabelInstr(TEMP::LabelString(((T::LabelStm *)stm)->label), ((T::LabelStm *)stm)->label));
    break;
  case T::Stm::JUMP:
    emit(new AS::OperInstr("jmp `j0  #7", nullptr, nullptr, new AS::Targets(((T::JumpStm *)stm)->jumps)));
    break;
  case T::Stm::CJUMP:
  {
    T::Exp *left = ((T::CjumpStm *)stm)->left;
    T::Exp *right = ((T::CjumpStm *)stm)->right;
    emit(new AS::OperInstr("cmp `s1, `s0  #8", nullptr, new TEMP::TempList(munchExp(left), new TEMP::TempList(munchExp(right), nullptr)), new AS::Targets(nullptr)));
    std::string instr;
    switch (((T::CjumpStm *)stm)->op)
    {
    case T::EQ_OP:
    {
      instr = "je `j0";
      break;
    }
    case T::NE_OP:
    {
      instr = "jne `j0";
      break;
    }
    case T::LT_OP:
    {
      instr = "jl `j0";
      break;
    }
    case T::LE_OP:
    {
      instr = "jle `j0";
      break;
    }
    case T::GT_OP:
    {
      instr = "jg `j0";
      break;
    }
    case T::GE_OP:
    {
      instr = "jge `j0";
      break;
    }
      // need unsigned compare?
    }
    emit(new AS::OperInstr(instr, nullptr, nullptr, new AS::Targets(new TEMP::LabelList(((T::CjumpStm *)stm)->true_label, nullptr))));
    break;
    }
    case T::Stm::MOVE: {
      T::Exp *dst = ((T::MoveStm *)stm)->dst;
      T::Exp *src = ((T::MoveStm *)stm)->src;
      if (dst->kind == T::Exp::MEM) {
        if (((T::MemExp *)dst)->exp->kind == T::Exp::BINOP) {
          std::string instr;
          instr.reserve(MAXLINE);
          if (((T::BinopExp *)(((T::MemExp *)dst)->exp))->right->kind == T::Exp::CONST) {
            T::Exp *e1 = ((T::BinopExp *)(((T::MemExp *)dst)->exp))->left;
            T::Exp *e2 = src;
            instr += "movq `s0, ";
            instr += std::to_string(((T::ConstExp *)(((T::BinopExp *)(((T::MemExp *)dst)->exp))->right))->consti);
            instr += "(`s1)  #9";
            emit(new AS::OperInstr(instr, nullptr, new TEMP::TempList(munchExp(e2), new TEMP::TempList(munchExp(e1), nullptr)), new AS::Targets(nullptr)));
            break;
          }
          else if (((T::BinopExp *)(((T::MemExp *)dst)->exp))->left->kind == T::Exp::CONST) {
            T::Exp *e1 = ((T::BinopExp *)(((T::MemExp *)dst)->exp))->right;
            T::Exp *e2 = src;
            instr += "movq `s0, ";
            instr += std::to_string(((T::ConstExp *)(((T::BinopExp *)(((T::MemExp *)dst)->exp))->left))->consti);
            instr += "(`s1)  #10";
            emit(new AS::OperInstr(instr, nullptr, new TEMP::TempList(munchExp(e2), new TEMP::TempList(munchExp(e1), nullptr)), new AS::Targets(nullptr)));
            break;
          }
          else {
            emit(new AS::OperInstr("movq `s0, (`s1)  #11", nullptr, new TEMP::TempList(munchExp(src), new TEMP::TempList(munchExp(((T::MemExp *)dst)->exp), nullptr)), new AS::Targets(nullptr)));
            break;
          }
        }
        else {
          emit(new AS::OperInstr("movq `s0, (`s1)  #12", nullptr, new TEMP::TempList(munchExp(src), new TEMP::TempList(munchExp(((T::MemExp *)dst)->exp), nullptr)), new AS::Targets(nullptr)));
          break;
        }
      }
      else if (dst->kind == T::Exp::TEMP) {
        emit(new AS::MoveInstr("movq `s0, `d0  #13", new TEMP::TempList(munchExp(dst), nullptr), new TEMP::TempList(munchExp(src), nullptr)));
        break;
      }
    }
    case T::Stm::EXP: {
      munchExp(((T::ExpStm *)stm)->exp);
      break;
    }
  }
}

TEMP::Temp *munchExp(T::Exp *exp) {
  TEMP::Temp *d = TEMP::Temp::NewTemp();
  switch (exp->kind) {
    case T::Exp::BINOP: {
      TEMP::Temp *left = munchExp(((T::BinopExp *)exp)->left);
      TEMP::Temp *right = munchExp(((T::BinopExp *)exp)->right);
      std::string opinstr;
      switch (((T::BinopExp *)exp)->op) {
        case T::PLUS_OP: {
          opinstr = "addq `s0, `d0  #14";
          break;
        }
        case T::MINUS_OP: {
          opinstr = "subq `s0, `d0  #15";
          break;
        }
        case T::MUL_OP: {
          opinstr = "imulq `s0, `d0  #16";
          break;
        }
        case T::DIV_OP: {
          emit(new AS::MoveInstr("movq `s0, `d0", new TEMP::TempList(F::RAX(), nullptr), new TEMP::TempList(left, nullptr)));
          emit(new AS::OperInstr("cqto", new TEMP::TempList(F::RDX(), new TEMP::TempList(F::RAX(), nullptr)), new TEMP::TempList(F::RAX(), nullptr), new AS::Targets(nullptr)));
          emit(new AS::OperInstr("idivq `s0", new TEMP::TempList(F::RDX(), new TEMP::TempList(F::RAX(), nullptr)), new TEMP::TempList(right, nullptr), new AS::Targets(nullptr)));
          emit(new AS::MoveInstr("movq `s0, `d0", new TEMP::TempList(d, nullptr), new TEMP::TempList(F::RAX(), nullptr)));
          return d;
        }
        case T::AND_OP:
        case T::OR_OP:
        case T::LSHIFT_OP:
        case T::RSHIFT_OP:
        case T::ARSHIFT_OP:
        case T::XOR_OP:
          assert(0);
      }
      emit(new AS::MoveInstr("movq `s0, `d0  #17", new TEMP::TempList(d, nullptr), new TEMP::TempList(left, nullptr)));
      emit(new AS::OperInstr(opinstr, new TEMP::TempList(d, nullptr), new TEMP::TempList(right, new TEMP::TempList(d, nullptr)), new AS::Targets(nullptr)));
      break;
    }
    case T::Exp::MEM: {
      if (((T::MemExp *)exp)->exp->kind == T::Exp::BINOP && ((T::TempExp *)(((T::BinopExp *)(((T::MemExp *)exp)->exp))->right))->temp == F::FP()) {
        std::string instr;
        instr += "movq ";
        instr += TEMP::LabelString(frame->label);
        instr += "framesize + ";
        instr += std::to_string(((T::ConstExp *)(((T:: BinopExp *)(((T::MemExp *)exp)->exp))->left))->consti);
        instr += "(`s0), `d0  #18";
        emit(new AS::OperInstr(instr, new TEMP::TempList(d, nullptr), new TEMP::TempList(F::RSP(), nullptr), new AS::Targets(nullptr)));
      }
      else if (((T::MemExp *)exp)->exp->kind == T::Exp::BINOP && (((T::BinopExp *)(((T::MemExp *)exp)->exp))->left)->kind == T::Exp::CONST) {
        std::string instr;
        instr += "movq ";
        instr += std::to_string(((T::ConstExp *)(((T:: BinopExp *)(((T::MemExp *)exp)->exp))->left))->consti);
        instr += "(`s0), `d0  #19";
        emit(new AS::OperInstr(instr, new TEMP::TempList(d, nullptr), new TEMP::TempList(munchExp(((T::BinopExp *)(((T::MemExp *)exp)->exp))->right), nullptr), new AS::Targets(nullptr)));
      }
      else {
        emit(new AS::OperInstr("movq (`s0), `d0  #20", new TEMP::TempList(d, nullptr), new TEMP::TempList(munchExp(((T::MemExp *)exp)->exp), nullptr), new AS::Targets(nullptr)));
      }
      break;
    }
    case T::Exp::TEMP: {
      if (((T::TempExp *)exp)->temp == F::FP()) {
        emit(new AS::OperInstr("movq `s0, `d0  #21", new TEMP::TempList(d, nullptr), new TEMP::TempList(F::RSP(), nullptr), new AS::Targets(nullptr)));
        std::string instr;
        instr += "addq $";
        instr += TEMP::LabelString(frame->label);
        instr += "framesize, `d0  #22";
        emit(new AS::OperInstr(instr, new TEMP::TempList(d, nullptr), nullptr, new AS::Targets(nullptr)));
      }
      else {
        d = ((T::TempExp *)exp)->temp;
      }
      break;
    }
    case T::Exp::ESEQ: {
      // canonical tree
      assert(0);
    }
    case T::Exp::NAME: {
      std::string instr;
      instr += "leaq ";
      instr += TEMP::LabelString(((T::NameExp *)exp)->name);
      instr += "(%rip), `d0  #23";
      emit(new AS::OperInstr(instr, new TEMP::TempList(d, nullptr), nullptr, new AS::Targets(nullptr)));
      break;
    }
    case T::Exp::CONST: {
      std::string instr;
      instr += "movq $";
      instr += std::to_string(((T::ConstExp *)exp)->consti);
      instr += ", `d0  #24";
      emit(new AS::OperInstr(instr, new TEMP::TempList(d, nullptr), nullptr, new AS::Targets(nullptr)));
      break;
    }
    case T::Exp::CALL: {
      TEMP::Label *func = ((T::NameExp *)(((T::CallExp *)exp)->fun))->name;
      int push = munchArgs(((T::CallExp *)exp)->args, true);
      std::string instr;
      instr += "call ";
      instr += TEMP::LabelString(func);
      emit(new AS::OperInstr(instr, caller_save, nullptr, new AS::Targets(nullptr)));

      if (push) {
        std::string instr;
        instr += "addq $";
        instr += push * 8;
        instr += ", %rsp  #25";
        emit(new AS::OperInstr(instr, nullptr, nullptr, new AS::Targets(nullptr)));
      }
      emit(new AS::MoveInstr("movq `s0, `d0  #26", new TEMP::TempList(d, nullptr), new TEMP::TempList(F::RAX(), nullptr)));
      break;
    }
  }
  return d;
}

int munchArgs(T::ExpList *l, bool reg) {
  int cpush = 0;
  if (reg && l) {
    TEMP::TempList *regs = argregs;
    for (int i = 0; i < fkeep && l; i++) {
      emit(new AS::MoveInstr("movq `s0, `d0  #27", new TEMP::TempList(regs->head, nullptr), new TEMP::TempList(munchExp(l->head), nullptr)));
      l = l->tail;
      regs = regs->tail;
    }
  }
  if (l) {
    cpush = munchArgs(l->tail, false) + 1;
    emit(new AS::OperInstr("pushq `s0  #28", nullptr, new TEMP::TempList(munchExp(l->head), nullptr), new AS::Targets(nullptr)));
  }
  return cpush;
}
}  // namespace CG