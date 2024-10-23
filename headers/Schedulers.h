#ifndef LAB2_SCHEDULER_H
#define LAB2_SCHEDULER_H

#include "DList.h"
#include "CPU.h"
#include <random>

class CPU;
class Scheduler;
//needs to be forward declared since Dispatcher and Scheduler mutually include each other
class Dispatcher{
private:
    CPU *cpu;
    Scheduler *scheduler;
    DList<PCB> *ready_queue;
    Clock *clock;
    bool _interrupt;
public:
    Dispatcher();
    Dispatcher(CPU *cp, Scheduler *sch, DList<PCB> *rq, Clock *cl);
    PCB* switchcontext(int index);
    void execute();
    void interrupt();
};

class Scheduler{
private:
    int next_pcb_index;
    DList<PCB> *ready_queue, *blocked_queue;
    CPU *cpu;
    Dispatcher *dispatcher;
    int algorithm;
    float timeq, timer; //time quantum, timer to keep track of when to interrupt dispatcher
public:
    Scheduler();
    Scheduler(DList<PCB> *rq, CPU *cp, int alg);
    Scheduler(DList<PCB> *rq, CPU *cp, int alg, int tq);
    void setdispatcher(Dispatcher *disp);
    int getnext();
    int get_random_number(int min_range, int max_range);
    void execute();
    void fcfs();
    void srtf();
    void rr();
    void pp();
    void pr();
};

#endif //LAB2_SCHEDULER_H
