/**
 * This file contains implementations for methods in the PhysicalAddress class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "physical_address/physical_address.h"

#include <sstream>
using namespace std;

string PhysicalAddress::to_string() const {
    // TODO: implement me

    //physical address 0000000001100000 [frame: 1; offset: 32]

    stringstream physString;
    bitset<FRAME_BITS> frameAddress(frame);
    bitset<OFFSET_BITS> offsetAddress(offset);

    physString << frameAddress << offsetAddress;

    return physString.str();
}


ostream& operator <<(ostream& out, const PhysicalAddress& address) {
    // TODO: implement me

    bitset<address.FRAME_BITS> frameAddress(address.frame);
    bitset<address.OFFSET_BITS> offsetAddress(address.offset);

    //out << frameAddress << offsetAddress;
    out << frameAddress << offsetAddress << " [frame: " << address.frame << "; offset: " << address.offset << "]";
    
    return out;
}
