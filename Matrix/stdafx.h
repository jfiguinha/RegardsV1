#define WIN32_LEAN_AND_MEAN		// Exclure les en-têtes Windows rarement utilisés

#include <Windows.h>
#include <math.h>

#define MagickSQ2PI 2.50662827463100024161235523934010416269302368164062


#ifndef PI
#define PI 3.141592653589793238
#define PI2 2*3.141592653589793238
#endif

#ifndef TINY
#define TINY 1e-20
#endif

inline void SWAP(double &data1, double &data2)
{
	double dataTemp = data1;
	data1 = data2;
	data2 = dataTemp;
}

inline void SWAP(int &data1, int &data2)
{
	int dataTemp = data1;
	data1 = data2;
	data2 = dataTemp;
}