#include "tiger/liveness/liveness.h"

#include <map>
#include <cstdio>

namespace LIVE {

// std::map<G::Node<AS::Instr> *, TEMP::TempList *> inMap;
// std::map<G::Node<AS::Instr> *, TEMP::TempList *> outMap;
static TAB::Table<G::Node<AS::Instr>, TEMP::TempList> *inMap;
static TAB::Table<G::Node<AS::Instr>, TEMP::TempList> *outMap;

LiveGraph Liveness(G::Graph<AS::Instr>* flowgraph) {
  // TODO: Put your codes here (lab6).
  LiveGraph lg;
    printf("----======calc_live before=======-----\n");
    inMap = new TAB::Table<G::Node<AS::Instr>, TEMP::TempList>();
    outMap = new TAB::Table<G::Node<AS::Instr>, TEMP::TempList>();

    // calc_live(flowgraph->Nodes());
    while(dfs_live(flowgraph->Nodes()));
    printf("----======calc_live after=======-----\n");

  TAB::Table<TEMP::Temp, G::Node<TEMP::Temp> > *temp_to_node = new TAB::Table<TEMP::Temp, G::Node<TEMP::Temp> >();
  lg.graph = new G::Graph<TEMP::Temp>();
  lg.moves = nullptr;

  TEMP::TempList *hardRegs = new TEMP::TempList(F::RAX(),
                             new TEMP::TempList(F::RBX(),
                             new TEMP::TempList(F::RCX(),
                             new TEMP::TempList(F::RDX(),
                             new TEMP::TempList(F::RDI(),
                             new TEMP::TempList(F::RSI(),
                             new TEMP::TempList(F::RBP(),
                             new TEMP::TempList(F::R8(),
                             new TEMP::TempList(F::R9(),
                             new TEMP::TempList(F::R10(),
                             new TEMP::TempList(F::R11(),
                             new TEMP::TempList(F::R12(),
                             new TEMP::TempList(F::R13(),
                             new TEMP::TempList(F::R14(),
                             new TEMP::TempList(F::R15(),
                             new TEMP::TempList(F::RSP(), nullptr))))))))))))))));
    printf("----======hard regs=======-----\n");

  for (TEMP::TempList *temps = hardRegs; temps; temps = temps->tail) {
    temp_to_node->Enter(temps->head, lg.graph->NewNode(temps->head));
  }

  for (TEMP::TempList *temps = hardRegs; temps; temps = temps->tail) {
    for (TEMP::TempList *next = temps->tail; next; next = next->tail) {
      lg.graph->AddEdge(temp_to_node->Look(temps->head), temp_to_node->Look(next->head));
    }
  }

  // add conflict edge
  for (G::NodeList<AS::Instr> *nodes = flowgraph->Nodes(); nodes; nodes = nodes->tail) {
    for (TEMP::TempList *def = FG::Def(nodes->head); def; def = def->tail) {
      G::Node<TEMP::Temp> *a = tempToNode(temp_to_node, def->head, lg.graph);
      for (TEMP::TempList *out = outMap->Look(nodes->head); out; out = out->tail) {
        if (out->head == def->head)
          continue;
        G::Node<TEMP::Temp> *b = tempToNode(temp_to_node, out->head, lg.graph);
        if (!b->Adj()->InNodeList(a) && (!FG::IsMove(nodes->head) || !inTempList(FG::Use(nodes->head), out->head))) {
          // but what if move instr is like a <- b + c
          lg.graph->AddEdge(a, b);
          lg.graph->AddEdge(b, a);
        }
      }

      // add movelist
      if (!FG::IsMove(nodes->head))
        continue;
      for (TEMP::TempList *out = FG::Use(nodes->head); out; out = out->tail) {
        if (out->head == F::FP() || out->head == def->head)
          continue;
        G::Node<TEMP::Temp> *b = tempToNode(temp_to_node, out->head, lg.graph);

        if (!inMoveList(lg.moves, b, a)) {
          lg.moves = new MoveList(b, a, lg.moves);
        }
      }
    }
  }
  
  return lg;
}

G::Node<TEMP::Temp> *tempToNode(TAB::Table<TEMP::Temp, G::Node<TEMP::Temp> > *tb, TEMP::Temp *t, G::Graph<TEMP::Temp> *g) {
  G::Node<TEMP::Temp> *node = tb->Look(t);
  if (!node) {
    node = g->NewNode(t);
    tb->Enter(t, node);
  }
  return node;
}

void calc_live(G::NodeList<AS::Instr> *nodes) {
  bool hasChange = true;
  while (hasChange) {
    hasChange = false;
    for (G::NodeList<AS::Instr> *ni = nodes; ni; ni = ni->tail) {
      TEMP::TempList *in = inMap->Look(ni->head);
      TEMP::TempList *out = outMap->Look(ni->head);
      for (G::NodeList<AS::Instr> *succs = ni->head->Succ(); succs; succs = succs->tail) {
        outMap->Enter(ni->head, unionTempList(outMap->Look(ni->head), inMap->Look(succs->head)));
      }
      inMap->Enter(ni->head, unionTempList(FG::Use(ni->head), subTempList(outMap->Look(ni->head), FG::Def(ni->head))));
      if (!isEqual(inMap->Look(ni->head), in) || !isEqual(outMap->Look(ni->head), out)) {
        hasChange = true;
      }
    }
  }
}

bool dfs_live(G::NodeList<AS::Instr> *nodes) {
  bool res = false;
  if (nodes->tail && nodes->tail->head)
    res = dfs_live(nodes->tail);
  else if (!nodes->head)
    return false;

  G::Node<AS::Instr> *n = nodes->head;
  TEMP::TempList *in_old = inMap->Look(n);
  TEMP::TempList *out_old = outMap->Look(n);

  TEMP::TempList *in = nullptr;
  TEMP::TempList *out = nullptr;

  G::NodeList<AS::Instr> *succs = n->Succ();
  for (; succs; succs = succs->tail) {
    TEMP::TempList *in_succ = inMap->Look(succs->head);
    out = unionTempList(out, in_succ);
  }
  in = unionTempList(FG::Use(n), subTempList(out, FG::Def(n)));

  bool cur_res = !isEqual(in_old, in) || !isEqual(out_old, out);
  inMap->Enter(n, in);
  outMap->Enter(n, out);

  return res || cur_res;
}

TEMP::TempList *unionTempList(TEMP::TempList *a, TEMP::TempList *b) {
  TEMP::TempList *res = a;
  for (; b; b = b->tail) {
    if (!inTempList(a, b->head)) {
      assert(b->head);
      res = new TEMP::TempList(b->head, res);
    }
  }
  return res;
}

TEMP::TempList *subTempList(TEMP::TempList *a, TEMP::TempList *b) {
  TEMP::TempList *res = nullptr;
  for (; a; a = a->tail) {
    if (!inTempList(b, a->head)) {
      assert(a->head);
      res = new TEMP::TempList(a->head, res);
    }
  }
  return res;
}

bool isEqual(TEMP::TempList *a, TEMP::TempList *b) {
  for (TEMP::TempList *p = a; p; p = p->tail) {
    if (!inTempList(b, p->head)) {
      return false;
    }
  }
  for (TEMP::TempList *p = b; p; p = p->tail) {
    if (!inTempList(a, p->head)) {
      return false;
    }
  }
  return true;
}

bool inTempList(TEMP::TempList *a, TEMP::Temp *t) {
  for (; a; a = a->tail) {
    if (a->head == t) {
      return true;
    }
  }
  return false;
}

bool inMoveList(LIVE::MoveList *a, G::Node<TEMP::Temp> *src, G::Node<TEMP::Temp> *dst) {
  for (; a; a = a->tail) {
    if (a->src == src && a->dst == dst) {
      return true;
    }
  }
  return false;
}

}  // namespace LIVE