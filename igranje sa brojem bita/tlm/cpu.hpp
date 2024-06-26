#ifndef CPU_H
#define CPU_H
#define SC_INCLUDE_FX

#include "../spec/image.h"
#include "../spec/ipoint.h"
#include "../spec/imload.h"
#include "../spec/fasthessian.h"
#include "utils.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <sys/time.h>
#include <string.h>
#include <cmath>
#include <systemc>
#include <string>
#include <fstream>
#include <deque>
#include <vector>
#include <array>
#include <algorithm>
#include "types.hpp"
#include "addr.hpp"
#include "tlm_utils/tlm_quantumkeeper.h"


//using namespace surf;
using namespace std;
using namespace sc_core;

//DA LI OVO MOZE OVAKO PROVERI!!!!
//IZBACUJE GRESU KADA SE OVO NE STAVI DA NISU DEFINISANE A OVAKO JE URADJENO U FASTEHIANU DA MOGU DA SE KORISTE
//class Ipoint;
//class Image;
//class FastHessian;

//typedef sc_dt::sc_int<11> num_i;
//typedef sc_dt::sc_fixed<48, 30, sc_dt::SC_TRN, sc_dt::SC_SAT> num_f;

SC_MODULE(Cpu)
{

	public:
		SC_HAS_PROCESS(Cpu);
		//Cpu(sc_module_name name, char* image_file_name);
		Cpu(sc_module_name name, const std::string& image_name, int argc, char **argv);
		tlm_utils::simple_initiator_socket<Cpu> interconnect_socket;

		surf::Image *_iimage; //= nullptr;
		surf::Ipoint *_current; //= nullptr;
		std::vector<std::vector<std::vector<num_f>>> _index;
		bool _doubleImage = false;
		num_i _VecLength = 0;
		//num_i _IndexSize /*= 4*/;
		num_i _MagFactor = 0;
		num_i _OriSize = 0;
		//num_i _width /*= 0*/, _height /*= 0*/;

		num_f _sine = 0.0, _cose = 1.0;
		std::vector<std::vector<num_f>> _Pixels;

		num_f _lookup1[83], _lookup2[40];
		
		int VLength; // Length of the descriptor vector
		
		double scale;
		int x;
		int y;	
		typedef vector<vector<num_f>> vector2D;
		typedef vector<num_f> vector1D;

	protected:
	
		void transform2Dto1D(vector2D source_vector, vector1D& dest_vector);
	
		sc_core::sc_time offset;
		
		void software();
		void createVector(double scale, double row, double col);
		void normalise();
		void createLookups();
		void initializeGlobals(surf::Image *im, bool dbl, int insi);
		int getVectLength();
		void setIpoint(surf::Ipoint* ipt);
		void assignOrientation();
		void makeDescriptor();
		void saveIpoints(string sFileName, const vector< surf::Ipoint >& ipts);
		
		void read_mem(sc_uint<64> addr, unsigned char *all_data, int length);
		void write_mem(sc_uint<64> addr, num_f val);
		int read_hard_int(sc_uint<64> addr);
		double read_hard_double(sc_uint<64> addr);
		void write_hard_int(sc_uint<64> addr, int val);
		void write_hard_double(sc_uint<64> addr, double val);
		
	private:
		surf::Image *_im;
		string fn; 
	
};


#endif
