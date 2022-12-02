#include "algorithms/rr/rr_algorithm.hpp"

#include <cassert>
#include <stdexcept>

#define FMT_HEADER_ONLY
#include "utilities/fmt/format.h"

/*
    Here is where you should define the logic for the round robin algorithm.
*/

RRScheduler::RRScheduler(int slice) {    

    if (slice == -1) {
        time_slice = 3;
    }
    else{
        time_slice = slice;
    }
}

std::shared_ptr<SchedulingDecision> RRScheduler::get_next_thread() {
    std::shared_ptr<SchedulingDecision> tempDecision;
    tempDecision = std::make_shared<SchedulingDecision>();

    if(rrQueue.empty()){
        tempDecision->explanation = "No threads available for scheduling.";
        return tempDecision;
    }

    std::string numThreadsString;
    std::string burstThreadString;

    numThreadsString = std::to_string(rrQueue.size());
    burstThreadString = std::to_string(time_slice);
    std::string tempStr1 = "Selected from ";
    std::string tempStr2 = " threads. Will run for at most ";
    std::string tempStr3 = " ticks.";

    tempStr1 = tempStr1 + numThreadsString + tempStr2 + burstThreadString + tempStr3;



    tempDecision->explanation = tempStr1;
    tempDecision->thread = rrQueue.front();
    rrQueue.pop();

    return tempDecision;
}

void RRScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {
    rrQueue.push(thread);
}

size_t RRScheduler::size() const {
    return rrQueue.size();
;
}
