#include "tiger/regalloc/color.h"
#include <cstdio>
namespace COL {

G::NodeList<TEMP::Temp> *simplifyWorklist = nullptr;
G::NodeList<TEMP::Temp> *freezeWorklist = nullptr;
G::NodeList<TEMP::Temp> *spillWorklist = nullptr;
G::NodeList<TEMP::Temp> *spilledNodes = nullptr;
G::NodeList<TEMP::Temp> *coalescedNodes = nullptr;
G::NodeList<TEMP::Temp> *coloredNodes = nullptr;
G::NodeList<TEMP::Temp> *selectStack = nullptr;

LIVE::MoveList *coalescedMoves = nullptr;
LIVE::MoveList *constrainedMoves = nullptr;
LIVE::MoveList *frozenMoves = nullptr;
LIVE::MoveList *worklistMoves = nullptr;
LIVE::MoveList *activeMoves = nullptr;

TEMP::TempList *notSpillTemps = nullptr;

std::map<G::Node<TEMP::Temp> *, int> degreeMap;
std::map<G::Node<TEMP::Temp> *, int> colorMap;
std::map<G::Node<TEMP::Temp> *, G::Node<TEMP::Temp> *> aliasMap;

std::string *hard_regs[17];

const int MAX_COLOR = 15;

Result Color(G::Graph<TEMP::Temp> *ig, TEMP::Map *initial, TEMP::TempList *regs,
             LIVE::MoveList* moves) {
  // TODO: Put your codes here (lab6).
  simplifyWorklist = nullptr;
  freezeWorklist = nullptr;
  spillWorklist = nullptr;
  spilledNodes = nullptr;
  coalescedNodes = nullptr;
  coloredNodes = nullptr;
  selectStack = nullptr;

  coalescedMoves = nullptr;
  constrainedMoves = nullptr;
  frozenMoves = nullptr;
  worklistMoves = moves;
  activeMoves = nullptr;

  // notSpillTemps = nullptr;
  
  hard_regs[0] = new std::string("none");
  hard_regs[1] = new std::string("%rax");
  hard_regs[2] = new std::string("%rbx");
  hard_regs[3] = new std::string("%rcx");
  hard_regs[4] = new std::string("%rdx");
  hard_regs[5] = new std::string("%rdi");
  hard_regs[6] = new std::string("%rsi");
  hard_regs[7] = new std::string("%rbp");
  hard_regs[8] = new std::string("%r8");
  hard_regs[9] = new std::string("%r9");
  hard_regs[10] = new std::string("%r10");
  hard_regs[11] = new std::string("%r11");
  hard_regs[12] = new std::string("%r12");
  hard_regs[13] = new std::string("%r13");
  hard_regs[14] = new std::string("%r14");
  hard_regs[15] = new std::string("%r15");
  hard_regs[16] = new std::string("%rsp");

  Build(ig);
  MakeWorkList(ig);
  while (simplifyWorklist || worklistMoves || freezeWorklist || spillWorklist) {
    if (simplifyWorklist) {
      Simplify();
    }
    else if (worklistMoves) {
      Coalesce();
    }
    else if (freezeWorklist) {
      Freeze();
    }
    else if (spillWorklist) {
      SelectSpill();
    }
  }

  AssignColors(ig);
  COL::Result ret;
  TEMP::Map *coloring = TEMP::Map::Empty();
  
  for (G::NodeList<TEMP::Temp> *nodes = ig->Nodes(); nodes; nodes = nodes->tail) {
    int color = colorMap[GetAlias(nodes->head)];
    coloring->Enter(nodes->head->NodeInfo(), hard_regs[color]);
  }

  ret.coloring = coloring;

  TEMP::TempList *actual_spills = nullptr;

  for (; spilledNodes; spilledNodes = spilledNodes->tail) {
    TEMP::Temp *temp = spilledNodes->head->NodeInfo();
    actual_spills = new TEMP::TempList(temp, actual_spills);
  }

  ret.spills = actual_spills;
  return ret;
}

void Build(G::Graph<TEMP::Temp> *ig) {
  for (G::NodeList<TEMP::Temp> *nodes = ig->Nodes(); nodes; nodes = nodes->tail) {
    // init degree
    int degree = 0;
    for (G::NodeList<TEMP::Temp> *cur = nodes->head->Succ(); cur; cur = cur->tail) {
      degree++;
    }
    degreeMap[nodes->head] = degree;

    // init color
    TEMP::Temp *temp = nodes->head->NodeInfo();
    int color = locate_register(temp);
    colorMap[nodes->head] = color;

    // init alias
    aliasMap[nodes->head] = nodes->head;
  }
}

int locate_register(TEMP::Temp *temp) {
  if (temp == F::RAX())
    return 1;
  if (temp == F::RBX())
    return 2;
  if (temp == F::RCX())
    return 3;
  if (temp == F::RDX())
    return 4;
  if (temp == F::RDI())
    return 5;
  if (temp == F::RSI())
    return 6;
  if (temp == F::RBP())
    return 7;
  if (temp == F::R8())
    return 8;
  if (temp == F::R9())
    return 9;
  if (temp == F::R10())
    return 10;
  if (temp == F::R11())
    return 11;
  if (temp == F::R12())
    return 12;
  if (temp == F::R13())
    return 13;
  if (temp == F::R14())
    return 14;
  if (temp == F::R15())
    return 15;
  if (temp == F::RSP())
    return 16;
  if (temp == F::FP())
    assert(0);
  return 0;
}

void MakeWorkList(G::Graph<TEMP::Temp> *ig) {
  for (G::NodeList<TEMP::Temp> *nodes = ig->Nodes(); nodes; nodes = nodes->tail) {
    int degree = degreeMap[nodes->head];
    int color = colorMap[nodes->head];

    if (color != 0) // if precolored
      continue;
    
    if (degree >= MAX_COLOR) {
      spillWorklist = new G::NodeList<TEMP::Temp>(nodes->head, spillWorklist);
    }
    else if (MoveRelated(nodes->head)) {
      freezeWorklist = new G::NodeList<TEMP::Temp>(nodes->head, freezeWorklist);
    }
    else {
      simplifyWorklist = new G::NodeList<TEMP::Temp>(nodes->head, simplifyWorklist);
    }
  }
}

void Simplify() {
  G::Node<TEMP::Temp> *node = simplifyWorklist->head;
  simplifyWorklist = simplifyWorklist->tail;
  selectStack = new G::NodeList<TEMP::Temp>(node, selectStack);
  for (G::NodeList<TEMP::Temp> *nodes = Adjacent(node); nodes; nodes = nodes->tail) {
    DecrementDegree(nodes->head);
  }
}

void DecrementDegree(G::Node<TEMP::Temp> *m) {
  int color = colorMap[m];
  if (color != 0)
    return;

  int degree = degreeMap[m];
  degree--;

  if (degree == MAX_COLOR - 1) {
    // just become insignificant node
    EnableMoves(new G::NodeList<TEMP::Temp>(m, Adjacent(m)));
    spillWorklist = subNodeList(spillWorklist, new G::NodeList<TEMP::Temp>(m, nullptr));
    if (MoveRelated(m)) {
      freezeWorklist = new G::NodeList<TEMP::Temp>(m, freezeWorklist);
    }
    else {
      simplifyWorklist = new G::NodeList<TEMP::Temp>(m, simplifyWorklist);
    }
  }
}

void EnableMoves(G::NodeList<TEMP::Temp> *nodes) {
  for (; nodes; nodes = nodes->tail) {
    for (LIVE::MoveList *m = NodeMoves(nodes->head); m; m = m->tail) {
      if (COL::inMoveList(activeMoves, m->src, m->dst)) {
        activeMoves = subMoveList(activeMoves, new LIVE::MoveList(m->src, m->dst, nullptr));
        worklistMoves = new LIVE::MoveList(m->src, m->dst, worklistMoves);
      }
    }
  }
}

void Coalesce() {
  G::Node<TEMP::Temp> *u;
  G::Node<TEMP::Temp> *v;
  G::Node<TEMP::Temp> *x = worklistMoves->src;
  G::Node<TEMP::Temp> *y = worklistMoves->dst;
  
  // make sure that if v is precolored, u must be precolored too.
  if (precolored(GetAlias(y))) {
    u = GetAlias(y);
    v = GetAlias(x);
  }
  else {
    u = GetAlias(x);
    v = GetAlias(y);
  }

  worklistMoves = worklistMoves->tail;
  if (u == v) {
    coalescedMoves = new LIVE::MoveList(x, y, coalescedMoves);
    AddWorkList(u);
  }
  else if (precolored(v) || v->Adj()->InNodeList(u)) {
    // both precolored or constrained
    constrainedMoves = new LIVE::MoveList(x, y, constrainedMoves);
    AddWorkList(u);
    AddWorkList(v);
  }
  else if (precolored(u) && OK(v, u)) {
    coalescedMoves = new LIVE::MoveList(x, y, coalescedMoves);
    Combine(u, v);
    AddWorkList(u);
  }
  else if (!precolored(u) && Briggs(u, v)) {
    coalescedMoves = new LIVE::MoveList(x, y, coalescedMoves);
    Combine(u, v);
    AddWorkList(u);
  }
  else {
    activeMoves = new LIVE::MoveList(x, y, activeMoves);
  }
}

G::NodeList<TEMP::Temp> *Adjacent(G::Node<TEMP::Temp> *n) {
  return subNodeList(subNodeList(n->Succ(), selectStack), coalescedNodes);
}

void AddWorkList(G::Node<TEMP::Temp> *u) {
  if (!precolored(u) && !MoveRelated(u) && degreeMap[u] < MAX_COLOR) {
    freezeWorklist = subNodeList(freezeWorklist, new G::NodeList<TEMP::Temp>(u, nullptr));
    simplifyWorklist = new G::NodeList<TEMP::Temp>(u, simplifyWorklist);
  }
}

// George heuristic strategy
bool OK(G::Node<TEMP::Temp> *t, G::Node<TEMP::Temp> *r) {
  for (G::NodeList<TEMP::Temp> *p = Adjacent(t); p; p = p->tail) {
    if (degreeMap[p->head] < MAX_COLOR || precolored(p->head) || r->Adj()->InNodeList(p->head))
      continue;
    else
      return false;
  }
  return true;
}

bool Briggs(G::Node<TEMP::Temp> *u, G::Node<TEMP::Temp> *v) {
  G::NodeList<TEMP::Temp> *nodes = unionNodeList(Adjacent(u), Adjacent(v));
  int k = 0;
  for (; nodes; nodes = nodes->tail) {
    if (degreeMap[nodes->head] >= MAX_COLOR) {
      k++;
    }
  }
  return (k < MAX_COLOR);
}

void Combine(G::Node<TEMP::Temp> *u, G::Node<TEMP::Temp> *v) {
  if (freezeWorklist->InNodeList(v)) {
    freezeWorklist = subNodeList(freezeWorklist, new G::NodeList<TEMP::Temp>(v, nullptr));
  }
  else {
    spillWorklist = subNodeList(spillWorklist, new G::NodeList<TEMP::Temp>(v, nullptr));
  }
  coalescedNodes = new G::NodeList<TEMP::Temp>(v, coalescedNodes);

  aliasMap[v] = u;

  for (G::NodeList<TEMP::Temp> *t = Adjacent(v); t; t = t->tail) {
    AddEdge(t->head, u);
    DecrementDegree(t->head);
  }

  if (degreeMap[u] >= MAX_COLOR && freezeWorklist->InNodeList(u)) {
    freezeWorklist = subNodeList(freezeWorklist, new G::NodeList<TEMP::Temp>(u, nullptr));
    spillWorklist = new G::NodeList<TEMP::Temp>(u, spillWorklist);
  }
}

void Freeze() {
  G::Node<TEMP::Temp> *u = freezeWorklist->head;
  freezeWorklist = freezeWorklist->tail;
  simplifyWorklist = new G::NodeList<TEMP::Temp>(u, simplifyWorklist);
  FreezeMoves(u);
}

void FreezeMoves(G::Node<TEMP::Temp> *u) {
  for (LIVE::MoveList *m = NodeMoves(u); m; m = m->tail) {
    G::Node<TEMP::Temp> *x = m->src;
    G::Node<TEMP::Temp> *y = m->dst;
    G::Node<TEMP::Temp> *v;

    if (GetAlias(y) == GetAlias(u)) {
      v = GetAlias(x);
    }
    else {
      v = GetAlias(y);
    }

    activeMoves = subMoveList(activeMoves, new LIVE::MoveList(x, y, nullptr));
    frozenMoves = new LIVE::MoveList(x, y, frozenMoves);
    if (!precolored(v) && NodeMoves(v) == nullptr && degreeMap[v] < MAX_COLOR) {
      freezeWorklist = subNodeList(freezeWorklist, new G::NodeList<TEMP::Temp>(v, nullptr));
      simplifyWorklist = new G::NodeList<TEMP::Temp>(v, simplifyWorklist);
    }
  }
}

void SelectSpill() {
  G::Node<TEMP::Temp> *m = nullptr;
  int max = 0;
  for (G::NodeList<TEMP::Temp> *p = spillWorklist; p; p = p->tail) {
    TEMP::Temp *t = p->head->NodeInfo();
    // if (inTempList()) // todo
    int degree = degreeMap[p->head];
    if (degree > max) {
      max = degree;
      m = p->head;
    }
  }

  if (m) {
    spillWorklist = subNodeList(spillWorklist, new G::NodeList<TEMP::Temp>(m, nullptr));
    simplifyWorklist = new G::NodeList<TEMP::Temp>(m, simplifyWorklist);
    FreezeMoves(m);
  }
  else {
    assert(0);
  }
}

void AssignColors(G::Graph<TEMP::Temp> *ig) {
  bool okColors[MAX_COLOR + 2];
  spilledNodes = nullptr;
  while (selectStack) {
    okColors[0] = false;
    for (int i = 1; i <= MAX_COLOR; i++) {
      okColors[i] = true;
    }
    G::Node<TEMP::Temp> *n = selectStack->head;
    selectStack = selectStack->tail;

    for (G::NodeList<TEMP::Temp> *adjs = n->Succ(); adjs; adjs = adjs->tail) {
      int color = colorMap[GetAlias(adjs->head)];
      okColors[color] = false;
    }

    int i;
    bool realSpill = true;
    for (i = 1; i <= MAX_COLOR; i++) {
      if (okColors[i]) {
        realSpill = false;
        break;
      }
    }

    if (realSpill) {
      spilledNodes = new G::NodeList<TEMP::Temp>(n, spilledNodes);
    }
    else {
      colorMap[n] = i;
    }
  }

  // color coalesced nodes
  for (G::NodeList<TEMP::Temp> *p = ig->Nodes(); p; p = p->tail) {
    colorMap[p->head] = colorMap[GetAlias(p->head)];
  }
}

bool MoveRelated(G::Node<TEMP::Temp> *n) {
  return (NodeMoves(n) != nullptr);
}

// get all moves connected with node n
LIVE::MoveList *NodeMoves(G::Node<TEMP::Temp> *n) {
  LIVE::MoveList *moves = nullptr;
  G::Node<TEMP::Temp> *m = GetAlias(n);
  for (LIVE::MoveList *p = unionMoveList(activeMoves, worklistMoves); p; p = p->tail) {
    if (GetAlias(p->src) == m || GetAlias(p->dst) == m) {
      moves = new LIVE::MoveList(p->src, p->dst, moves);
    }
  }
  return moves;
}

G::Node<TEMP::Temp> *GetAlias(G::Node<TEMP::Temp> *n) {
  G::Node<TEMP::Temp> *res = aliasMap[n];
  if (res != n) {
    return GetAlias(res);
  }
  return res;
}

bool inMoveList(LIVE::MoveList *a, G::Node<TEMP::Temp> *src, G::Node<TEMP::Temp> *dst) {
  for (; a; a = a->tail) {
    if (a->src == src && a->dst == dst) {
      return true;
    }
  }
  return false;
}

LIVE::MoveList *subMoveList(LIVE::MoveList *a, LIVE::MoveList *b) {
  LIVE::MoveList *res = nullptr;
  for (LIVE::MoveList *p = a; p; p = p->tail) {
    if (!COL::inMoveList(b, p->src, p->dst)) {
      res = new LIVE::MoveList(p->src, p->dst, res);
    }
  }
  return res;
}

LIVE::MoveList *unionMoveList(LIVE::MoveList *a, LIVE::MoveList *b) {
  LIVE::MoveList *res = a;
  for (LIVE::MoveList *p = b; p; p = p->tail) {
    if (!COL::inMoveList(a, p->src, p->dst)) {
      res = new LIVE::MoveList(p->src, p->dst, res);
    }
  }
  return res;
}

bool precolored(G::Node<TEMP::Temp> *n) {
  return (colorMap[n] != 0);
}

void AddEdge(G::Node<TEMP::Temp> *u, G::Node<TEMP::Temp> *v) {
  if (!v->Adj()->InNodeList(u) && u != v) {
    if (!precolored(u)) {
      degreeMap[u]++;
      u->GetGraph()->AddEdge(u, v);
    }
    if (!precolored(v)) {
      degreeMap[v]++;
      v->GetGraph()->AddEdge(v, u);
    }
  }
}

G::NodeList<TEMP::Temp> *subNodeList(G::NodeList<TEMP::Temp> *u, G::NodeList<TEMP::Temp> *v) {
  G::NodeList<TEMP::Temp> *res = nullptr;
  for (G::NodeList<TEMP::Temp> *nodes = u; nodes; nodes = nodes->tail) {
    if (!v->InNodeList(nodes->head)) {
      res = new G::NodeList<TEMP::Temp>(nodes->head, res);
    }
  }
  return res;
}

G::NodeList<TEMP::Temp> *unionNodeList(G::NodeList<TEMP::Temp> *u, G::NodeList<TEMP::Temp> *v) {
  G::NodeList<TEMP::Temp> *res = u;
  for (G::NodeList<TEMP::Temp> *nodes = v; nodes; nodes = nodes->tail) {
    if (!u->InNodeList(nodes->head)) {
      res = new G::NodeList<TEMP::Temp>(nodes->head, res);
    }
  }
  return res;
}

bool inTempList(TEMP::TempList *a, TEMP::Temp *t) {
  for (; a; a = a->tail) {
    if (a->head == t) {
      return true;
    }
  }
  return false;
}

void enter_hard_regs(TEMP::Map *coloring) {
  coloring->Enter(F::RAX(), hard_regs[1]);
  coloring->Enter(F::RBX(), hard_regs[2]);
  coloring->Enter(F::RCX(), hard_regs[3]);
  coloring->Enter(F::RDX(), hard_regs[4]);
  coloring->Enter(F::RSI(), hard_regs[5]);
  coloring->Enter(F::RDI(), hard_regs[6]);
  coloring->Enter(F::RBP(), hard_regs[7]);
  coloring->Enter(F::R8(), hard_regs[8]);
  coloring->Enter(F::R9(), hard_regs[9]);
  coloring->Enter(F::R10(), hard_regs[10]);
  coloring->Enter(F::R11(), hard_regs[11]);
  coloring->Enter(F::R12(), hard_regs[12]);
  coloring->Enter(F::R13(), hard_regs[13]);
  coloring->Enter(F::R14(), hard_regs[14]);
  coloring->Enter(F::R15(), hard_regs[15]);
  coloring->Enter(F::RSP(), hard_regs[16]);
}

}  // namespace COL