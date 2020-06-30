#include ".\c3ds.h"
#include <windows.h> 
#include <gl\gl.h>								// Header File For The OpenGL32 Library
#include <gl\glu.h>								// Header File For The GLu32 Library
//#include <gl\glaux.h>	

C3ds::C3ds(void)
{
	model = NULL;
}

C3ds::~C3ds(void)
{
	if(model != NULL)
		Free3dsData(model);

	free(model);

	model = NULL;
}

bool C3ds::Load3dsFile(const char *mFile,float &distance)
{
   //
   //load the model file
   //

	char tFile[MAX_TEXTURES][256] = { "", "", "", "", "", "", "", ""};

	if(model != NULL)
	{
		Free3dsData(model);
		free(model);
		model = NULL;
	}

	model = (data3ds_t * )malloc(sizeof(data3ds_t));
	
	ZeroMemory(model,sizeof(data3ds_t));

	Read3dsFile( (char *)mFile, model);
	Calculate3dsNormals( model);
	Calculate3dsTangentSpace( model);
	Calculate3dsBoundingBox( model);

	if (model->triCount == 0)
	{
		MessageBox(NULL,"Error: Model file '%s' is either missing or lacks triangles\n", mFile,0);
		return FALSE;
	}

	float x,y,z;

	x = model->max[0] - model->min[0];
	y = model->max[1] - model->min[1];
	z = model->max[2] - model->min[2];
	
	distance = sqrtf(x*x + y*y + z*z);


   return TRUE;
}

int C3ds::DrawObject(const float &m_fRed,
			const float & m_fGreen,
			const float & m_fBlue)
{

   //
   //Drawn in immediate mode for convenience, for performance, use vertex arrays with VAO
   //

   glBegin( GL_TRIANGLES);
   int vert;
   for (int ii=0; ii< model->meshCount; ii++)
   {
      for (int gg=0; gg< model->meshes[ii].triCount; gg++)
      {
         vert = model->meshes[ii].tris[gg][0];
		 /*
         if (m_stOpenGLData->model.meshes[ii].texCoords)
         {
            glMultiTexCoord2fvARB( GL_TEXTURE0_ARB, m_stOpenGLData->model.meshes[ii].texCoords[vert]);
            glMultiTexCoord3fvARB( GL_TEXTURE1_ARB, &(m_stOpenGLData->model.meshes[ii].tangentSpace[gg*3][0]));
            glMultiTexCoord3fvARB( GL_TEXTURE2_ARB, &(m_stOpenGLData->model.meshes[ii].tangentSpace[gg*3][3]));
         }*/

		 glColor3f(m_fRed, m_fGreen, m_fBlue);	
         glNormal3fv(model->meshes[ii].norms[gg*3]);
		 glColor3f(m_fRed, m_fGreen, m_fBlue);	
         glVertex3fv( model->meshes[ii].verts[vert]);
         vert = model->meshes[ii].tris[gg][1];
		 /*
         if (m_stOpenGLData->model.meshes[ii].texCoords)
         {
            glMultiTexCoord2fvARB( GL_TEXTURE0_ARB, m_stOpenGLData->model.meshes[ii].texCoords[vert]);
            glMultiTexCoord3fvARB( GL_TEXTURE1_ARB, &(m_stOpenGLData->model.meshes[ii].tangentSpace[gg*3+1][0]));
            glMultiTexCoord3fvARB( GL_TEXTURE2_ARB, &(m_stOpenGLData->model.meshes[ii].tangentSpace[gg*3+1][3]));
         }*/

		 glColor3f(m_fRed, m_fGreen, m_fBlue);	
         glNormal3fv( model->meshes[ii].norms[gg*3+1]);
		 glColor3f(m_fRed, m_fGreen, m_fBlue);	
         glVertex3fv( model->meshes[ii].verts[vert]);
         vert = model->meshes[ii].tris[gg][2];
		 /*
         if (m_stOpenGLData->model.meshes[ii].texCoords)
         {
            glMultiTexCoord2fvARB( GL_TEXTURE0_ARB, m_stOpenGLData->model.meshes[ii].texCoords[vert]);
            glMultiTexCoord3fvARB( GL_TEXTURE1_ARB, &(m_stOpenGLData->model.meshes[ii].tangentSpace[gg*3+2][0]));
            glMultiTexCoord3fvARB( GL_TEXTURE2_ARB, &(m_stOpenGLData->model.meshes[ii].tangentSpace[gg*3+2][3]));
         }*/
		 glColor3f(m_fRed, m_fGreen, m_fBlue);	
         glNormal3fv( model->meshes[ii].norms[gg*3+2]);
		 glColor3f(m_fRed, m_fGreen, m_fBlue);	
         glVertex3fv( model->meshes[ii].verts[vert]);
      }
       
   }
   glEnd();

   return 0;
}