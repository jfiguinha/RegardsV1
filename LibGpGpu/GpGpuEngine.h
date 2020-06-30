#pragma once

#ifdef GPGPU_CG

class CGpGpuEngine
{
public:
	CGpGpuEngine(void);
	~CGpGpuEngine(void);

	void InitializeEngine(const char * cgSource, const char * cgFunction);
	void InitValue(int w, int h, int colorSize, BYTE * data);
	void Compute(const int & iNbTime = 1);
	BYTE * GetOutputData();
	void DeleteGLTextures();

private:

	static void cgErrorCallback();
	


    int           _iWidth, _iHeight, _iColorSize; // The dimensions of our array
    float         _rAngle;           // used for animation
    BYTE *		  _data;
	BYTE *		  _outputdata;
    unsigned int  _iTexture;         // The texture used as a data array
	GLuint	texture[1];			// Storage For One Texture ( NEW )
    CGprogram     _fragmentProgram;  // the fragment program used to update
    CGparameter   _textureParam, _widthParam, _heightParam;     // a parameter to the fragment program


};

#endif