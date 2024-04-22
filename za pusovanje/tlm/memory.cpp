#ifndef MEM_C
#define MEM_C
#include "memory.hpp"

Mem::Mem(sc_module_name name):sc_module(name)
{
    mem_socket_1.register_b_transport(this, &Mem::b_transport);
    mem_socket_2.register_b_transport(this, &Mem::b_transport);
	
    cout<<"Memory constructed"<<endl;
    mem.reserve(240000);
} 

void Mem::b_transport(pl_t& pl, sc_time& offset)
{
    tlm_command cmd    = pl.get_command();
    sc_dt::uint64 addr  = pl.get_address();
    unsigned char *buf = pl.get_data_ptr();
    unsigned int len   = pl.get_data_length();
        
    switch(cmd)
    {
        case TLM_WRITE_COMMAND:
            for(unsigned int i=0; i<len; i++)
            {       
                mem[addr+i]=buf[i];
            }
            pl.set_response_status(TLM_OK_RESPONSE);
            
            offset += sc_core::sc_time(DELAY, sc_core::SC_NS);
            break;

        case TLM_READ_COMMAND:
            for (unsigned int i = 0; i < len; ++i)
            {
                buf[i] = mem[addr++];
            }
            pl.set_response_status(TLM_OK_RESPONSE);
            
            offset += sc_core::sc_time(DELAY, sc_core::SC_NS);
            break;
        
        default:
            pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
            offset += sc_core::sc_time(DELAY, sc_core::SC_NS);
    }
}

#endif // MEM_C
