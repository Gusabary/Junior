#include "tiger/liveness/flowgraph.h"

#include <string>

namespace FG {

TEMP::TempList* Def(G::Node<AS::Instr>* n) {
  // TODO: Put your codes here (lab6).
  AS::Instr *instr = n->NodeInfo();
  if (instr->kind == AS::Instr::OPER) {
    return ((AS::OperInstr *)instr)->dst;
  }
  if (instr->kind == AS::Instr::MOVE) {
    return ((AS::MoveInstr *)instr)->dst;
  }
  return nullptr;
}

TEMP::TempList* Use(G::Node<AS::Instr>* n) {
  // TODO: Put your codes here (lab6).
  AS::Instr *instr = n->NodeInfo();
  if (instr->kind == AS::Instr::OPER) {
    return ((AS::OperInstr *)instr)->src;
  }
  if (instr->kind == AS::Instr::MOVE) {
    return ((AS::MoveInstr *)instr)->src;
  }
  return nullptr;
}

bool IsMove(G::Node<AS::Instr>* n) {
  // TODO: Put your codes here (lab6).
  AS::Instr *instr = n->NodeInfo();
  return instr->kind == AS::Instr::MOVE;
}

G::Graph<AS::Instr>* AssemFlowGraph(AS::InstrList* il, F::Frame* f) {
  // TODO: Put your codes here (lab6).
  G::Graph<AS::Instr> *g = new G::Graph<AS::Instr>();
  G::Node<AS::Instr> *prev = nullptr;

  TAB::Table<TEMP::Label, G::Node<AS::Instr> > *label_table = new TAB::Table<TEMP::Label, G::Node<AS::Instr> >();

  // begin with sequential control flow
  for (AS::InstrList *cur = il; cur; cur = cur->tail) {
    G::Node<AS::Instr> *node = g->NewNode(cur->head);
    if (prev)
      g->AddEdge(prev, node);
    if (cur->head->kind == AS::Instr::OPER && ((AS::OperInstr *)(cur->head))->assem.compare(0, 3, "jmp") == 0) {
      prev = nullptr;
    }
    else {
      prev = node;
    }
    if (cur->head->kind == AS::Instr::LABEL) {
      label_table->Enter(((AS::LabelInstr *)cur->head)->label, node);
    }
  }

  // add jmp->label link
  for (G::NodeList<AS::Instr> *ni = g->Nodes(); ni; ni = ni->tail) {
    AS::Instr *instr = ni->head->NodeInfo();
    if (instr->kind == AS::Instr::OPER && ((AS::OperInstr *)instr)->jumps != nullptr) {
      for (TEMP::LabelList *targets = ((AS::OperInstr *)instr)->jumps->labels; targets; targets = targets->tail) {
        G::Node<AS::Instr> *target = label_table->Look(targets->head);
        if (target) {
          g->AddEdge(ni->head, target);
        }
      }
    }
  }

  return g;
}

}  // namespace FG
