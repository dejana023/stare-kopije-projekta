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
    //num_f *buf = reinterpret_cast<num_f*>(pl.get_data_ptr());
    unsigned int len   = pl.get_data_length();
    
    int64_t value;
        
    switch(cmd)
    {
        case TLM_WRITE_COMMAND:
            if(addr+len != mem.size())
                mem.resize(mem.size() - (mem.size()-addr)+len,0);
                
            for(unsigned int i=0; i<len; i++)
            {       
                unsigned int m = addr + i;
                mem[m]=((num_f*)buf)[i];
                //mem[m] = buf[i];
                
                //cout << "Upisano u mem:" ;
                //cout << "Adresa: " << static_cast<int>(addr-1) << ", Upisan broj: " << static_cast<int>(mem[addr-1]) << endl;
                cout<< "mem[" << m << "] = " << mem[m] << endl;


            }
            pl.set_response_status(TLM_OK_RESPONSE);
            
            offset += sc_core::sc_time(DELAY, sc_core::SC_NS);
            break;

/*case TLM_READ_COMMAND:
{
    for (unsigned int i = 0; i < len; i++) {
        uint32_t temp_value = mem[addr++];
        
        cout<< temp_value << endl;
        value = num_f((double)temp_value/1073741824.0);
        
        unsigned char* value_bytes = reinterpret_cast<unsigned char*>(&value);
        for (int j = 0; j < 4; j++)
        {
            buf[i+j] = value_bytes[j];
        }
     
    }
    pl.set_response_status(tlm:: TLM_OK_RESPONSE);
    
    offset += sc_core::sc_time(DELAY, sc_core::SC_NS);
    break;
}*/

case tlm::TLM_READ_COMMAND:
            /*for (unsigned int i = 0; i < len; i+=6)
            {
                unsigned int m = addr + i;
                buf = (unsigned char*)&mem[addr++];
                //cout << "buf[" << m << "] = " << *(num_f*)buf << endl;
            }
            pl.set_response_status(tlm::TLM_OK_RESPONSE);
            
            offset += sc_core::sc_time(DELAY, sc_core::SC_NS);
            break;*/
            for (unsigned int i = 0; i < len; i+=6)
            {
                value = (int64_t)(mem[addr++] << 18);
                buf[i] = (unsigned char)(value >> 8);
		buf[i+1] = (unsigned char)(value >> 16);
                buf[i+2] = (unsigned char)(value >> 24);
                buf[i+3] = (unsigned char)(value >> 32);
                buf[i+4] = (unsigned char)(value >> 40);
                buf[i+5] = (unsigned char)(value && 0xFF);

	/*std::cout << "citanje iz sin_rom-a: ";
				std::cout << "adresa: " << (int)(addr-1) << " " << mem[addr-1] <<", vrednosti: " << value << " " << std::hex << (int)buf[i] << " " << std::hex << (int)buf[i+1] << std::endl;*/
	     }

        
        default:
            pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
            offset += sc_core::sc_time(DELAY, sc_core::SC_NS);
    }
}

#endif // MEM_C
