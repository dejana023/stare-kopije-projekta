#ifndef IP_H
#define IP_H
#define SC_INCLUDE_FX

#include <iostream>
#include <systemc>
#include <string>
#include <fstream>
#include <deque>
#include <vector>
#include <array>
#include <algorithm>
#include "utils.hpp"
#include "types.hpp"
#include "addr.hpp"
#include "tlm_utils/tlm_quantumkeeper.h"

using namespace std;
using namespace sc_core;

SC_MODULE(Ip)
{
    public:
        //SC_HAS_PROCESS(Ip);
        Ip(sc_module_name name);
        ~Ip();

        tlm_utils::simple_target_socket<Ip> interconnect_socket;
        tlm_utils::simple_initiator_socket<Ip> mem_socket;

    protected:
        void b_transport(pl_t&, sc_time&);
        
        void AddSample(num_i r, num_i c, num_f rpos, num_f cpos, num_f rx, num_f cx, num_i step);
        void PlaceInIndex(num_f mag1, num_i ori1, num_f mag2, num_i ori2, num_f rx, num_f cx);
        
        void write_mem(sc_uint<64> addr, num_f val);
        num_f read_mem(sc_uint<64> addr);
        
        vector<num_f> mem;

        sc_core::sc_time offset;
        
        sc_uint<1> ready;
        sc_uint<1> start;

        std::vector<num_f> _lookup2;     
        num_i r;
        num_i c;
        num_f rpos;
        num_f cpos;
        num_f rx;
        num_f cx;
        num_i step;
        num_f _cose;
        num_f _sine;
        std::vector<std::vector<num_f>> _Pixels;
        std::vector<std::vector<std::vector<num_f>>> _index;    
};

        
#endif
