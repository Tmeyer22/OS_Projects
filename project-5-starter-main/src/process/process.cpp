/**
 * This file contains implementations for methods in the Process class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "process/process.h"

using namespace std;


Process* Process::read_from_input(std::istream& in) {
    std::vector<Page*> pages;

    int numBytesForFile = in.gcount();
    while (!in.eof()) {
        pages.push_back(Page::read_from_input(in));
    }
    
    Page* tempPage = Page::read_from_input(in);
    return new Process(numBytesForFile, pages);
    
}


size_t Process::size() const
{
    int byteSize = 0;
    for (int i = 0; i < pages.size(); i++)
    {
       byteSize += pages.at(i)->size();
    }
    
    return byteSize;
}


bool Process::is_valid_page(size_t index) const
{
    if (pages.size() > index && pages.size() > 0){
        return pages.at(index);
    }
    
    return false;
}


size_t Process::get_rss() const
{
    int rssSize = 0;
    for (int i = 0; i < page_table.rows.size(); i++){
        if(page_table.rows.at(i).present){
            rssSize++;
        }
    }

    return rssSize;
}


double Process::get_fault_percent() const
{
    if (memory_accesses == 0){
        return 0.0;
    }

    return (double)page_faults/memory_accesses * 100;
}
