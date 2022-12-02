#include "algorithms/spn/spn_algorithm.hpp"

#include <cassert>
#include <stdexcept>

#define FMT_HEADER_ONLY
#include "utilities/fmt/format.h"

/*
    Here is where you should define the logic for the SPN algorithm.
*/

SPNScheduler::SPNScheduler(int slice) {
    if (slice != -1) {
        throw("SPN must have a timeslice of -1");
    }
}

std::shared_ptr<SchedulingDecision> SPNScheduler::get_next_thread() {
    // TODO: implement me!

    std::shared_ptr<SchedulingDecision> tempDecision;
    tempDecision = std::make_shared<SchedulingDecision>();

    if(spnQueue.empty()){
        tempDecision->explanation = "No threads available for scheduling.";
        return tempDecision;
    }

    std::string numThreadsString;
    numThreadsString = std::to_string(spnQueue.size());
    std::string tempStr = "Selected from  threads. Will run to completion of burst.";

    tempStr.insert(14, numThreadsString);

    tempDecision->explanation = tempStr;
    tempDecision->thread = spnQueue.top();
    spnQueue.pop();

    return tempDecision;

}

void SPNScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {
    //TODO: Implement me!
    spnQueue.push(thread->get_next_burst(CPU)->length, thread);
}

size_t SPNScheduler::size() const {

    return spnQueue.size();
}
