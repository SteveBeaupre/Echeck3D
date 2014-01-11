#ifndef _3D_SHADOWCASTER_H_
#define _3D_SHADOWCASTER_H_
#ifdef __cplusplus

#include <Windows.h>
#include <stdio.h>
#include <math.h>

#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glext.h>		// Header File For The GLext Library

#include <SafeKill.h>

typedef float GLvector4f[4];
typedef float GLmatrix16f[16];

#define SHADOW_INFINITE    50000.0f

#define USE_VBO_FOR_SHADOW

// vertex in 3d-coordinate system
struct sPoint{
	float x, y, z;
};

// plane equation
struct sPlaneEq{
	float a, b, c, d;
};

// structure describing an object's face
struct sPlane{
	UINT p[3];
	int  neigh[3];
	sPlaneEq PlaneEq;
	bool visible;
};

// object structure
struct glObject {
	bool IsBuffersAllocated;
	GLuint NumFaces, NumVerts;
	UINT VertsBufSize;
	UINT NumVertsVisible;
	UINT VertsArrayID;

	sPoint *pVerts;
	sPlane *pFaces;
	float  *pVertsArray;
};

class C3DShadowCaster {
public:
	bool Load(char *st, glObject *o);
	bool LoadSet(char *st, glObject *o, UINT MaxModels);
	void BuildVertexArray(glObject *o, float *lp);
public:
	void VMatMult(GLmatrix16f M, GLvector4f v);
	void AllocateVertexArray(glObject *o);
	void FreeObject(glObject *o);
	void CastShadows(glObject *o, float *lp, bool BuildVA = false);
public:
	virtual bool OnCreateVBO(glObject *o){return false;}
	virtual void OnBindVBO(glObject *o){}
	virtual void OnDeleteVBO(glObject *o){}
};


#endif
#endif //--_3D_SHADOWCASTER_H_