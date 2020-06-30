#ifndef _COMPLEX_INC_
#define _COMPLEX_INC_
//------------------------------------
//  complex.h
//  Complex number
//  (c) Reliable Software, 1996
//------------------------------------

#if !(defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))
	#include <math.h>
#else
	#include <mathimf.h>
#endif

class Complex
{
private:
    double _re;
    double _im;

public:
    Complex () {}
    Complex (double re): _re(re), _im(0.0) {}
    Complex (double re, double im): _re(re), _im(im) {}

    void operator += (const Complex& c)
    {
        _re += c._re;
        _im += c._im;
    }
    void operator -= (const Complex& c)
    {
        _re -= c._re;
        _im -= c._im;
    }
    void operator *= (const Complex& c)
    {
        double reT = c._re * _re - c._im * _im;
        _im = c._re * _im + c._im * _re;
        _re = reT;
    }
    Complex operator- () 
    {
            return Complex (- _re, _im);
    }
    double mod () const { return sqrt (_re * _re + _im * _im); }
};

#endif