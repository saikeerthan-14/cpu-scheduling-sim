#include "../headers/Schedulers.h"
#include <cstdlib>
#include <ctime>
using namespace std;

Scheduler::Scheduler() {
    next_pcb_index = -1;
    ready_queue = NULL;
    blocked_queue = NULL;
}

//constructor for non-RR algs
Scheduler::Scheduler(DList<PCB> *rq, CPU *cp, int alg){
    ready_queue = rq;
    blocked_queue = new DList<PCB>();
    cpu = cp;
    dispatcher = NULL;
    next_pcb_index = -1;
    algorithm = alg;
}

//constructor for RR alg
Scheduler::Scheduler(DList<PCB> *rq, CPU *cp, int alg, int tq){
    ready_queue = rq;
    blocked_queue = new DList<PCB>();
    cpu = cp;
    dispatcher = NULL;
    next_pcb_index = -1;
    algorithm = alg;
    timeq = timer = tq;
}

//dispatcher needed to be set after construction since they mutually include each other
//can only be set once
void Scheduler::setdispatcher(Dispatcher *disp) {
    if(dispatcher == NULL) dispatcher = disp;
}

//dispatcher uses this to determine which process in the queue to grab
int Scheduler::getnext() {
    return next_pcb_index;
}

//switch for the different algorithms
void Scheduler::execute() {
    if(timer > 0) timer -= .5;
    if(ready_queue->size()) {
        switch (algorithm) {
            case 0:
                fcfs();
                break;
            case 1:
                srtf();
                break;
            case 2:
                rr();
                break;
            case 3:
                pp();
                break;
            case 4:
                pr();
                break;
            default:
                break;
        }
    }

    if(blocked_queue != NULL) {
        for(int index = 0; index < blocked_queue->size(); index++) {
            if(blocked_queue->getindex(index)->io_burst <= 0) {
                if(blocked_queue->getindex(index)->process_states.back() != "READY") blocked_queue->getindex(index)->add_state("READY");
                cout << "Process " << cpu->getpcb()->pid << " moved from BLOCKED state to READY state at "<< cpu->gettime();
                cout<<", Time left: " << cpu->getpcb()->time_left << endl;
                ready_queue->add_end(*blocked_queue->getindex(index));
                blocked_queue->removeindex(index);
            } else {
                blocked_queue->getindex(index)->io_burst -= 0.5;
            }
        }
    }
}

//simply waits for cpu to go idle and then tells dispatcher to load next in queue
void Scheduler::fcfs() {
    next_pcb_index = 0;
    if(cpu->isidle()) dispatcher->interrupt();
}

//shortest remaining time first
void Scheduler::srtf() {
    float short_time;
    int short_index = -1;

    //if cpu is idle, initialize shortest time to head of queue
    if(!cpu->isidle()) short_time = cpu->getpcb()->time_left;
    else {
        short_time = ready_queue->gethead()->time_left;
        short_index = 0;
    }

    //now search through queue for actual shortest time
    for(int index = 0; index < ready_queue->size(); ++index){
        if(ready_queue->getindex(index)->time_left < short_time){ //less than ensures FCFS is used for tie
            short_index = index;
            short_time = ready_queue->getindex(index)->time_left;
        }
    }

    //-1 means nothing to schedule, only happens if cpu is already working on shortest
    if(short_index >= 0) {
        next_pcb_index = short_index;
        dispatcher->interrupt();
    }
}

//round robin, simply uses timer and interrupts dispatcher when timer is up, schedules next in queue
void Scheduler::rr() {
    if(cpu->isidle() || timer <= 0){
        timer = timeq;
        next_pcb_index = 0;
        dispatcher->interrupt();
    }
}

//preemptive priority
void Scheduler::pp() {
    int low_prio;
    int low_index = -1;
    // cout<<"At time: "<< cpu->gettime();
    //if cpu is idle, set next pcb in queue as lowest priority initially
    if(!cpu->isidle()) { 
        if(cpu->getpcb()->process_states.back() != "RUNNING") cpu->getpcb()->add_state("RUNNING");
        low_prio = cpu->getpcb()->priority; 
        cout<<"entered if\n";
        if((timeq != -1) && (cpu->getpcb()->io_burst > 0) && (cpu->getpcb()->blocked == false)) {
            cout<<"entered blocked block\n";
            if(timer <= 0.5*timeq) {
                cout<<"entered blocked block & if\n";

                timer = timeq;
                if(cpu->getpcb()->process_states.back() != "BLOCKED") cpu->getpcb()->add_state("BLOCKED");
                cout << "Process " << cpu->getpcb()->pid << " moved from RUNNING state to BLOCKED state at "<< cpu->gettime();
                cout<<", Time left: "<<cpu->getpcb()->time_left<<endl;
                cpu->getpcb()->blocked = true;
                blocked_queue->add_end(*cpu->getpcb());
                dispatcher->interrupt();
            }
        }
    }
    else{
        cout<<"entered else\n";
        low_prio = ready_queue->gethead()->priority;
        low_index = 0;
        if(timeq != -1) timer = timeq;
    }

    cout<<"timer: "<< timer <<", lp: "<<low_prio<<", li: "<<low_index<<", Processes in ready queue: ";
    //search through entire queue for actual lowest priority
    for(int index = 0; index < ready_queue->size(); ++index){
        int temp_prio = ready_queue->getindex(index)->priority;
        cout<<ready_queue->getindex(index)->pid<<"("<<temp_prio<<") ";
        if(temp_prio < low_prio){ //less than ensures FCFS is used for ties
            low_prio = temp_prio;
            low_index = index;
        } else if(temp_prio == low_prio && timer == 0) {
            low_index = index;
            break;
        } 
    }

    
    if(timeq != -1 && timer <= 0) {
            timer = timeq;
            if(low_index!=-1)
                dispatcher->interrupt();
    }
    cout<<", low prio: "<<low_prio<<" ";
    cout<<endl;
    if(low_index!=-1)
        cout<<"next pid "<<ready_queue->getindex(low_index)->pid<<endl;

    if(blocked_queue != NULL) {
        cout<<"Processes in blocked queue: ";
        for(int index = 0; index < blocked_queue->size(); index++) {
            
            cout<<blocked_queue->getindex(index)->pid<<"("<<blocked_queue->getindex(index)->priority<<") ";
            
        }
        cout<<endl;
    }

    //only -1 if couldn't find a pcb to schedule, happens if cpu is already working on lowest priority
    if(low_index >= 0){
        if(timeq != -1) timer = timeq;
        next_pcb_index = low_index;
        dispatcher->interrupt();
    }
}


int Scheduler::get_random_number(int min_range, int max_range) {

    std::uniform_int_distribution<int> distribute(min_range, max_range);

    std::random_device rd;  // Seed the random number

    std::mt19937 gen(rd()); // Mersenne Twister


    // Generate a random number within the specified range
    int number_random = distribute(gen);
    return number_random;
}

void Scheduler::pr() {
    if(cpu->isidle() || (timeq != -1 && timer <= 0)){
        if (timeq != -1) timer = timeq;
        if (ready_queue->size() > 0){
            next_pcb_index = get_random_number(0, ready_queue->size()-1);
        }
        dispatcher->interrupt();
    }
}

/*
 *
 * Dispatcher Implementation
 *
 */
Dispatcher::Dispatcher(){
    cpu = NULL;
    scheduler = NULL;
    ready_queue = NULL;
    clock = NULL;
    _interrupt = false;
}

Dispatcher::Dispatcher(CPU *cp, Scheduler *sch, DList<PCB> *rq, Clock *cl) {
    cpu = cp;
    scheduler = sch;
    ready_queue = rq;
    clock = cl;
    _interrupt = false;
};

//function to handle switching out pcbs and storing back into ready queue
PCB* Dispatcher::switchcontext(int index) {
    PCB* old_pcb = cpu->pcb;
    PCB* new_pcb = new PCB(ready_queue->removeindex(scheduler->getnext()));
    cpu->pcb = new_pcb;
    return old_pcb;
}

//executed every clock cycle, only if scheduler interrupts it
void Dispatcher::execute() {
    if(_interrupt) {
        PCB* old_pcb = switchcontext(scheduler->getnext());
        if(old_pcb != NULL){ //only consider it a switch if cpu was still working on process
            old_pcb->num_context++;
            cpu->getpcb()->wait_time += .5;
            if(cpu->getpcb()->process_states.back() != "RUNNING") cpu->getpcb()->add_state("RUNNING");

            clock->step();
            if(old_pcb->process_states.back() != "READY" && old_pcb->time_left > 0) old_pcb->add_state("READY");
            ready_queue->add_end(*old_pcb);
            cout << "Process " << old_pcb->pid << " moved from RUNNING state to READY state at "<< clock->gettime();
            cout<<", Time left: "<<old_pcb->time_left<<endl;
            cout << "Process " << cpu->getpcb()->pid << " moved from READY state to RUNNING state at "<< clock->gettime();
            cout<<", Time left: "<<cpu->getpcb()->time_left<<endl;
            delete old_pcb;
        }
        _interrupt = false;
    }
}

//routine for scheudler to interrupt it
void Dispatcher::interrupt() {
    _interrupt = true;
}