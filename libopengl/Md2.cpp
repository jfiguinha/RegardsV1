// Md2.cpp: implementation of the CMd2 class.
//
//////////////////////////////////////////////////////////////////////

#include "Md2.h"
#include "vector_t.h"
#include <stdio.h>
#include <stdlib.h>
#include <gl\gl.h>								// Header File For The OpenGL32 Library
#include <gl\glu.h>								// Header File For The GLu32 Library
//#include <gl\glaux.h>	
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

vector_t operator-(vector_t a, vector_t b)
{
	vector_t c;

	c.point[0] = a.point[0] - b.point[0];
	c.point[1] = a.point[1] - b.point[1];
	c.point[2] = a.point[2] - b.point[2];

	return c;
}

vector_t operator*(float f, vector_t b)
{
	vector_t c;

	c.point[0] = f * b.point[0];
	c.point[1] = f * b.point[1];
	c.point[2] = f * b.point[2];

	return c;
}

// vector division
vector_t operator/(vector_t a, vector_t b)
{
	vector_t c;

	c.point[0] = a.point[0] / b.point[0];
	c.point[1] = a.point[1] / b.point[1];
	c.point[2] = a.point[2] / b.point[2];

	return c;
}


vector_t operator+(vector_t a, vector_t b)
{
	vector_t c;

	c.point[0] = a.point[0] + b.point[0];
	c.point[1] = a.point[1] + b.point[1];
	c.point[2] = a.point[2] + b.point[2];

	return c;
}


CMd2::CMd2()
{
	model = NULL;
	//model->triIndex = NULL;
}

CMd2::~CMd2()
{
	if(model != NULL)
	{
		if(model->triIndex != NULL)
		{
			free(model->triIndex);
			model->triIndex = NULL;
		}

		if(model->pointList != NULL)
		{
			free(model->pointList);
			model->pointList = NULL;
		}

		free(model);
	}

	model = NULL;
}

void CMd2::DisplayMD2(const float &m_fRed,
			const float & m_fGreen,
			const float & m_fBlue)
{
	vector_t *pointList;
	int i;

    // create a pointer to the frame we want to show
    pointList = &model->pointList[0/*model->numPoints*/];

	// set the texture

	// display the textured model with proper lighting normals
    glBegin(GL_TRIANGLES);
		for(i = 0; i < model->numTriangles; i++)
        {
			CalculateNormal(pointList[model->triIndex[i].meshIndex[0]].point,
                             pointList[model->triIndex[i].meshIndex[2]].point,
                             pointList[model->triIndex[i].meshIndex[1]].point);
			glColor3f(m_fRed, m_fGreen, m_fBlue);	
			glVertex3fv(pointList[model->triIndex[i].meshIndex[0]].point);

			glColor3f(m_fRed, m_fGreen, m_fBlue);
            glVertex3fv(pointList[model->triIndex[i].meshIndex[2]].point);

			glColor3f(m_fRed, m_fGreen, m_fBlue);
            glVertex3fv(pointList[model->triIndex[i].meshIndex[1]].point);
        }
    glEnd();
}

// CalculateNormal()
// desc: given 3 points, calculates the normal to the points
void CMd2::CalculateNormal( float *p1, float *p2, float *p3 )
{
   float a[3], b[3], result[3];
   float length;

   a[0] = p1[0] - p2[0];
   a[1] = p1[1] - p2[1];
   a[2] = p1[2] - p2[2];

   b[0] = p1[0] - p3[0];
   b[1] = p1[1] - p3[1];
   b[2] = p1[2] - p3[2];

   result[0] = a[1] * b[2] - b[1] * a[2];
   result[1] = b[0] * a[2] - a[0] * b[2];
   result[2] = a[0] * b[1] - b[0] * a[1];

   // calculate the length of the normal
   length = (float)sqrt(result[0]*result[0] + result[1]*result[1] + result[2]*result[2]);

   // normalize and specify the normal
   glNormal3f(result[0]/length, result[1]/length, result[2]/length);
}

int CMd2::LoadMD2Model(const char *filename,float &distance)
{
	FILE *filePtr;						// file pointer
	int fileLen;						// length of model file
    char *buffer;						// file buffer
		

	if(model != NULL)
	{
		if(model->triIndex != NULL)
		{
			free(model->triIndex);
			model->triIndex = NULL;
		}

		if(model->pointList != NULL)
		{
			free(model->pointList);
			model->pointList = NULL;
		}
		free(model);
	}

	model = NULL;

	modelHeader_t *modelHeader;			// model header

	stIndex_t *stPtr;					// texture data
    frame_t *frame;						// frame data
	vector_t *pointListPtr;				// index variable
    mesh_t *triIndex, *bufIndexPtr;		// index variables
    int i, j;							// index variables

	// open the model file
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return false;

	// find length of file
    fseek(filePtr, 0, SEEK_END);
    fileLen = ftell(filePtr);
    fseek(filePtr, 0, SEEK_SET);
	
    // read entire file into buffer
    buffer = (char*)malloc(fileLen + 1);
    fread(buffer, sizeof(char), fileLen, filePtr);

	// extract model file header from buffer
    modelHeader = (modelHeader_t*)buffer;

	// allocate memory for model data
   	model = (modelData_t*)malloc(sizeof(modelData_t));
	if (model == NULL)
		return false;

	// allocate memory for all vertices used in model, including animations
    model->pointList = (vector_t*)malloc(sizeof(vector_t)*modelHeader->numXYZ * modelHeader->numFrames);

	// store vital model data
    model->numPoints = modelHeader->numXYZ;
    model->numFrames = modelHeader->numFrames;
	model->frameSize = modelHeader->framesize;

    // loop number of frames in model file
    for(j = 0; j < modelHeader->numFrames; j++)
    {
       // offset to the points in this frame
       frame = (frame_t*)&buffer[modelHeader->offsetFrames + modelHeader->framesize * j];

	   // calculate the point positions based on frame details
       pointListPtr = (vector_t*)&model->pointList[modelHeader->numXYZ * j];
       for(i = 0; i < modelHeader->numXYZ; i++)
       {
          pointListPtr[i].point[0] = frame->scale[0] * frame->fp[i].v[0] + frame->translate[0];
          pointListPtr[i].point[1] = frame->scale[1] * frame->fp[i].v[1] + frame->translate[1];
          pointListPtr[i].point[2] = frame->scale[2] * frame->fp[i].v[2] + frame->translate[2];
       }
    }
			 

	// allocate an index of triangles
	triIndex = (mesh_t*)malloc(sizeof(mesh_t) * modelHeader->numTris);

	// set total number of triangles
	model->numTriangles = modelHeader->numTris;
	model->triIndex = triIndex;
	
	// point to triangle indexes in buffer
	bufIndexPtr = (mesh_t*)&buffer[modelHeader->offsetTris];

	// create a mesh (triangle) list
	for (j = 0; j < model->numFrames; j++)		
	{
		// for all triangles in each frame
		for(i = 0; i < modelHeader->numTris; i++)
		{
		   triIndex[i].meshIndex[0] = bufIndexPtr[i].meshIndex[0];
		   triIndex[i].meshIndex[1] = bufIndexPtr[i].meshIndex[1];
		   triIndex[i].meshIndex[2] = bufIndexPtr[i].meshIndex[2];
		   triIndex[i].stIndex[0] = bufIndexPtr[i].stIndex[0];
		   triIndex[i].stIndex[1] = bufIndexPtr[i].stIndex[1];
		   triIndex[i].stIndex[2] = bufIndexPtr[i].stIndex[2];
		}
	}

	// close file and free memory
	fclose(filePtr);
    free(buffer);

	model->currentFrame = 0;
	model->nextFrame = 1;
	model->interpol = 0.0;

	distance = 64.0f;

	return true;
}

