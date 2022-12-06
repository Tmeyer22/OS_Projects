/**
 * This file contains implementations for the methods defined in the Simulation
 * class.
 *
 * You'll probably spend a lot of your time here.
 */

#include "simulation/simulation.h"
#include <stdexcept>
#include <sstream>

Simulation::Simulation(FlagOptions &flags)
{
    this->flags = flags;
    this->frames.reserve(this->NUM_FRAMES);
}

void Simulation::run()
{
    // TODO: implement me
    for (int i = 0; i < 512; i++)
    {
        free_frames.push_back(i);
    }

    for (size_t i = 0; i < virtual_addresses.size(); i++)
    {
        perform_memory_access(virtual_addresses.at(i));
    }
}

char Simulation::perform_memory_access(const VirtualAddress &virtual_address)
{
    // TODO: implement me

    Process *temp = processes[virtual_address.process_id];
    size_t frameNum;
    if (temp->is_valid_page(virtual_address.page))
    {
        bool didPageFault = false;
        // for (size_t i = 0; i < temp->page_table.rows.size(); i++)
        // {
        //     frameNum = temp->page_table.rows.at(i).frame;
        //     if (temp->page_table.rows.at(i).present && frames.at(frameNum).page_number == frameNum)
        //     {
        //         std::cout << "good";
        //         didPageFault = false;
        //         break;
        //     }
        // }

        if (temp->page_table.rows.at(virtual_address.page).present == 0)
        {
            didPageFault = true;
        }

        std::cout << virtual_address << "\n";
        if (didPageFault)
        {
            handle_page_fault(temp, virtual_address.page);
            std::cout << "  -> PAGE FAULT\n";
        }
        else
        {
            std::cout << "  -> IN MEMORY\n";
        }

        frameNum = temp->page_table.rows.at(virtual_address.page).frame;
        PhysicalAddress physAddress(frameNum, virtual_address.offset);

        std::cout << "  -> physical address " << physAddress << "\n";

        std::cout << "  -> RSS: " << temp->get_rss() << "\n\n";

        temp->page_table.rows.at(frameNum).last_accessed_at = stepTime;
        stepTime;
        return frames.at(frameNum).contents->get_byte_at_offset(virtual_address.offset);
    }

    return 0;
}

void Simulation::handle_page_fault(Process *process, size_t page)
{
    // TODO: implement me
    page_faults++;

    if (process->get_rss() < flags.max_frames)
    {
        process->page_table.rows.at(page).present = 1;

        int freeFrameNum = free_frames.front();
        free_frames.pop_front();

        process->page_table.rows.at(page).loaded_at = stepTime;
        stepTime++;

        process->page_table.rows.at(page).frame = freeFrameNum;

        // Should this have New?
        Frame newFrame;
        newFrame.set_page(process, page);
        frames.push_back(newFrame);
    }
    else if (flags.strategy == ReplacementStrategy::FIFO)
    {

        int freeFrameNum = process->page_table.rows.at(process->page_table.get_oldest_page()).frame;
        process->page_table.rows.at(process->page_table.get_oldest_page()).present = 0;
        process->page_table.rows.at(page).present = 1;
        process->page_table.rows.at(page).loaded_at = stepTime;
        stepTime++;

        frames.at(freeFrameNum).set_page(process, page);
    }
    else if (flags.strategy == ReplacementStrategy::LRU)
    {
        int freeFrameNum = process->page_table.rows.at(process->page_table.get_least_recently_used_page()).frame;
        process->page_table.rows.at(process->page_table.get_least_recently_used_page()).present = 0;

        process->page_table.rows.at(page).present = 1;

        process->page_table.rows.at(page).loaded_at = stepTime;
        stepTime++;

        frames.at(freeFrameNum).set_page(process, page);
    }
}

void Simulation::print_summary()
{
    if (!this->flags.csv)
    {
        boost::format process_fmt(
            "Process %3d:  "
            "ACCESSES: %-6lu "
            "FAULTS: %-6lu "
            "FAULT RATE: %-8.2f "
            "RSS: %-6lu\n");

        for (auto entry : this->processes)
        {
            std::cout << process_fmt % entry.first % entry.second->memory_accesses % entry.second->page_faults % entry.second->get_fault_percent() % entry.second->get_rss();
        }

        // Print statistics.
        boost::format summary_fmt(
            "\n%-25s %12lu\n"
            "%-25s %12lu\n"
            "%-25s %12lu\n");

        std::cout << summary_fmt % "Total memory accesses:" % this->virtual_addresses.size() % "Total page faults:" % this->page_faults % "Free frames remaining:" % this->free_frames.size();
    }

    if (this->flags.csv)
    {
        boost::format process_fmt(
            "%d,"
            "%lu,"
            "%lu,"
            "%.2f,"
            "%lu\n");

        for (auto entry : processes)
        {
            std::cout << process_fmt % entry.first % entry.second->memory_accesses % entry.second->page_faults % entry.second->get_fault_percent() % entry.second->get_rss();
        }

        // Print statistics.
        boost::format summary_fmt(
            "%lu,,,,\n"
            "%lu,,,,\n"
            "%lu,,,,\n");

        std::cout << summary_fmt % this->virtual_addresses.size() % this->page_faults % this->free_frames.size();
    }
}

int Simulation::read_processes(std::istream &simulation_file)
{
    int num_processes;
    simulation_file >> num_processes;

    for (int i = 0; i < num_processes; ++i)
    {
        int pid;
        std::string process_image_path;

        simulation_file >> pid >> process_image_path;

        std::ifstream proc_img_file(process_image_path);

        if (!proc_img_file)
        {
            std::cerr << "Unable to read file for PID " << pid << ": " << process_image_path << std::endl;
            return 1;
        }
        this->processes[pid] = Process::read_from_input(proc_img_file);
    }
    return 0;
}

int Simulation::read_addresses(std::istream &simulation_file)
{
    int pid;
    std::string virtual_address;

    try
    {
        while (simulation_file >> pid >> virtual_address)
        {
            this->virtual_addresses.push_back(VirtualAddress::from_string(pid, virtual_address));
        }
    }
    catch (const std::exception &except)
    {
        std::cerr << "Error reading virtual addresses." << std::endl;
        std::cerr << except.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Error reading virtual addresses." << std::endl;
        return 1;
    }
    return 0;
}

int Simulation::read_simulation_file()
{
    std::ifstream simulation_file(this->flags.filename);
    // this->simulation_file.open(this->flags.filename);

    if (!simulation_file)
    {
        std::cerr << "Unable to open file: " << this->flags.filename << std::endl;
        return -1;
    }
    int error = 0;
    error = this->read_processes(simulation_file);

    if (error)
    {
        std::cerr << "Error reading processes. Exit: " << error << std::endl;
        return error;
    }

    error = this->read_addresses(simulation_file);

    if (error)
    {
        std::cerr << "Error reading addresses." << std::endl;
        return error;
    }

    if (this->flags.file_verbose)
    {
        for (auto entry : this->processes)
        {
            std::cout << "Process " << entry.first << ": Size: " << entry.second->size() << std::endl;
        }

        for (auto entry : this->virtual_addresses)
        {
            std::cout << entry << std::endl;
        }
    }

    return 0;
}
