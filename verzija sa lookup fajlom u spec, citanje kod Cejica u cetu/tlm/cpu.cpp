#ifndef CPU_C
#define CPU_C
#include "cpu.hpp"

Cpu(sc_module_name name, char* image_file_name, char* labels_file_name):sc_module(name), image_file_name(image_file_name, labels_file_name(labels_file_name))
{
    SC_THREAD(software);
    p_port0.bind(sig0);
    sig0 = sc_dt::SC_LOGIC_0;
    labels.clear();
	
    cout << "Cpu constucted" << endl;
}

void Cpu:software()
{
    int samplingStep = 2; // Initial sampling step (default 2)
    int octaves = 4; // Number of analysed octaves (default 4)
    double thres = 4.0; // Blob response treshold
    bool doubleImageSize = false; // Set this flag "true" to double the image size
    int initLobe = 3; // Initial lobe size, default 3 and 5 (with double image size)
    int indexSize = 4; // Spatial size of the descriptor window (default 4)
    struct timezone tz; struct timeval tim1, tim2; // Variables for the timing measure
    unsigned char *buf; //za prebacivanje podataka iz memorije u IP
    
    ImLoad im(image_file_name);
    
      // Start measuring the time
    gettimeofday(&tim1, &tz);

    // Create the integral image
    Image iimage(im, doubleImageSize);

    // Start finding the SURF points
      cout << "Finding SURFs...\n";

    // These are the interest points
    vector< Ipoint > ipts;
    ipts.reserve(300);

    // Extract interest points with Fast-Hessian
    FastHessian fh(&iimage, /* pointer to integral image */
                   ipts,
                   thres, /* blob response threshold */
                   doubleImageSize, /* double image size flag */
                   initLobe * 3 /* 3 times lobe size equals the mask size */,
                   samplingStep, /* subsample the blob response map */
                   octaves /* number of octaves to be analysed */);


    fh.getInterestPoints();

    // Initialise the SURF descriptor
    initializeGlobals(&iimage, doubleImageSize, indexSize);
    // Get the length of the descriptor vector resulting from the parameters
    VLength = getVectLength();

    // Compute the orientation and the descriptor for every interest point
    for (unsigned n=0; n<ipts.size(); n++){
      setIpoint(&ipts[n]); // set the current interest point
      assignOrientation(); // assign reproducible orientation
      makeDescriptor(); // make the SURF descriptor
    }
    // stop measuring the time, we're all done
    gettimeofday(&tim2, &tz);

    // save the interest points in the output file
    saveIpoints(fn, ipts);

    // print some nice information on the command prompt
      cout << "Detection time: " <<
        (double)tim2.tv_sec + ((double)tim2.tv_usec)*1e-6 -
        (double)tim1.tv_sec - ((double)tim1.tv_usec)*1e-6 << endl;

    delete im;

    return 0;
}


void Cpu::initializeGlobals(Image *im, bool dbl = false, int insi = 4) {
    _iimage = im;
    _doubleImage = dbl;
    _IndexSize = insi;
    _MagFactor = 12 / insi; // Pretpostavka na osnovu prvobitne logike
    _OriSize = 4; // Pretpostavljena vrednost
    _VecLength = _IndexSize * _IndexSize * _OriSize; // Izračunavanje na osnovu datih vrednosti
    _width = im->getWidth();
    _height = im->getHeight();
    // Inicijalizacija _Pixels, _lookup1, _lookup2...
    createLookups(); // Popunjava _lookup1 i _lookup2 tabele
    
    
  double** tempPixels = _iimage->getPixels();
  _Pixels.resize(_height);
  for (int i = 0; i < _height; ++i) {
      _Pixels[i].resize(_width);
      for (int j = 0; j < _width; ++j) {
          _Pixels[i][j] = static_cast<num_f>(tempPixels[i][j]);
      }
  }

 // allocate _index
_index.resize(_IndexSize, std::vector<std::vector<num_f>>(
    _IndexSize, std::vector<num_f>(_OriSize, 0.0f)));

  // initial sine and cosine
  _sine = 0.0;
  _cose = 1.0;
}


int Cpu::getVectLength() {
    return _VecLength;
}


void setIpoint(Ipoint* ipt) {
    _current = ipt;
}


void Cpu::assignOrientation() {
  scale = (1.0+_doubleImage) * _current->scale;
  x = (int)((1.0+_doubleImage) * _current->x + 0.5);
  y = (int)((1.0+_doubleImage) * _current->y + 0.5);
  
  int pixSi = (int)(2*scale + 1.6);
  const int pixSi_2 = (int)(scale + 0.8);
  double weight;
  const int radius=9;
  double dx=0, dy=0, magnitude, angle, distsq;
  const double radiussq = 81.5;
  int y1, x1;
  int yy, xx;

  vector< pair< double, double > > values;
  for (yy = y - pixSi_2*radius, y1= -radius; y1 <= radius; y1++,
       yy+=pixSi_2){
    for (xx = x - pixSi_2*radius, x1 = -radius; x1 <= radius; x1++,
         xx+=pixSi_2) {
      // Do not use last row or column, which are not valid
      if (yy + pixSi + 2 < _height &&
          xx + pixSi + 2 < _width &&
          yy - pixSi > -1 &&
          xx - pixSi > -1) {
        distsq = (y1 * y1 + x1 * x1);
        
        if (distsq < radiussq) {
          weight = _lookup1[(int)distsq];
          dx = get_wavelet2(_iimage->getPixels(), xx, yy, pixSi);
          dy = get_wavelet1(_iimage->getPixels(), xx, yy, pixSi);

          magnitude = sqrt(dx * dx + dy * dy);
          if (magnitude > 0.0){
            angle = atan2(dy, dx);
            values.push_back( make_pair( angle, weight*magnitude ) );
          }
        }
      }
    }
  }
  
  double best_angle = 0;

  if (values.size()) {
    sort( values.begin(), values.end() );
    int N = values.size();

    float d2Pi = 2.0*M_PI;
    
    for( int i = 0; i < N; i++ ) {
      values.push_back( values[i] );
      values.back().first += d2Pi;
    }

    double part_sum = values[0].second;
    double best_sum = 0;
    double part_angle_sum = values[0].first * values[0].second;

    for( int i = 0, j = 0; i < N && j<2*N; ) {
      if( values[j].first - values[i].first < window ) {
        if( part_sum > best_sum ) {
          best_angle  = part_angle_sum / part_sum;
          best_sum = part_sum;
        }
        j++;
        part_sum += values[j].second;
        part_angle_sum += values[j].second * values[j].first;
      }
      else {
        part_sum -= values[i].second;
        part_angle_sum -= values[i].second * values[i].first;
        i++;
      }
    }
  }
  _current->ori = best_angle;
}


void Cpu::makeDescriptor() {
  _current->allocIvec(_VecLength);
  
  // Initialize _index array
  for (int i = 0; i < _IndexSize; i++) {
    for (int j = 0; j < _IndexSize; j++) {
      for (int k = 0; k < _OriSize; k++)
        _index[i][j][k] = 0.0;
    }
  }

    // calculate _sine and co_sine once
    _sine = sin(_current->ori);
    _cose = cos(_current->ori);
    
    // Produce _upright sample vector
    createVector(1.65*(1+_doubleImage)*_current->scale,
                 (1+_doubleImage)*_current->y,
                 (1+_doubleImage)*_current->x);
 
  int v = 0;
  for (int i = 0; i < _IndexSize; i++){
    for (int j = 0; j < _IndexSize; j++){
      for (int k = 0; k < _OriSize; k++)
        _current->ivec[v++] = _index[i][j][k];
    }
  }
  normalise();
}


void Cpu::createVector(double scale, double row, double col) {
   int i, j, iradius, iy, ix;
  double spacing, radius, rpos, cpos, rx, cx;
  int step = MAX((int)(scale/2 + 0.5),1);
  
  iy = (int) (y + 0.5);
  ix = (int) (x + 0.5);

  double fracy = y-iy;
  double fracx = x-ix;
  double fracr =   _cose * fracy + _sine * fracx;
  double fracc = - _sine * fracy + _cose * fracx;
  
  // The spacing of _index samples in terms of pixels at this scale
  spacing = scale * _MagFactor;

  // Radius of _index sample region must extend to diagonal corner of
  // _index patch plus half sample for interpolation.
  radius = 1.4 * spacing * (_IndexSize + 1) / 2.0;
  iradius = (int) (radius/step + 0.5);
  
  // Examine all points from the gradient image that could lie within the
  // _index square.
  for (i = -iradius; i <= iradius; i++)
    for (j = -iradius; j <= iradius; j++) {
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

      // Test whether this sample falls within boundary of _index patch
      if (rx > -1.0 && rx < (double) _IndexSize  &&
          cx > -1.0 && cx < (double) _IndexSize) {
          
          num_i r = iy + i*step;
          num_i c = ix + j*step; 
          
          pl_t pl;
          
    unsigned char* buf =  reinterpret_cast<unsigned char*> (r);
    pl.set_address(22002);
	  pl.set_data_length(1);
	  pl.set_command(TLM_WRITE_COMMAND);
	  pl.set_data_ptr(buf);
	  s_cp_i0->b_transport(pl, offset);
	  assert(pl.get_response_status() == TLM_OK_RESPONSE);
	  
	  unsigned char* buf =  reinterpret_cast<unsigned char*> (c);
    pl.set_address(22003);
	  pl.set_data_length(1);
	  pl.set_command(TLM_WRITE_COMMAND);
	  pl.set_data_ptr(buf);
	  s_cp_i0->b_transport(pl, offset);
	  assert(pl.get_response_status() == TLM_OK_RESPONSE);
	  
	  unsigned char* buf =  reinterpret_cast<unsigned char*> (rpos);
    pl.set_address(22004);
	  pl.set_data_length(1);
	  pl.set_command(TLM_WRITE_COMMAND);
	  pl.set_data_ptr(buf);
	  s_cp_i0->b_transport(pl, offset);
	  assert(pl.get_response_status() == TLM_OK_RESPONSE);
	  
	  unsigned char* buf =  reinterpret_cast<unsigned char*> (cpos);
    pl.set_address(22005);
	  pl.set_data_length(1);
	  pl.set_command(TLM_WRITE_COMMAND);
	  pl.set_data_ptr(buf);
	  s_cp_i0->b_transport(pl, offset);
	  assert(pl.get_response_status() == TLM_OK_RESPONSE);
	  
	  unsigned char* buf =  reinterpret_cast<unsigned char*> (rx);
    pl.set_address(22006);
	  pl.set_data_length(1);
	  pl.set_command(TLM_WRITE_COMMAND);
	  pl.set_data_ptr(buf);
	  s_cp_i0->b_transport(pl, offset);
	  assert(pl.get_response_status() == TLM_OK_RESPONSE);
	  
	  unsigned char* buf =  reinterpret_cast<unsigned char*> (cx);
    pl.set_address(22007);
	  pl.set_data_length(1);
	  pl.set_command(TLM_WRITE_COMMAND);
	  pl.set_data_ptr(buf);
	  s_cp_i0->b_transport(pl, offset);
	  assert(pl.get_response_status() == TLM_OK_RESPONSE);
	  
	  unsigned char* buf =  reinterpret_cast<unsigned char*> (scale);
    pl.set_address(22008);
	  pl.set_data_length(1);
	  pl.set_command(TLM_WRITE_COMMAND);
	  pl.set_data_ptr(buf);
	  s_cp_i0->b_transport(pl, offset);
	  assert(pl.get_response_status() == TLM_OK_RESPONSE);

    unsigned char* buf =  reinterpret_cast<unsigned char*> (_cose);
    pl.set_address(22009);
	  pl.set_data_length(1);
	  pl.set_command(TLM_WRITE_COMMAND);
	  pl.set_data_ptr(buf);
	  s_cp_i0->b_transport(pl, offset);
	  assert(pl.get_response_status() == TLM_OK_RESPONSE);

    unsigned char* buf =  reinterpret_cast<unsigned char*> (_sine);
    pl.set_address(22010);
	  pl.set_data_length(1);
	  pl.set_command(TLM_WRITE_COMMAND);
	  pl.set_data_ptr(buf);
	  s_cp_i0->b_transport(pl, offset);
	  assert(pl.get_response_status() == TLM_OK_RESPONSE);
	  
	//startovati slanjem jedinice u startni registar u ip-u  
       // AddSample(r, c, rpos, cpos,
                //  rx, cx, (int)(scale));
                
        }  
    }
    
    
}

//Normalise descriptor vector for illumination invariance for Lambertian surfaces
void Cpu::normalise() {
  num_f val, sqlen = 0.0, fac;
  for (num_i i = 0; i < _VecLength; i++){
    val = _current->ivec[i];
    sqlen += val * val;
  }
  fac = 1.0/sqrt(sqlen);
  for (num_i i = 0; i < _VecLength; i++)
    _current->ivec[i] *= fac;
}

//Create _lookup tables
void Cpu::createLookups() {
  for (int n=0;n<83;n++)
    _lookup1[n]=exp(-((double)(n+0.5))/12.5);

  for (int n=0;n<40;n++)
    _lookup2[n]=exp(-((double)(n+0.5))/8.0);
}
