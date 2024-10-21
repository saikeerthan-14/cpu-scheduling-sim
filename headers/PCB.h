//
// Created by Joseph on 10/7/2017.
//

#ifndef LAB2_PCB_H
#define LAB2_PCB_H

#include <iostream>
using namespace std;

//data structure to hold process information
struct PCB{
    int pid, arrival, burst, priority, num_context, io_burst;
    float time_left, resp_time, wait_time, finish_time;
    bool started, blocked;
    PCB *process_state;

    PCB(){pid = arrival = burst = time_left = priority = io_burst = resp_time = wait_time = num_context = finish_time = started = blocked = 0;}
    PCB(int id, int arr, int time, int prio, int i_o) : pid(id), arrival(arr), burst(time), priority(prio), io_burst(i_o),time_left(time){
        resp_time = wait_time = num_context = finish_time = started = blocked = 0;
    }
    void print(){
        std::cout << pid << " " << arrival << " " << time_left << " " << priority << " " << io_burst << std::endl;
    }

};
#endif //LAB2_PCB_H
