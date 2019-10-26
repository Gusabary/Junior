#include "straightline/slp.h"

namespace A {
int A::CompoundStm::MaxArgs() const {
  int stm1Args = stm1->MaxArgs();
  int stm2Args = stm2->MaxArgs();
  int maxArgs = stm1Args > stm2Args ? stm1Args : stm2Args;
  return maxArgs;
}

Table *A::CompoundStm::Interp(Table *t) const {
  Table *stm1Table = stm1->Interp(t);
  Table *stm2Table = stm2->Interp(stm1Table);
  return stm2Table;
}

int A::AssignStm::MaxArgs() const {
  int maxArgs = exp->MaxArgs();
  return maxArgs;
}

// t as input won't get changed
Table *A::AssignStm::Interp(Table *t) const {
  IntAndTable *intAndTable = exp->Interp(t);
  int value = intAndTable->i;
  Table *newTable = intAndTable->t;
  return newTable->Update(id, value);
}

int A::PrintStm::MaxArgs() const {
  int maxArgs = exps->MaxArgs();
  return maxArgs;
}

Table *A::PrintStm::Interp(Table *t) const {
  Table *newTable = exps->Interp(t)->t;
  return newTable;
}

int IdExp::MaxArgs() const {
  return 1;
}

IntAndTable *IdExp::Interp(Table *t) const {
  int value = t->Lookup(id);
  return new IntAndTable(value, t);
}

int NumExp::MaxArgs() const {
  return 1;
}

IntAndTable *NumExp::Interp(Table *t) const {
  return new IntAndTable(num, t);
}

int OpExp::MaxArgs() const {
  int leftArgs = left->MaxArgs();
  int rightArgs = right->MaxArgs();
  int maxArgs = leftArgs > rightArgs ? leftArgs : rightArgs;
  return maxArgs;
}

IntAndTable *OpExp::Interp(Table *t) const {
  IntAndTable *leftIntAndTable = left->Interp(t);
  int leftValue = leftIntAndTable->i;
  Table *tmpTable = leftIntAndTable->t;
  IntAndTable *rightIntAndTable = right->Interp(t);
  int rightValue = rightIntAndTable->i;
  Table *newTable = rightIntAndTable->t;
  int value = 0;
  switch (oper) {
    case PLUS:
      value = leftValue + rightValue; break;
    case MINUS:
      value = leftValue - rightValue; break;
    case TIMES:
      value = leftValue * rightValue; break;
    case DIV:
      value = leftValue / rightValue;
  }    
  return new IntAndTable(value, newTable);
}

int EseqExp::MaxArgs() const {
  int stmArgs = stm->MaxArgs();
  int expArgs = exp->MaxArgs();
  int maxArgs = stmArgs > expArgs ? stmArgs : expArgs;
  return maxArgs;
}

IntAndTable *EseqExp::Interp(Table *t) const {
  Table *newTable = stm->Interp(t);
  return exp->Interp(newTable);
}

int PairExpList::MaxArgs() const {
  int headArgs = head->MaxArgs();
  int tailArgs = tail->MaxArgs();
  int numExps = NumExps();
  if (headArgs > tailArgs && headArgs > numExps)
    return headArgs;
  if (tailArgs > numExps)
    return tailArgs;
  return numExps;
}

int PairExpList::NumExps() const {
  int numExps = 1 + tail->NumExps();
  return numExps;
}

IntAndTable *PairExpList::Interp(Table *t) const {
  IntAndTable *intAndTable = head->Interp(t);
  int value = intAndTable->i;
  Table *newTable = intAndTable->t;
  std::cout << value << ' ';
  return tail->Interp(newTable);
}

int LastExpList::MaxArgs() const {
  int maxArgs = last->MaxArgs();
  return maxArgs;
}

int LastExpList::NumExps() const {
  return 1;
}

IntAndTable *LastExpList::Interp(Table *t) const {
  IntAndTable *intAndTable = last->Interp(t);
  int value = intAndTable->i;
  Table *newTable = intAndTable->t;
  std::cout << value << std::endl;
  return new IntAndTable(value, newTable);
}

int Table::Lookup(std::string key) const {
  if (id == key) {
    return value;
  } else if (tail != nullptr) {
    return tail->Lookup(key);
  } else {
    assert(false);
  }
}

Table *Table::Update(std::string key, int value) const {
  return new Table(key, value, this);
}
}  // namespace A

