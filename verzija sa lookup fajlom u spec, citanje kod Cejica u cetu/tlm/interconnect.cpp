#ifndef INT_CON_C
#define INT_CON_C
#include "interconnect.hpp"

InterCon::InterCon(sc_module_name name):sc_module(name)
{
    s_ic_t.register_b_transport(this, &InterCon::b_transport);
    cout<< "Interconnect created" << endl;
}

void InterCon::b_transport(pl_t& pl, sc_time& offset)
{
    sc_dt::uint64 address = pl.get_address();
    string msg;
    offset += sc_time(2, SC_NS);
        
    if (address >= 0 && address < 22000)
    s_ic_i0 -> b_transport(pl, offset);  //memorija
    
    else if (address >= 22000)
    s_ic_i1 -> b_transport(pl, offset); //IP    
}

#endif //INT_CON_C
