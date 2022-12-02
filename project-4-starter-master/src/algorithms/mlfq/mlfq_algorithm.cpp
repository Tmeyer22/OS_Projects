#include "algorithms/mlfq/mlfq_algorithm.hpp"

#include <cassert>
#include <stdexcept>
#include <cmath>
#include <sstream>

#define FMT_HEADER_ONLY
#include "utilities/fmt/format.h"

/*
    Here is where you should define the logic for the MLFQ algorithm.
*/

MFLQScheduler::MFLQScheduler(int slice) {
    if (slice != -1) {
        throw("MLFQ does NOT take a customizable time slice");
    }
}

std::shared_ptr<SchedulingDecision> MFLQScheduler::get_next_thread() {

    //Instantiate the scheduling decision which will be returned and the counter
    std::shared_ptr<SchedulingDecision> tempDecision;
    tempDecision = std::make_shared<SchedulingDecision>();
    int milfIndex = 0;

    while(milfQueue[milfIndex].empty()){
        milfIndex++;
        if(milfIndex >= 10){
                tempDecision->explanation = "No threads available for scheduling.";
                return tempDecision;
            }
    }
    
    //set decision thread to current thread for next burst
    tempDecision->thread = milfQueue[milfIndex].top();
    milfQueue[milfIndex].pop();
    std::stringstream mainString;

    mainString << "Selected from queue " << milfIndex << " (priority = " << priorityMap[tempDecision->thread->priority] << ", runtime = " << tempDecision->thread->runTime << "). Will run for at most " << std::pow(2, milfIndex) << " ticks.";
    tempDecision->explanation = mainString.str();

    // use pow to inorder to create priority for n * n
    tempDecision->time_slice = (int)std::pow(2, milfIndex);
    time_slice = (int)std::pow(2, milfIndex);

    tempDecision->thread->runTime += tempDecision->thread->get_next_burst(CPU)->length;
    return tempDecision;


}

void MFLQScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {

    //Add next thread to its specific queue then reset runtime for next thread
    if(thread->prevQueue < 9 && thread->runTime >= std::pow(2, thread->prevQueue)){
        thread->prevQueue++;
        thread->runTime = 0;
    }
    milfQueue[thread->prevQueue].push(thread->priority, thread);

}

size_t MFLQScheduler::size() const {
        int total = 0;
        for (int i = 0; i < 10; i++){
            total += milfQueue[i].size();
        }
        
        return total;
}
