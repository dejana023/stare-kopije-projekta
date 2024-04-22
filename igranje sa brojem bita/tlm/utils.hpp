#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include "addr.hpp"
#include "types.hpp"
#include <systemc>
#include <iostream>
#include <string.h>
#include <bitset>
#include <math.h>
#include <string>
#include <tlm>
#include <vector>

using namespace std;
using namespace sc_core;
using namespace tlm;
using namespace sc_dt;

static const int TOTAL_SIZE = 48;//num of bits in num_t
static const int WHOLE_SIZE = 30;//num of whole bits in num_t
static const int FRAC_SIZE = 48 - 30;//num of fractional bits in num_t
static const int CHAR_LEN = 8;//num of bits in char
static const int BUFF_SIZE = (int) std::ceil( (float)TOTAL_SIZE / CHAR_LEN);//how many chars are needed for num_t
static const int BUFF_EXTRA = BUFF_SIZE*CHAR_LEN - TOTAL_SIZE;//how many chars are extra

int toInt(unsigned char *buf);
void intToUchar(unsigned char *buf,int val);
double toDouble(unsigned char *buf);
void doubleToUchar(unsigned char *buf, double val);

num_f to_fixed (unsigned char *buf);//change unsigned char * into num_t
void to_uchar (unsigned char *buf, num_f d);//change num_t into unsigned char *

//void fixToChar(unsigned char *c, res_type d);
//void stringToChar (unsigned char *buf, string s);

#endif
