#ifndef ADDR_H
#define ADDR_H

#define OriHistTh 0.8
#define window M_PI/3
#define IndexSigma 1.0

#define get_sum(I, x1, y1, x2, y2) (I[y1+1][x1+1] + I[y2][x2] - I[y2][x1+1] - I[y1+1][x2])
#define get_wavelet1(IPatch, x, y, size) (get_sum(IPatch, x + size, y, x - size, y - size) - get_sum(IPatch, x + size, y + size, x - size, y))
#define get_wavelet2(IPatch, x, y, size) (get_sum(IPatch, x + size, y + size, x, y - size) - get_sum(IPatch, x, y + size, x - size, y - size))

#define _height 129
#define _width 129
#define _IndexSize 4

#define DELAY 10

//ADRESE
#define addr_Pixels1 0
#define addr_index1 17000

#define addr_start 100300
#define addr_ready 100301
#define addr_scale1 100302
#define addr_row 100303
#define addr_col 100304
#define addr_sine 100305
#define addr_cose 100306

#endif
