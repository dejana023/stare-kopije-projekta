#ifndef MEM_C
#define MEM_C
#include "memory.hpp"

Mem::Mem(sc_module_name name):sc_module(name)
{
    s_mem_t0.register_b_transport(this, &Mem::b_transport);
    s_mem_t1.register_b_transport(this, &Mem::b_transport);
	
    cout<<"Memory constructed"<<endl;
    ddr.reserve(64*308);
} 

void Mem::b_transport(pl_t& pl, sc_time& offset)
{
    tlm_command cmd    = pl.get_command();
    sc_dt::uint64 adr  = pl.get_address();
    unsigned char *buf = pl.get_data_ptr();
    unsigned int len   = pl.get_data_length();
        
    switch(cmd)
    {
        case TLM_WRITE_COMMAND:
            for(unsigned int i=0; i<len; i++)
            {       
                ddr[adr+i]=((num_f*)buf)[i];
            }
            pl.set_response_status(TLM_OK_RESPONSE);
            break;

        case TLM_READ_COMMAND:
            buf = (unsigned char*)&ddr[adr];
            pl.set_data_ptr(buf);
            pl.set_response_status(TLM_OK_RESPONSE);
            break;
        
        default:
            pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
    }
    
    offset += sc_time(10, SC_NS);
}

#endif // MEM_C