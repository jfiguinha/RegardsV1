#pragma once

class GpGpuEngineGL
{
public:
	GpGpuEngineGL(void);
	~GpGpuEngineGL(void);

	int InitializeEngine(const char * cgSource, float * tabVecs, int kSize, float kFactor, float kOffset);
	void InitValue(int w, int h, int colorSize, BYTE * data);
	void Compute(const int & iNbTime = 1);
	BYTE * GetOutputData();
	void DeleteGLTextures();

private:

	void CreateTexture(BYTE * data);

    int           _iWidth, _iHeight, _iColorSize; // The dimensions of our array
    float         _rAngle;           // used for animation
    BYTE *		  _data;
	BYTE *		  _outputdata;
    unsigned int  _iTexture;         // The texture used as a data array
    GLhandleARB   _programObject;    // the program used to update
    GLhandleARB   _fragmentShader;
	float *		  _tabVecs;
	int			_ikSize;
	float _fkFactor;
	float _fkOffset;
    GLint         _texUnit, _vecKernel, _Koffset, _Kfactor, _Ksize;          // a parameter to the fragment program          // a parameter to the fragment program

};
