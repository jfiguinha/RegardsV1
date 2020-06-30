#ifndef __PERLINNOISE_H__
#define __PERLINNOISE_H__

class CPerlinNoise
{

public:

	CPerlinNoise();

	~CPerlinNoise();

	inline const float Noise2d(const int &x, const int &y);

	inline const double Interpolate(const double &a,const double &b,const double &x);

	const float GetValue(float x, float y);

	float FinalNoise(float x, float y);

	float Smooth_Noise(int x, int y);


private:
	float m_fPrivx0y0,m_fPrivx1y0,m_fPrivx0y1,m_fPrivx1y1;

	inline float CalculPosValue(const int Xint,const int Yint);
};

#endif