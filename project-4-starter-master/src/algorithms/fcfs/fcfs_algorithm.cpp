#include "algorithms/fcfs/fcfs_algorithm.hpp"

#include <cassert>
#include <stdexcept>

#include <queue>
#define FMT_HEADER_ONLY
#include "utilities/fmt/format.h"

/*
    Here is where you should define the logic for the FCFS algorithm.
*/

FCFSScheduler::FCFSScheduler(int slice) {
    if (slice != -1) {
        throw("FCFS must have a timeslice of -1");
    }
}

std::shared_ptr<SchedulingDecision> FCFSScheduler::get_next_thread() {

    std::shared_ptr<SchedulingDecision> tempDecision;
    tempDecision = std::make_shared<SchedulingDecision>();

    if(fcfsQueue.empty()){
        tempDecision->explanation = "No threads available for scheduling.";
        return tempDecision;
    }

    std::string numThreadsString;
    numThreadsString = std::to_string(fcfsQueue.size());
    std::string tempStr = "Selected from  threads. Will run to completion of burst.";

    tempStr.insert(14, numThreadsString);

    tempDecision->explanation = tempStr;
    tempDecision->thread = fcfsQueue.front();
    fcfsQueue.pop();

    return tempDecision;
}

void FCFSScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {
        fcfsQueue.push(thread);
}

size_t FCFSScheduler::size() const {
        return fcfsQueue.size();
}
