#include "tiger/regalloc/regalloc.h"
#include <cstdio>
#include <string>

namespace RA {

static std::string fp_fix[1000];
static int fp_fix_num;
static int fp_fix_off[1000];

bool hasTemp(TEMP::TempList *list, TEMP::Temp *temp) {
  for (; list; list = list->tail) {
    if (list->head == temp)
      return true;
  }
  return false;
}

void replaceTemp(TEMP::TempList *list, TEMP::Temp *old, TEMP::Temp *neww) {
  for (; list; list = list->tail) {
    if (list->head == old) {
      list->head = neww;
    }
  }
}

AS::InstrList *rewriteProgram(F::Frame *f, AS::InstrList *il, TEMP::TempList *spills) {
    printf("----======Rewrite=======-----\n");
  AS::InstrList *result = il;
  for (; spills; spills = spills->tail) {
    f->localCount++;

    for (AS::InstrList *instrs = result; instrs; instrs = instrs->tail) {
      if (instrs->head->kind == AS::Instr::OPER || instrs->head->kind == AS::Instr::MOVE) {
        // what about move?
        if (hasTemp(((AS::OperInstr *)instrs->head)->dst, spills->head)) {
          TEMP::Temp *temp = TEMP::Temp::NewTemp();
          replaceTemp(((AS::OperInstr *)instrs->head)->dst, spills->head, temp);
          fp_fix[fp_fix_num] = "movq `s0, ";
          fp_fix[fp_fix_num] += TEMP::LabelString(f->label);
          fp_fix[fp_fix_num] += "framesize + ";
          fp_fix[fp_fix_num] += std::to_string(-(f->localCount) * F::wordSize);
          fp_fix[fp_fix_num] += "(%rsp)";
          fp_fix_off[fp_fix_num] = f->localCount;

          // AS::InstrList *storeList = new AS::InstrList(nullptr, nullptr);
          AS::Instr *store = new AS::OperInstr(fp_fix[fp_fix_num], nullptr, new TEMP::TempList(temp, nullptr), nullptr);
          // TEMP::Temp *d = TEMP::Temp::NewTemp();
          // storeList->head = new AS::OperInstr("movq `s0, `d0  #C1", new TEMP::TempList(d, nullptr), new TEMP::TempList(F::RSP(), nullptr), nullptr);

          // std::string instr;
          // instr += "addq $";
          // instr += TEMP::LabelString(f->label);
          // instr += "framesize, `d0  #C2";
          // AS::Instr *store = new AS::OperInstr(instr, new TEMP::TempList(d, nullptr), nullptr, new AS::Targets(nullptr));
          // storeList->tail = new AS::InstrList(store, nullptr);

          // instr.clear();
          // instr += "movq `s0, ";
          // instr += std::to_string(-f->localCount * F::wordSize);
          // instr += "(`d0)  #C3";
          // store = new AS::OperInstr(instr, new TEMP::TempList(d, nullptr), new TEMP::TempList(temp, nullptr), new AS::Targets(nullptr));
          // storeList->tail->tail = new AS::InstrList(store, instrs->tail);

          // instrs->tail = storeList;
          instrs->tail = new AS::InstrList(store, instrs->tail);
          fp_fix_num++;
        }
        else if (hasTemp(((AS::OperInstr *)instrs->head)->src, spills->head)) {
          TEMP::Temp *temp = TEMP::Temp::NewTemp();
          replaceTemp(((AS::OperInstr *)instrs->head)->src, spills->head, temp);
          fp_fix[fp_fix_num] = "movq ";
          fp_fix[fp_fix_num] += TEMP::LabelString(f->label);
          fp_fix[fp_fix_num] += "framesize + ";
          fp_fix[fp_fix_num] += std::to_string(-(f->localCount) * F::wordSize);
          fp_fix[fp_fix_num] += "(%rsp), `d0";
          fp_fix_off[fp_fix_num] = -f->localCount;

          // AS::InstrList *fetchList = new AS::InstrList(nullptr, nullptr);
          AS::Instr *fetch = new AS::OperInstr(fp_fix[fp_fix_num], new TEMP::TempList(temp, nullptr), nullptr, nullptr);
          
          // std::string instr = "movq "

          // TEMP::Temp *d = TEMP::Temp::NewTemp();
          // fetchList->head = new AS::OperInstr("movq `s0, `d0  #C1", new TEMP::TempList(d, nullptr), new TEMP::TempList(F::RSP(), nullptr), nullptr);

          // std::string instr;
          // instr += "addq $";
          // instr += TEMP::LabelString(f->label);
          // instr += "framesize, `d0  #C2";
          // AS::Instr *fetch = new AS::OperInstr(instr, new TEMP::TempList(d, nullptr), nullptr, new AS::Targets(nullptr));
          // fetchList->tail = new AS::InstrList(fetch, nullptr);

          // instr.clear();
          // instr += "movq ";
          // instr += std::to_string(-f->localCount * F::wordSize);
          // instr += "(`s0), `d0  #C4";
          // fetch = new AS::OperInstr(instr, new TEMP::TempList(temp, nullptr), new TEMP::TempList(d, nullptr), new AS::Targets(nullptr));
          // fetchList->tail->tail = new AS::InstrList(fetch, new AS::InstrList(instrs->head, fetchList));

          // todo: fix fp here

          // AS::Instr *fetch = new AS::OperInstr(fp_fix[fp_fix_num], new TEMP::TempList(temp, new TEMP::TempList(F::FP(), nullptr)), nullptr, nullptr);
          
          instrs->tail = new AS::InstrList(instrs->head, instrs->tail);
          instrs->head = fetch;
          fp_fix_num++;
        }
      }
    }
  }
  return result;
}

Result RegAlloc(F::Frame* f, AS::InstrList* il) {
  // TODO: Put your codes here (lab6).
  COL::Result color;
  fp_fix_num = 0;
  while (true) {
    G::Graph<AS::Instr> *flow_graph = FG::AssemFlowGraph(il, f);
    LIVE::LiveGraph live_graph = LIVE::Liveness(flow_graph);
    color = COL::Color(live_graph.graph, nullptr, nullptr, live_graph.moves);  // todo

    if (color.spills == nullptr)
      break;

    il = rewriteProgram(f, il, color.spills);
  }

  int frameSize = f->localCount * F::wordSize;
  for (int i = 0; i < fp_fix_num; i++) {
    if (fp_fix_off[i] > 0) {
      // sprintf(fp_fix[i], "movq `s0, %d(%%rsp)", (-(fp_fix_off[i]) * F::wordSize) + frameSize);
      fp_fix[i] = "movq `s0, ";
      fp_fix[i] += std::to_string((-(fp_fix_off[i]) * F::wordSize) + frameSize);
      fp_fix[i] += "(%rsp) #T1";
    }
    else {
      // sprintf(fp_fix[i], "movq %d(%%rsp), `d0", ((fp_fix_off[i]) * F::wordSize) + frameSize);
      fp_fix[i] = "movq ";
      fp_fix[i] += std::to_string(((fp_fix_off[i]) * F::wordSize) + frameSize);
      fp_fix[i] += "(%rsp), `d0 #T2";
    }
  }
  RA::Result ret(il);
  ret.coloring = color.coloring;
  return ret;
}

}  // namespace RA