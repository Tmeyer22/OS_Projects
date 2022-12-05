/**
 * This file contains implementations for methods in the PageTable class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "page_table/page_table.h"

using namespace std;


size_t PageTable::get_present_page_count() const {

    int numPages = 0;
    for(int i = 0 ; i < rows.size(); i++){
        if(rows.at(i).present){
            numPages++;
        }
    }
    return numPages;
}


size_t PageTable::get_oldest_page() const {
    
    int oldestTime = 100000;
    int oldPage = 0;

    for(int i = 0 ; i < rows.size(); i++){
        if( rows.at(i).present && rows.at(i).loaded_at < oldestTime){
            oldestTime = rows.at(i).loaded_at;
            oldPage = i;
        }
    }

    return oldPage;
}


size_t PageTable::get_least_recently_used_page() const {

    int oldestTime = 100000;
    int oldPage = 0;

    for(int i = 0 ; i < rows.size(); i++){
        if( rows.at(i).present && rows.at(i).last_accessed_at < oldestTime){
            oldestTime = rows.at(i).last_accessed_at;
            oldPage = i;
        }
    }

    return oldPage;
}
