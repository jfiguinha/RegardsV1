#ifndef __CPERLIN_H__
#define __CPERLIN_H__

#define B 0x100
#define BM 0xff
#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

class CPerlin
{
	public:

	void init(void);
	double noise1(double);
	double noise2(double *);
	double noise3(double *);
	void normalize3(double *);
	void normalize2(double *);
	
	double PerlinNoise1D(double,double,double,int);
	double PerlinNoise2D(double,double,double,double,int);
	double PerlinNoise3D(double,double,double,double,double,int);

	static int p[B + B + 2];
	static double g3[B + B + 2][3];
	static double g2[B + B + 2][2];
	static double g1[B + B + 2];
	static int start;	
};

#endif