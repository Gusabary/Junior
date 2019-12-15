#ifndef TIGER_LIVENESS_LIVENESS_H_
#define TIGER_LIVENESS_LIVENESS_H_

#include "tiger/codegen/assem.h"
#include "tiger/frame/frame.h"
#include "tiger/frame/temp.h"
#include "tiger/liveness/flowgraph.h"
#include "tiger/util/graph.h"

namespace LIVE {

class MoveList {
 public:
  G::Node<TEMP::Temp>*src, *dst;
  MoveList* tail;

  MoveList(G::Node<TEMP::Temp>* src, G::Node<TEMP::Temp>* dst, MoveList* tail)
      : src(src), dst(dst), tail(tail) {}
};

class LiveGraph {
 public:
  G::Graph<TEMP::Temp>* graph;
  MoveList* moves;
};

LiveGraph Liveness(G::Graph<AS::Instr>* flowgraph);

G::Node<TEMP::Temp> *tempToNode(TAB::Table<TEMP::Temp, G::Node<TEMP::Temp>> *tb, TEMP::Temp *t, G::Graph<TEMP::Temp> *g);

void calc_live(G::NodeList<AS::Instr> *nodes);

bool dfs_live(G::NodeList<AS::Instr> *nodes);

// a union b
TEMP::TempList *unionTempList(TEMP::TempList *a, TEMP::TempList *b);

// a sub b
TEMP::TempList *subTempList(TEMP::TempList *a, TEMP::TempList *b);

bool isEqual(TEMP::TempList *a, TEMP::TempList *b);

bool inTempList(TEMP::TempList *a, TEMP::Temp *t);

bool inMoveList(LIVE::MoveList *a, G::Node<TEMP::Temp> *src, G::Node<TEMP::Temp> *dst);

}  // namespace LIVE

#endif