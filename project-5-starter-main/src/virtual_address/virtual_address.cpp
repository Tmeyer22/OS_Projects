/**
 * This file contains implementations for methods in the VirtualAddress class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "virtual_address/virtual_address.h"
#include <sstream>

using namespace std;

VirtualAddress VirtualAddress::from_string(int process_id, string address) {

    const size_t frame = bitset<32>(address.substr(0,10)).to_ulong();
    const size_t offset = bitset<32>(address.substr(10,6)).to_ulong();
    
    return VirtualAddress(process_id, frame, offset);
}


string VirtualAddress::to_string() const {

    stringstream mainString;

    bitset<10> pageAddress(page);
    bitset<6> offsetAddress(offset);
    mainString << pageAddress << offsetAddress;

    return mainString.str();
}


ostream& operator <<(ostream& out, const VirtualAddress& address) {

    bitset<10> pageAddress(address.page);
    bitset<6> offsetAddress(address.offset);

    //out << frameAddress << offsetAddress;
    out << "PID " << address.process_id << " @ "<< pageAddress << offsetAddress << " [page: " << address.page << "; offset: " << address.offset << "]";
    
    return out;
}
