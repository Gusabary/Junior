#ifndef TIGER_REGALLOC_COLOR_H_
#define TIGER_REGALLOC_COLOR_H_

#include "tiger/frame/temp.h"
#include "tiger/liveness/liveness.h"
#include "tiger/util/graph.h"

#include <map>
#include <string>

namespace COL {

class Result {
 public:
  TEMP::Map* coloring;
  TEMP::TempList* spills;
};

Result Color(G::Graph<TEMP::Temp> *ig, TEMP::Map *initial, TEMP::TempList *regs,
             LIVE::MoveList *moves);

void Build(G::Graph<TEMP::Temp> *ig);

int locate_register(TEMP::Temp *temp);

void MakeWorkList(G::Graph<TEMP::Temp> *ig);

void Simplify();

void DecrementDegree(G::Node<TEMP::Temp> *m);

void EnableMoves(G::NodeList<TEMP::Temp> *nodes);

void Coalesce();

G::NodeList<TEMP::Temp> *Adjacent(G::Node<TEMP::Temp> *n);

void AddWorkList(G::Node<TEMP::Temp> *u);

bool OK(G::Node<TEMP::Temp> *t, G::Node<TEMP::Temp> *r);

bool Briggs(G::Node<TEMP::Temp> *u, G::Node<TEMP::Temp> *v);

void Combine(G::Node<TEMP::Temp> *u, G::Node<TEMP::Temp> *v);

void Freeze();

void FreezeMoves(G::Node<TEMP::Temp> *u);

void SelectSpill();

void AssignColors(G::Graph<TEMP::Temp> *ig);

bool MoveRelated(G::Node<TEMP::Temp> *n);

LIVE::MoveList *NodeMoves(G::Node<TEMP::Temp> *n);

G::Node<TEMP::Temp> *GetAlias(G::Node<TEMP::Temp> *n);

bool inMoveList(LIVE::MoveList *a, G::Node<TEMP::Temp> *src, G::Node<TEMP::Temp> *dst);

LIVE::MoveList *subMoveList(LIVE::MoveList *a, LIVE::MoveList *b);

LIVE::MoveList *unionMoveList(LIVE::MoveList *a, LIVE::MoveList *b);

bool precolored(G::Node<TEMP::Temp> *n);

void AddEdge(G::Node<TEMP::Temp> *u, G::Node<TEMP::Temp> *v);

G::NodeList<TEMP::Temp> *subNodeList(G::NodeList<TEMP::Temp> *u, G::NodeList<TEMP::Temp> *v);

G::NodeList<TEMP::Temp> *unionNodeList(G::NodeList<TEMP::Temp> *u, G::NodeList<TEMP::Temp> *v);

bool inTempList(TEMP::TempList *a, TEMP::Temp *t);

void enter_hard_regs(TEMP::Map *coloring);

}  // namespace COL

#endif