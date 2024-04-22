#ifndef IP_C
#define IP_C
#include "ip.hpp"

SC_HAS_PROCESS(Ip);

Ip::Ip(sc_module_name name) :
    sc_module(name),
    ready(1)
    
{
_index.resize(_IndexSize, std::vector<std::vector<num_f>>(
        _IndexSize, std::vector<num_f>(4, 0.0f)));
        interconnect_socket.register_b_transport(this, &Ip::b_transport);
        cout << "IP constructed" << endl;
        
               
    for (int i = 0; i < _IndexSize; i++) {
    for (int j = 0; j < _IndexSize; j++) {
      for (int k = 0; k < 4; k++)
        _index[i][j][k] = 0.0;
    }
  }
}
    
    
Ip::~Ip()
{
	SC_REPORT_INFO("Ip", "Destroyed");
}


void Ip::b_transport(pl_t& pl, sc_time& offset)
{
    tlm_command cmd = pl.get_command();
    sc_dt::uint64 addr = pl.get_address();
    unsigned char *buf = pl.get_data_ptr();
    unsigned int len = pl.get_data_length();
    pl.set_response_status(TLM_OK_RESPONSE);

    switch (cmd)
    {
        case TLM_WRITE_COMMAND:
            switch(addr)
            {
                case addr_start:
                    start = toInt(buf);
                    proc(scale1,row,col);
                    				 // cout << "start IP: " << start << endl;
                    break;
                    
             /*   case addr_fracy:
                    fracy = toDouble(buf);
                      cout << "fracy IP: " << fracy << endl;
                    break;
                    
                case addr_fracx:
                    fracx = toDouble(buf);
                      cout << "fracx IP: " << fracx << endl;
                    break;
                    
		case addr_fracr:
                    fracr = toDouble(buf);
                      cout << "fracr IP: " << fracr << endl;
                    break;
                    
                case addr_fracc:
                    fracc = toDouble(buf);
                      cout << "fracc IP: " << fracc << endl;
                    break;*/
                
               /* case addr_radius:
                    radius = toDouble(buf);
                      cout << "radius IP: " << radius << endl;
                    break;*/
                    
               /* case addr_iradius:
                    iradius = toInt(buf);
                      cout << "iradius IP: " << iradius << endl;
                    break;
                    
                case addr_spacing:
                    spacing = toDouble(buf);
                      cout << "spacing IP: " << spacing << endl;
                    break;
                    
                case addr_iy:
                    iy = toInt(buf);
                     cout << "iy IP: " << iy << endl;
                    break;
                    
                case addr_ix:
                    ix = toInt(buf);
                      cout << "ix IP: " << ix << endl;
                    break;*/
                    
                       
               /* case addr_r:
                    r = toInt(buf);
                                                 // cout << "r IP: " << r << endl;
                    break;
                case addr_c:
                    c = toInt(buf);
                                               // cout << "c IP: " << c << endl;
                    break;
                case addr_rpos:
                    rpos = toDouble(buf);
                                                 // cout << "rpos IP: " << rpos << endl;
                    break;
                case addr_cpos:
                    cpos = toDouble(buf);
                                                //  cout << "cpos IP: " << cpos << endl;
                    break;
                case addr_rx:
                    rx = toDouble(buf);
                                                //  cout << "rx IP: " << rx << endl;
                    break;
                case addr_cx:
                    cx = toDouble(buf);
                  */                              //  cout << "cx IP: " << cx << endl;
               /*     break;
                case addr_step:
                    step = toInt(buf);
                                                 cout << "step IP: " << step << endl;
                    break;*/
                 case addr_scale1:
                     scale1 = toDouble(buf);
                     break;
                     
                 case addr_row:
                     row = toDouble(buf);
                     break;
                     
                 case addr_col:
                     col = toDouble(buf);
                     break;   
                    
                case addr_cose:
                    _cose = toDouble(buf);
                                                 // cout << "_cose IP: " << _cose << endl;
                    break;
                case addr_sine:
                    _sine = toDouble(buf);
                             			// cout << "_sine IP: " << _sine << endl;
                    break;
                    
                default:
                    pl.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
                    cout << "Wrong address" << endl;
            }
            break;
                
        case TLM_READ_COMMAND:
            switch(addr)
            {
                case addr_ready:
                    intToUchar(buf, ready);
                    break;
                default:
                    pl.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
            }
            break;
            
        default:
            pl.set_response_status(TLM_COMMAND_ERROR_RESPONSE);
            cout << "Wrong command" << endl;
    }
    offset += sc_time(10, SC_NS);
}

void Ip::proc(num_f scale, num_f row, num_f col)
{

    
        
    if (start == 1 && ready == 1)
    {
    
        //cout << "Uslo u start=1 i ready=1" << endl;
        ready = 0;
        offset += sc_time(DELAY, SC_NS);
    }
    
    else if (start == 0 && ready == 0)
    {
       cout << "Processing started" << endl;

        num_i i, j, iradius, iy, ix;
  num_f spacing, radius, rpos, cpos, rx, cx;
  num_i step = MAX((int)(scale/2 + 0.5),1);
  
  //cout << "step: " << step << endl;
  
  iy = (num_i) (row + 0.5);
  ix = (num_i) (col + 0.5);

  num_f fracy = row-iy;
  num_f fracx = col-ix;
  num_f fracr =   _cose * fracy + _sine * fracx;
  num_f fracc = - _sine * fracy + _cose * fracx;
  
  // The spacing of _index samples in terms of pixels at this scale
  spacing = scale * 3;

  // Radius of _index sample region must extend to diagonal corner of
  // _index patch plus half sample for interpolation.
  radius = 1.4 * spacing * (_IndexSize + 1) / 2.0;
  
  cout<< radius << endl;
  iradius = (num_i) (radius/step + 0.5);      
               
    for (int i = -iradius; i <= iradius; i++)
    for (int j = -iradius; j <= iradius; j++) {
    
    
      // Rotate sample offset to make it relative to key orientation.
      // Uses (x,y) coords.  Also, make subpixel correction as later image
      // offset must be an integer.  Divide by spacing to put in _index units.
      rpos = (step*(_cose * i + _sine * j) - fracr) / spacing;
      cpos = (step*(- _sine * i + _cose * j) - fracc) / spacing;
      
      // Compute location of sample in terms of real-valued _index array
      // coordinates.  Subtract 0.5 so that rx of 1.0 means to put full
      // weight on _index[1] (e.g., when rpos is 0 and _IndexSize is 3.
      rx = rpos + _IndexSize / 2.0 - 0.5;
      cx = cpos + _IndexSize / 2.0 - 0.5;
      
      //cout << "rx: " << rx << endl;
      //cout << "cx: " << cx << endl;

      // Test whether this sample falls within boundary of _index patch
      if (rx > -1.0 && rx < (double) _IndexSize  &&
          cx > -1.0 && cx < (double) _IndexSize) {
          
           r = iy + i*step;
           c = ix + j*step; 
           
          // cout << "r: " << r << endl;
         //  cout << "c: " << c << endl;
            
            AddSample(r, c, rpos, cpos,
                  rx, cx,step);
                  
                    num_f* index1D = new num_f[_IndexSize * _IndexSize * 4];
        
        //index_1d = new num_f[_IndexSize * _IndexSize * 4];
                  int index1D_index = 0;
                  for (int i = 0; i < _IndexSize; i++)
                  {
                      for (int j = 0; j < _IndexSize; j++)
                      {
                          for (int k = 0; k < _IndexSize; k++) {
                              index1D[index1D_index++] = static_cast<num_f>(_index[i][j][k]);
                          }
                      }
                  }
                  
                  for (long unsigned int i = 0; i < _IndexSize*_IndexSize*4; ++i)
                  {
                      mem.push_back(index1D[i]);
                  }
                  
                  
                  
          //ISPIS PIXELSA    
             /* for (int y = 0; y < _width; ++y)
    	      {
                  for (int x = 0; x < _height; ++x)
                  {
                      std::cout << static_cast<int>(pixels1D[y * _height + x]) << " ";
                  }
                  std::cout << std::endl;
              }  */
              
              //std::cout << "///////////////////////////////////////////////////////////" << endl;
                
                  for (int i = 0; i < _IndexSize * _IndexSize * 4; i++)
                  {
                      pl_t pl;
    		      offset += sc_core::sc_time(DELAY, sc_core::SC_NS);
    		      unsigned char* buf;
    		      buf = (unsigned char*)&mem[i];
   		      pl.set_address(addr_index1+i);
  		      pl.set_data_length(1);
  		      pl.set_data_ptr(buf);
   		      pl.set_command(tlm::TLM_WRITE_COMMAND);
  		      pl.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
  		      mem_socket->b_transport(pl, offset);
                  
                  }
    
    
        //delete[] index_1d;
        //delete[] pixels1D;
   
    
        //cout << "Upis iz IP-a u memoriju zavrsen" << endl;
       
    
   // cout << "Doslo na kraj PlaceInIndex" << endl;
   
 cout << "Sadržaj _index niza:" << endl;
    for (int i = 0; i < _IndexSize; ++i) {
        for (int j = 0; j < _IndexSize; ++j) {
            for (int k = 0; k < 4; ++k) {
                cout << "_index[" << i << "][" << j << "][" << k << "]: " << _index[i][j][k] << endl;
            }
        }
    } 
        }
        
     
     }
     ready = 1;
       
    }

}


void Ip::AddSample(num_i r, num_i c, num_f rpos,
                     num_f cpos, num_f rx, num_f cx, num_i step) {
                     
                    // cout<< "Uslo u addsample" << endl;
                  //   cout << "start" << start << ", ready " << ready << endl;
                  
                  //static int callCount = 0;
    
    // Inkrementiraj brojac
    //callCount++;
    
    // Ispisi vrednost brojaca u terminalu
    //cout << "AddSample je pozvan " << callCount << " puta." << endl;
    
     vector<num_f> pixels1D;
       int pixels1D_index = 0;
     
       
/*for (int i = 0; i < _width*_height; i++) {
    int n = addr_Pixels1 + i;
    num_f data = read_mem(n);
    //cout << data << endl;
    pixels1D.push_back(data);
    cout << pixels1D[pixels1D_index++] << endl;
}*/

       for (int w = 0; w < _width; w++)
       {
           offset += sc_core::sc_time(DELAY, sc_core::SC_NS);
           for (int h = 0; h < _height; h++)
           {
               offset += sc_core::sc_time(DELAY, sc_core::SC_NS);
               pixels1D.push_back( read_mem(addr_Pixels1 + (w * _height + h)));
           }
       }
       
       
      /* for (int i = 0 ; i  < _width*_height; i++) {
         
               std::cout << static_cast<num_f>((pixels1D[i])) << " ";
       }*/
          
          
        num_f** _Pixels = new num_f*[_width];
        for (int i = 0; i < _width; i++) {
            _Pixels[i] = new num_f[_height];
        }
        

    
    
        int pixels1D_index2 = 0;
        for (int w = 0; w < _width; w++) {
            for (int h = 0; h < _height; h++) {
                _Pixels[w][h] = static_cast<num_f>(pixels1D[pixels1D_index2++]);
            }
        }  
    
    if (r < 1+step  ||  r >= _height - 1-step  ||
             c < 1+step  ||  c >= _width - 1-step) {
            // cout << "uslo u return"<<endl;
             return;}
                                                   
       // cout << "r: " << r << endl;
       // cout << "c: " << c << endl;
    
        /*for (int i = 0; i < _width; i++) {
        for (int j = 0; j < _height; j++) {
            std::cout << _Pixels[i][j] << " ";
        }
        std::cout << std::endl;
        }*/
       
        num_f weight;
        num_f dx, dy;
    
        std::vector<num_f> _lookup2(40);
        for (int n=0;n<40;n++)
            _lookup2[n]=exp(-((num_f)(n+0.5))/8.0);


        /*// Ispisivanje sadržaja niza _lookup2
        for (const auto& value : _lookup2) {
            std::cout << value << " ";
        }
        std::cout << std::endl;*/
        
       // cout << "rpos: " << rpos << endl;
      //  cout << "cpos: " << cpos << endl
 
        weight = _lookup2[num_i(rpos * rpos + cpos * cpos)];
    
     //  cout << "weight: " << weight << endl;
        
        
  
        num_f dxx, dyy, dxx1, dxx2, dyy1, dyy2;
// cout << "r: " << r << endl;
// cout << "step: " << step << endl;
// cout << "c: " << c << endl;

// num_f seg = _Pixels[7][4]+_Pixels[2][11]-_Pixels[2][14]-_Pixels[7][11];
// cout << seg <<endl;
         
        // Izračunavanje dxx i dyy koristeći privremene promenljive
    dxx1 = _Pixels[r + step + 1][c + step + 1] + _Pixels[r - step][c] - _Pixels[r - step][c + step + 1] - _Pixels[r + step + 1][c];
 //   cout << "dxx1: " << dxx1 << endl;

    dxx2 = _Pixels[r + step + 1][c + 1] + _Pixels[r - step][c - step] - _Pixels[r - step][c + 1] - _Pixels[r + step + 1][c - step];
  //  cout << "dxx2: " << dxx2 << endl;

    dyy1 = _Pixels[r + 1][c + step + 1] + _Pixels[r - step][c - step] - _Pixels[r - step][c + step + 1] - _Pixels[r + 1][c - step];
   // cout << "dyy1: " << dyy1 << endl;

    dyy2 = _Pixels[r + step + 1][c + step + 1] + _Pixels[r][c - step] - _Pixels[r][c + step + 1] - _Pixels[r + step + 1][c - step];
  //  cout << "dyy2: " << dyy2 << endl;

    // Izračunavanje dxx i dyy
    dxx = weight * (dxx1 - dxx2);
  //  cout << "dxx: " << dxx << endl;

    dyy = weight * (dyy1 - dyy2);
  //  cout << "dyy: " << dyy << endl;


       /* dxx = weight*get_wavelet2(_Pixels, c, r, step);
    
        cout << "dxx: " << dxx << endl;
        dyy = weight*get_wavelet1(_Pixels, c, r, step);
             //   cout << "dyy: " << dyy << endl;*/
        dx = _cose*dxx + _sine*dyy;
        //        cout << "dx: " << dx << endl;
        dy = _sine*dxx - _cose*dyy;
        //     cout << "dy: " << dy << endl;

        PlaceInIndex(dx, (dx<0?0:1), dy, (dy<0?2:3), rx, cx);
        
                 
    for (int i = 0; i < _width; i++) {
            delete[] _Pixels[i];
        }
        delete[] _Pixels;
   
    
}
    


void Ip::PlaceInIndex(num_f mag1, num_i ori1, num_f mag2, num_i ori2, num_f rx, num_f cx) {
    
  //  cout << "Uslo u PlaceInIndex" << endl;
    
    num_i ri = std::max(0, std::min(static_cast<int>(_IndexSize - 1), static_cast<int>(rx)));
    num_i ci = std::max(0, std::min(static_cast<int>(_IndexSize - 1), static_cast<int>(cx)));
  
    num_f rfrac = rx - ri;
    num_f cfrac = cx - ci;
  
    rfrac = std::max(0.0f, std::min(float(rfrac), 1.0f));
    cfrac = std::max(0.0f, std::min(float(cfrac), 1.0f));
  
    num_f rweight1 = mag1 * (1.0 - rfrac);
    num_f rweight2 = mag2 * (1.0 - rfrac);
    num_f cweight1 = rweight1 * (1.0 - cfrac);
    num_f cweight2 = rweight2 * (1.0 - cfrac);
    
  // cout << "ri: " << ri << endl;
  //  cout << "ci: " << ci << endl;
  // cout << "rweight1: " << rweight1 << endl;
 //  cout << "cweight1: " << cweight1 << endl;
  //  cout << "_IndexSize: " << _IndexSize << endl;
    
  //  cout << "Ispred ifova u PlaceInIndex" << endl;


    if (ri >= 0 && ri < _IndexSize && ci >= 0 && ci < _IndexSize) {
   //         cout << "Uslo u prvi if" << endl;
   //          cout << "Pristupam _index[" << ri << "][" << ci << "][" << ori1 << "]" << endl;
  //  cout << "Pristupam _index[" << ri << "][" << ci << "][" << ori2 << "]" << endl;
        _index[ri][ci][ori1] += cweight1;
        _index[ri][ci][ori2] += cweight2;
    } else {
   //          cout << "Neuspesan pristup _index vektoru!" << endl;
   //     cout << "ri: " << ri << ", ci: " << ci << endl;
   //     cout << "_IndexSize: " << _IndexSize << endl;
    }

    if (ci + 1 < _IndexSize) {
        _index[ri][ci + 1][ori1] += rweight1 * cfrac;
        _index[ri][ci + 1][ori2] += rweight2 * cfrac;
    }

    if (ri + 1 < _IndexSize) {
        _index[ri + 1][ci][ori1] += mag1 * rfrac * (1.0 - cfrac);
        _index[ri + 1][ci][ori2] += mag2 * rfrac * (1.0 - cfrac);
    }
    
     
    
}



//VIDI JE L DOBRA OVA FUNKCIJA
void Ip::write_mem(sc_uint<64> addr, num_f val)
{
    pl_t pl;
    unsigned char buf[6];
    doubleToUchar(buf,val);
    pl.set_address(addr);
    pl.set_data_length(1);
    pl.set_data_ptr(buf);
    pl.set_command(tlm::TLM_WRITE_COMMAND);
    pl.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
    mem_socket->b_transport(pl, offset);
}


/*void Ip::read_mem(sc_uint<64> addr)
{
    //for (int i=0; i < _width*_height; i++) {
    pl_t pl;
    unsigned char* buf = new unsigned char;
    pl.set_address(addr);
    pl.set_data_length(1);
    pl.set_data_ptr(buf);
    pl.set_command(tlm::TLM_READ_COMMAND);
    pl.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
    mem_socket->b_transport(pl, offset);
    //mem.push_back(num_f(*buf));
    //mem[i] = ((num_f*)buf)[i];
    //cout << mem[i] << endl;
    //delete buf;
    //}
}*/

/*num_f Ip::read_mem(sc_uint<64> addr)
{
    pl_t pl;
    num_f result;
    unsigned char* buf = reinterpret_cast<unsigned char*>(&result);
    pl.set_address(addr);
    pl.set_data_length(6); // Postavljamo dužinu podataka na dužinu tipa num_f
    pl.set_data_ptr(buf); // Postavljamo pokazivač na bafer za čitanje
    pl.set_command(tlm::TLM_READ_COMMAND);
    pl.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
    mem_socket->b_transport(pl, offset);
    cout << result << endl;
    return result;
}*/

//OVAJ RADI S NULAMA
/*num_f Ip::read_mem(sc_uint<64> addr)
{
    pl_t pl;
    num_f result;
    pl.set_address(addr);
    pl.set_data_length(1);
    pl.set_data_ptr(reinterpret_cast<unsigned char*>(&result));
    pl.set_command(tlm::TLM_READ_COMMAND);
    pl.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
    mem_socket->b_transport(pl, offset);
    //cout << result.to_double() << endl;
    return result;
}*/

num_f Ip::read_mem(sc_dt::sc_uint<64> addr)
{
	pl_t pl;
	sc_dt::sc_int <64> val;
	unsigned char buf[6];
	pl.set_address(addr);
	pl.set_data_length(6); 
	pl.set_data_ptr(buf);
	pl.set_command( tlm::TLM_READ_COMMAND );
	pl.set_response_status ( tlm::TLM_INCOMPLETE_RESPONSE );
	mem_socket->b_transport(pl,offset);
	
	num_f mega = toNum_f(buf);
	
	//cout << "buf iz ip-a: " << mega << endl;
	//cout << addr << endl;

	return toNum_f(buf);
}

/*num_f Ip::read_mem(sc_uint<64> addr)
{
    pl_t pl;
    unsigned char buf[sizeof(num_f)]; // Napravite bafer odgovarajuće veličine
    pl.set_address(addr);
    pl.set_data_length(sizeof(num_f)); // Postavite dužinu podataka na veličinu num_f
    pl.set_data_ptr(buf);
    pl.set_command(tlm::TLM_READ_COMMAND);
    pl.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
    mem_socket->b_transport(pl, offset);
    return toDouble(buf); // Konvertujte podatke iz bafera u num_f pomoću vaše funkcije
}*/




/*num_f Ip::read_mem(sc_uint<64> addr)
{
    pl_t pl;
    num_f result;
    unsigned char* buf = new unsigned char[sizeof(result)];
    pl.set_address(addr);
    pl.set_data_length(sizeof(result));
    pl.set_data_ptr(buf);
    pl.set_command(tlm::TLM_READ_COMMAND);
    
    // Izvršavamo TLM transakciju
    unsigned int status = mem_socket->transport_dbg(pl);
    
    //cout << "Reading from address: " << addr << endl;
    //cout << "Data length: " << pl.get_data_length() << endl;
    //cout << "Response status: " << status << endl;

    if (status == 1) { // Ako je status 1, tada je odgovor OK
        // Kopiramo podatke iz bafera u rezultujući tip
        result = toNum_f(buf);
        //cout << "Read result: " << result << endl;
    } else {
        //cout << "Error reading memory at address " << addr << endl;
    }
    
    // Oslobađamo alociranu memoriju
    delete[] buf;
    
    return result;
}*/










#endif // IP_C
