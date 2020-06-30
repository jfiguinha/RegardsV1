// Md2.h: interface for the CMd2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MD2_H__E916B62F_EF9F_492F_9999_08FCC3AF3940__INCLUDED_)
#define AFX_MD2_H__E916B62F_EF9F_492F_9999_08FCC3AF3940__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include <windows.h>

#ifndef __MD2_H
#define __MD2_H

#define MAX_FRAMES 512

enum texTypes_t
{
	PCX,
	BMP,
	TGA
};

typedef struct 
{
	// C Compatible version: enum texTypes textureType;
	// We use C++ only version
	texTypes_t textureType;

	int width;					// width of texture
	int height;					// height of texture
	long int scaledWidth;
	long int scaledHeight;

	unsigned int texID;			// the texture object id of this texture
	unsigned char *data;		// the texture data
	unsigned char *palette;
} texture_t;

// a single vertex
typedef struct
{
   float point[3];
} vector_t;


// texture coordinate
typedef struct
{
   float s;
   float t;
} texCoord_t;

// texture coordinate index
typedef struct
{
   short s;
   short t;
} stIndex_t;

// info for a single frame point
typedef struct
{
   unsigned char v[3];
   unsigned char normalIndex;	// not used
} framePoint_t;

// information for a single frame
typedef struct
{
   float scale[3];
   float translate[3];
   char name[16];
   framePoint_t fp[1];
} frame_t;

// data for a single triangle
typedef struct
{
   unsigned short meshIndex[3];		// vertex indices
   unsigned short stIndex[3];		// texture coordinate indices
} mesh_t;

// the model data
typedef struct
{
	int numFrames;				// number of frames
	int numPoints;				// number of points
	int numTriangles;			// number of triangles
    int numST;					// number of skins
	int frameSize;				// size of each frame in bytes
	int texWidth, texHeight;	// texture width, height
	int currentFrame;			// current frame # in animation
	int nextFrame;				// next frame # in animation
	float interpol;				// percent through current frame
	mesh_t *triIndex;			// triangle list
	texCoord_t *st;				// texture coordinate list
	vector_t *pointList;		// vertex list
	texture_t *modelTex;		// texture data
} modelData_t;

typedef struct
{
   int ident;		// identifies as MD2 file "IDP2"
   int version;		// mine is 8
   int skinwidth;    // width of texture
   int skinheight;   // height of texture
   int framesize;    // number of bytes per frame
   int numSkins;     // number of textures
   int numXYZ;       // number of points
   int numST;        // number of texture
   int numTris;      // number of triangles
   int numGLcmds;
   int numFrames;    // total number of frames
   int offsetSkins;  // offset to skin names (64 bytes each)
   int offsetST;     // offset of texture s-t values
   int offsetTris;   // offset of triangle mesh
   int offsetFrames; // offset of frame data (points)
   int offsetGLcmds; // type of OpenGL commands to use
   int offsetEnd;    // end of file
} modelHeader_t;

#endif

class CMd2  
{
public:

	int LoadMD2Model(const char * filename,float &distance);
	void DisplayMD2(const float &m_fRed,
			const float & m_fGreen,
			const float & m_fBlue);
	CMd2();
	virtual ~CMd2();

private:
	modelData_t * model;
	void CalculateNormal( float *p1, float *p2, float *p3 );
};

#endif // !defined(AFX_MD2_H__E916B62F_EF9F_492F_9999_08FCC3AF3940__INCLUDED_)
