//------------------------------------
//  fft.cpp
//  The implementation of the 
//  Fast Fourier Transform algorithm
//  (c) Reliable Software, 1996
//------------------------------------
#include "fft.h"
#include "complex.h"
#include <string.h>

// Points must be a power of 2
Fft::Fft (int points, int window)
{
	double pi = (2.0 * asin(1.0));
	_Points = points;
    _aTape = new double [_Points];
    _window = new double [_Points];
#if 0
    // 1 kHz calibration wave
    for (int i = 0; i < _Points; i++)
        _aTape[i] = 1600 * sin (2 * pi * 1000. * i / _sampleRate);
#else
    for (int i = 0; i < _Points; i++)
        _aTape[i] = 0;
#endif
    _sqrtPoints = sqrt((double)_Points);
    // calculate binary log
    _logPoints = 0;
	points--;
    while (points != 0)
    {
		points >>= 1;
		_logPoints++;
    }

    _aBitRev = new int [_Points];
    _X = new Complex[_Points];
    _W = new Complex* [_logPoints+1];
    // Precompute complex exponentials

    int _2_l = 2;
    for (int l = 1; l <= _logPoints; l++)
    {
        _W[l] = new Complex [_Points];

        for ( int i = 0; i < _Points; i++ )
        {
            double re =  cos (2. * pi * i / _2_l);
            double im = -sin (2. * pi * i / _2_l);
            _W[l][i] = Complex (re, im);
        }
        _2_l *= 2;
    }
	switch (window) {
	case hanning:
		for (int i = 0; i < _Points; i++) {
			_window[i] = .5 - .5 * cos(2. * pi * i / _Points);
		}
		break;
	case hamming:
		for (int i = 0; i < _Points; i++) {
			_window[i] = .54 - .46 * cos(2. * pi * i / _Points);
		}
		break;
	case blackman:
		for (int i = 0; i < _Points; i++) {
			_window[i] = .42 - .5 * cos(2. * pi * i / _Points) +
					.08 * cos(4. * pi * i / _Points);
		}
		break;
	case rectangle:
	default:
		for (int i = 0; i < _Points; i++) {
			_window[i] = 1.0;
		}
		break;
	}
    // set up bit reverse mapping
    int rev = 0;
    int halfPoints = _Points/2;
    for (int i = 0; i < _Points - 1; i++)
    {
        _aBitRev[i] = rev;
        int mask = halfPoints;
        // add 1 backwards
        while (rev >= mask)
        {
            rev -= mask; // turn off this bit
            mask >>= 1;
        }
        rev += mask;
    }
    _aBitRev [_Points-1] = _Points-1;
}

Fft::~Fft()
{
    delete []_aTape;
    delete []_aBitRev;
    for (int l = 1; l <= _logPoints; l++)
    {
        delete []_W[l];
    }
    delete []_W;
    delete []_X;
	delete []_window;
}

void Fft::copy(int pos, short s)
{
	_aTape[pos] = (double)s * _window[pos];
}

void Fft::transform(int* spec)
{
	int i, j;

	for (i = 0; i < _Points; i ++) {
		_X [_aBitRev[i]] = Complex(_aTape[i]);
	}
    // step = 2 ^ (level-1)
    // increm = 2 ^ level;
    int step = 1;
    for (int level = 1; level <= _logPoints; level++)
    {
        int increm = step * 2;
        for (j = 0; j < step; j++)
        {
            // U = exp ( - 2 PI j / 2 ^ level )
            Complex U = _W [level][j];
			for (i = j; i < _Points; i += increm)
            {
                // butterfly
                Complex T = U;
                T *= _X [i+step];
                _X [i+step] = _X[i];
                _X [i+step] -= T;
                _X [i] += T;
            }
        }
        step *= 2;
    }

	for (i = 0; i < _Points / 2; i ++) {
		double d = _X[i].mod() / _sqrtPoints;
		spec[i] = (int)(20 * log10(d));
	}
}
