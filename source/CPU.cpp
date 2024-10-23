#include "../headers/CPU.h"

CPU::CPU(DList<PCB> *fq, Clock *cl) {
    pcb = NULL;
    idle = true;
    finished_queue = fq;
    clock = cl;
}

//used by others to determine what the cpu is working on like priority and, Time left
PCB* CPU::getpcb() {
    return pcb;
}

//check to see if cpu is currently working on a process
bool CPU::isidle() {
    return idle;
}

//called every clock cycle
void CPU::execute() {
    // cout<<"current process: "<<pcb->pid<<", time left: "<<pcb->time_left<<endl;
    if(pcb != NULL){
        idle = false;
        if(!pcb->started){ //helps determine response time, only increments it if pcb hasn't been worked on yet
            pcb->started = true;
            if(pcb->process_states.back() != "RUNNING") pcb->add_state("RUNNING");
            // cout << "Process " << pcb->pid << " moved from READY state to RUNNING state for the first time at "<< clock->gettime();
            // cout<<", Time left: "<<pcb->time_left<<endl;
            pcb->resp_time = clock->gettime() - pcb->arrival;
        }
        pcb->time_left -= .5; //simulate process being worked on for a clock cycle
        if(pcb->time_left <= 0) { //terminate it if its done and set self to idle
            terminate();
            idle = true;
        }

    }
}

//routine to update termination related stats, for StatUpdater to use later
void CPU::terminate() {
    // cout << "Process " << pcb->pid << " moved from RUNNING state to TERMINATED state at "<< clock->gettime()+.5;
    // cout<<", Time left: "<<pcb->time_left<<endl;

    if(pcb->process_states.back() != "TERMINATED") pcb->add_state("TERMINATED");
    pcb->finish_time = clock->gettime()+.5;
    finished_queue->add_end(*pcb);
    delete pcb;
    pcb = NULL;
}