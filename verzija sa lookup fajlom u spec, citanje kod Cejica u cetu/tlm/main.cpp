#include <systemc>
#include "cpu.hpp"
#include "ip.hpp"

using namespace sc_core;
using namespace std;

int sc_main(int argc, char* argv[])
{
        cout << "proba" << endl;
	sc_start(10, sc_core::SC_SEC);

    return 0;
}
