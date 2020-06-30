#include "stdafx.h"
#include "perlinnoise.h"
#include "floattoint.h"

CPerlinNoise::CPerlinNoise()
{
	int Xint = 0;
	int Yint = 0;
	//Calcul Préemptif
	m_fPrivx0y0 = Noise2d(Xint, Yint);
	m_fPrivx1y0 = Noise2d(Xint+1, Yint);
	m_fPrivx0y1 = Noise2d(Xint, Yint+1);
	m_fPrivx1y1 = Noise2d(Xint+1, Yint+1);
}

CPerlinNoise::~CPerlinNoise()
{
}

inline const float CPerlinNoise::Noise2d(const int &x, const int &y)
{
	int n = ((x + (y << 6)) << 13) ^ (x + (y << 6));
	return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

inline const double CPerlinNoise::Interpolate(const double &a,const double &b,const double &x)
{
	return (a + (x * (b - a)));
}

inline float CPerlinNoise::CalculPosValue(const int Xint,const int Yint)
{
	int m = Xint + ((Yint) << 6);
	int n = (m << 13) ^ (m);
	return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f); 

}

const float CPerlinNoise::GetValue(float x, float y) 
{
		int Xint = FloatToInt(x);
		int Yint = FloatToInt(y);

		float Xfrac = x - Xint;
		float Yfrac = y - Yint;

		float x0y0,x1y0,x0y1,x1y1;

		if(Xint != 0 || Yint != 0)
		{		
			#pragma omp parallel
			{
				x0y0 = CalculPosValue(Xint,Yint);
				x1y0 = CalculPosValue(Xint+1,Yint);
				x0y1 = CalculPosValue(Xint,Yint+1);
				x1y1 = CalculPosValue(Xint+1,Yint+1);
			}

			/*
			int m = Xint + (Yint << 6);
			int n = (m << 13) ^ (m);
			x0y0 = (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);

			m = Xint + 1 + ((Yint) << 6);
			n = (m << 13) ^ (m);
			x1y0 = (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f); 

			m = Xint + ((Yint+1) << 6);
			n = (m << 13) ^ (m);
			x0y1 = (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f); 

			m = Xint + 1 + ((Yint+1) << 6);
			n = (m << 13) ^ (m);
			x1y1 = (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
			*/
		}
		else
		{
			x0y0 = m_fPrivx0y0;  //find the noise values of the four corners
			x1y0 = m_fPrivx1y0;
			x0y1 = m_fPrivx0y1;
			x1y1 = m_fPrivx1y1;
		}

		//interpolate between those values according to the x and y fractions
		float v1 = (x0y0 + (Xfrac * (x1y0 - x0y0)));//Interpolate(x0y0, x1y0, Xfrac); //interpolate in x direction (y)
		float v2 = (x0y1 + (Xfrac * (x1y1 - x0y1)));//Interpolate(x0y1, x1y1, Xfrac); //interpolate in x direction (y+1)
		float fin = (v1 + (Yfrac * (v2 - v1)));//Interpolate(v1, v2, Yfrac);  //interpolate in y direction

		return fin;
}

float CPerlinNoise::FinalNoise(float x, float y) {
		//properties of the perlin noise
		float P = 0.5f;
		int octaves = 4;

		//properties of one octave (changing each loop)
		float total = 0.0f;
		int freq = 1;
		float pers = 1;

		for(int i=0; i<octaves; i++) 
		{
			total += GetValue(x*freq,y*freq) * pers;
			freq *= 2;
			pers *= P;
		}
		return total;
}

float CPerlinNoise::Smooth_Noise(int x, int y) 
{
		float corners = ( Noise2d(x-1, y-1) + Noise2d(x+1, y-1) + Noise2d(x-1, y+1) + Noise2d(x+1, y+1) ) / 16.0f;
		float sides = ( Noise2d(x-1, y) +Noise2d(x+1, y) + Noise2d(x, y-1) + Noise2d(x, y+1) ) / 8.0f;
		float center = Noise2d(x, y) / 4.0f;
		return corners + sides + center;
}
