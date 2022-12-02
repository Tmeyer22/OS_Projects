#include "algorithms/priority/priority_algorithm.hpp"

#include <cassert>
#include <stdexcept>

#define FMT_HEADER_ONLY
#include "utilities/fmt/format.h"


/*
    Here is where you should define the logic for the priority algorithm.
*/

PRIORITYScheduler::PRIORITYScheduler(int slice) {
    if (slice != -1) {
        throw("PRIORITY must have a timeslice of -1");
    }
}

std::shared_ptr<SchedulingDecision> PRIORITYScheduler::get_next_thread() {

    std::shared_ptr<SchedulingDecision> tempDecision;
    tempDecision = std::make_shared<SchedulingDecision>();

    if(sysQueue.empty() && interactiveQueue.empty() && normQueue.empty() && batchQueue.empty()){
        tempDecision->explanation = "No threads available for scheduling.";
        return tempDecision;
    }

    std::string mainString;

    std::string sysSize = std::to_string(sysQueue.size());
    std::string interactiveSize = std::to_string(interactiveQueue.size());
    std::string normSize = std::to_string(normQueue.size());
    std::string batchSize = std::to_string(batchQueue.size());

    mainString = "[S: " + sysSize + " I: " + interactiveSize
     + " N: " + normSize + " B: " + batchSize + "] -> ";

    if (!sysQueue.empty())
    {
        tempDecision->thread = sysQueue.front();
        sysQueue.pop();
    }
    else if (!interactiveQueue.empty())
    {
        tempDecision->thread = interactiveQueue.front();
        interactiveQueue.pop();
    }
    else if (!normQueue.empty())
    {
        tempDecision->thread = normQueue.front();
        normQueue.pop();
    }
    else if (!batchQueue.empty())
    {
        tempDecision->thread = batchQueue.front();
        batchQueue.pop();
    }

    sysSize = std::to_string(sysQueue.size());
    interactiveSize = std::to_string(interactiveQueue.size());
    normSize = std::to_string(normQueue.size());
    batchSize = std::to_string(batchQueue.size());

    mainString = mainString + "[S: " + sysSize + " I: " + interactiveSize
     + " N: " + normSize + " B: " + batchSize + "]. Will run to completion of burst.";

    tempDecision->explanation = mainString;
     return tempDecision;

}

void PRIORITYScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {
    // TODO: implement me!
    if (thread->priority == SYSTEM)
    {
        sysQueue.push(thread);
    }
    else if (thread->priority == INTERACTIVE)
    {
        interactiveQueue.push(thread);
    }
    else if (thread->priority == NORMAL)
    {
        normQueue.push(thread);
    }
    else if (thread->priority == BATCH)
    {
        batchQueue.push(thread);
    }
    
}

size_t PRIORITYScheduler::size() const {
    return sysQueue.size() + interactiveQueue.size() + normQueue.size() + batchQueue.size();

}
