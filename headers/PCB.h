//
// Created by Joseph on 10/7/2017.
//

#ifndef LAB2_PCB_H
#define LAB2_PCB_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

//data structure to hold process information
struct PCB{
    int pid, arrival, burst, priority, num_context, io_burst;
    float time_left, resp_time, wait_time, finish_time;
    bool started, blocked;
    vector<string> process_states;

    PCB(){
        pid = arrival = burst = time_left = priority = io_burst = resp_time = wait_time = num_context = finish_time = started = blocked = 0;
        process_states = {};
    }
    
    PCB(int id, int arr, int time, int prio, int i_o) : pid(id), arrival(arr), burst(time), priority(prio), io_burst(i_o),time_left(time){
        resp_time = wait_time = num_context = finish_time = started = blocked = 0;
        process_states = {};
    }
    void print(){
        std::cout << pid << " " << arrival << " " << time_left << " " << priority << " " << io_burst << std::endl;
    }

    void add_state(string state) {
        process_states.push_back(state);
    }


};
#endif //LAB2_PCB_H
