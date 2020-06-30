#ifndef _FFT_INC_
#define _FFT_INC_

//------------------------------------
//  fft.h
//  Fast Fourier Transform
//  (c) Reliable Software, 1996
//------------------------------------
class Complex;
class Fft
{
private:
	int			_Points;
	int			_logPoints;
	double		_sqrtPoints;
	int		   *_aBitRev;       // bit reverse vector
	Complex	   *_X;             // in-place fft array
	Complex	  **_W;             // exponentials
	double     *_aTape;         // recording tape
	double	   *_window;

public:
	enum {
		rectangle	= 0,
		hanning		= 1,
		hamming		= 2,
		blackman	= 3,
	};

public:
    Fft(int points, int window);
    ~Fft();

	void copy(int pos, short sample);
	void transform(int* spec);
};

#endif
