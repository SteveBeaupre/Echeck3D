#include "3DShadowCaster.h"

GLuint Abs(int i)
{
	if(i < 0)
		i *= -1;
	return i;
}

void C3DShadowCaster::VMatMult(GLmatrix16f M, GLvector4f v)
{
	GLfloat res[4];										// Hold Calculated Results
	res[0]=M[ 0]*v[0]+M[ 4]*v[1]+M[ 8]*v[2]+M[12]*v[3];
	res[1]=M[ 1]*v[0]+M[ 5]*v[1]+M[ 9]*v[2]+M[13]*v[3];
	res[2]=M[ 2]*v[0]+M[ 6]*v[1]+M[10]*v[2]+M[14]*v[3];
	res[3]=M[ 3]*v[0]+M[ 7]*v[1]+M[11]*v[2]+M[15]*v[3];
	v[0]=res[0];										// Results Are Stored Back In v[]
	v[1]=res[1];
	v[2]=res[2];
	v[3]=res[3];										// Homogenous Coordinate
}

// load object
/*int C3DShadowCaster::LoadFromFile(char *st, glObject *o)
{
	FILE *file;
	
	file = fopen(st, "rb");
	if (!file){
		char c[128];
		ZeroMemory(&c[0], 128);
		sprintf(&c[0], "Error: Can't open the file \"%s\".", st);
		return FALSE;
	}
	
	o->IsBuffersAllocated = false;
	o->pPoints = NULL;
	o->pPlane = NULL;
	o->pVertsArray = NULL;
	o->VertsArrayID = 0;

	//points
	fread(&o->nPoints, 1, sizeof(UINT), file);
	
	o->pPoints = new sPoint[o->nPoints + 1];
	ZeroMemory(o->pPoints, (o->nPoints + 1) * sizeof(sPoint));
	for(UINT i = 1; i <= o->nPoints; i++){
		fread(&o->pPoints[i], 1, sizeof(sPoint), file);
	}

	//planes
	fread(&o->nPlanes, 1, sizeof(o->nPlanes), file);
	
	o->pPlane = new sPlane[o->nPlanes];
	ZeroMemory(&o->pPlane[0], o->nPlanes * sizeof(sPlane));
	for(UINT i = 0; i < o->nPlanes; i++){

		fread(&o->pPlane[i].p[0],      1, sizeof(UINT)  * 3, file);
		fread(&o->pPlane[i].neigh[0],  1, sizeof(UINT)  * 3, file);
		fread(&o->pPlane[i].PlaneEq.a, 1, sizeof(float) * 4, file);
		
		int Vis;
		fread(&Vis, 1, sizeof(int), file);
		o->pPlane[i].visible = Vis == 1;
	}

	fclose(file);

	AllocateVertexArray(o);

	return TRUE;
}*/

// load object
bool C3DShadowCaster::Load(char *st, glObject *o)
{
	return LoadSet(st, o, 1);
}

// load object
bool C3DShadowCaster::LoadSet(char *st, glObject *o, UINT MaxModels)
{
	LPCSTR Sig = "Shadow Models 3D";
	LPCSTR Ver = "Version #: v1.00";

	glObject *pObj = o;

	if(MaxModels == 0)
		MaxModels = 0xFFFFFFFF;

	// Open the file
	FILE *file = fopen(st, "rb");
	if (!file){
		char c[128];
		ZeroMemory(&c[0], 128);
		sprintf(&c[0], "Error: Can't open the file \"%s\".", st);
		MessageBox(0, c, "Error", 0);
		return false;
	}

	// Check the file signature
	BYTE SigBuf[16];
	ZeroMemory(&SigBuf[0], 16);
	fread(&SigBuf[0], 1, 16, file);
	if(memcmp(&SigBuf[0], Sig, 16) != 0){
		char c[128];
		ZeroMemory(&c[0], 128);
		sprintf(&c[0], "Error: File Signature not found.", st);
		MessageBox(0, c, "Error", 0);
		return false;
	}

	// Check the file version
	BYTE VerBuf[16];
	ZeroMemory(&VerBuf[0], 16);
	fread(&VerBuf[0], 1, 16, file);
	if(memcmp(&VerBuf[0], Ver, 16) != 0){
		char c[128];
		ZeroMemory(&c[0], 128);
		sprintf(&c[0], "Error: Invalid file version.", st);
		MessageBox(0, c, "Error", 0);
		return false;
	}

	// Read the number of objects in the file
	UINT ObjCount = 0;
	fread(&ObjCount, 1, sizeof(UINT), file);
	
	// Save the number of models loaded so far...
	UINT NumModelsLoaded = 0;

	do 
	{
		// Dont care about the name but we need to read it
		{
			const int MaxNameLen = 64;
			BYTE *pBuf = new BYTE[MaxNameLen + 4];
			ZeroMemory(pBuf, MaxNameLen + 4);

			for(int Cpt = 0; Cpt < 64; Cpt += 4){
				fread(&pBuf[Cpt], 1, 4, file);
				if(pBuf[Cpt] == 0 || pBuf[Cpt+1] == 0 || pBuf[Cpt+2] == 0 || pBuf[Cpt+3] == 0)
					break;
			}
			SAFE_DELETE_ARRAY(pBuf);
		}

		// Initialize buffers
		pObj->IsBuffersAllocated = false;
		pObj->pVerts = NULL;
		pObj->pFaces = NULL;
		pObj->pVertsArray = NULL;
		pObj->VertsArrayID = 0;

		// Read the number of vertices and faces
		fread(&pObj->NumVerts, 1, sizeof(UINT), file);
		fread(&pObj->NumFaces, 1, sizeof(UINT), file);
	
		// Allocate our buffers
		pObj->pVerts = new sPoint[pObj->NumVerts];
		pObj->pFaces  = new sPlane[pObj->NumFaces];

		// Clear them
		ZeroMemory(pObj->pVerts, (pObj->NumVerts) * sizeof(sPoint));
		ZeroMemory(&pObj->pFaces[0], pObj->NumFaces * sizeof(sPlane));

		// Read vertices
		for(UINT i = 0; i < pObj->NumVerts; i++)
			fread(&pObj->pVerts[i], 1, sizeof(sPoint), file);

		// Read face data
		for(UINT i = 0; i < pObj->NumFaces; i++){
			fread(&pObj->pFaces[i].p[0],      1, sizeof(UINT)  * 3, file);
			fread(&pObj->pFaces[i].neigh[0],  1, sizeof(int)   * 3, file);
			fread(&pObj->pFaces[i].PlaneEq.a, 1, sizeof(float) * 4, file);
			pObj->pFaces[i].visible = false;
		}

		// Allocate VA Buffer
		AllocateVertexArray(pObj);

		//
		NumModelsLoaded++;
		pObj++;

		if(NumModelsLoaded >= MaxModels)
			break;
	} 
	while(NumModelsLoaded <= ObjCount);

	// Close the file
	fclose(file);

	return true;
}

void C3DShadowCaster::CastShadows(glObject *o, float *lp, bool BuildVA/* = false*/)
{
	if(!o->IsBuffersAllocated)
		return;

	if(BuildVA)
		BuildVertexArray(o, lp);

	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);

	glColorMask(0, 0, 0, 0);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xff);

	#ifdef USE_VBO_FOR_SHADOW
	bool IsVBOSupported = OnCreateVBO(o);
	#else 
	bool IsVBOSupported = OnCreateVBO(NULL);
	#endif

	glEnableClientState(GL_VERTEX_ARRAY);

	// Pass #1
	glFrontFace(GL_CCW);  
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	if(IsVBOSupported){
		OnBindVBO(o);
		glVertexPointer(3, GL_FLOAT, 0, 0);
	} else {
		glVertexPointer(3, GL_FLOAT, 0, o->pVertsArray);
	}
	glDrawArrays(GL_QUADS, 0, o->NumVertsVisible * 3);
	
	// Pass #2
	glFrontFace(GL_CW); 
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	if(IsVBOSupported){
		OnBindVBO(o);
		glVertexPointer(3, GL_FLOAT, 0, 0);
	} else {
		glVertexPointer(3, GL_FLOAT, 0, o->pVertsArray);
	}
	glDrawArrays(GL_QUADS, 0, o->NumVertsVisible * 3);
	
	glDisableClientState(GL_VERTEX_ARRAY);

	#ifdef USE_VBO_FOR_SHADOW
	OnDeleteVBO(o);
	#endif

	glDisable(GL_STENCIL_TEST);
	glFrontFace(GL_CCW);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glColorMask(1, 1, 1, 1);
}

void C3DShadowCaster::BuildVertexArray(glObject *o, float *lp)
{
	if(!o->IsBuffersAllocated)
		return;

	int CurrentPass = 0;
	unsigned int i, j, k, jj;
	unsigned int p1, p2;
	sPoint v1, v2;
	float  side;

	//set visual parameter
	sPlaneEq *pPlaneEq = NULL;
	for(i = 0; i < o->NumFaces; i++){
		pPlaneEq = &o->pFaces[i].PlaneEq;
		// chech to see if light is in front or behind the plane (face plane)
		side = pPlaneEq->a*lp[0] + pPlaneEq->b*lp[1] + pPlaneEq->c*lp[2] + pPlaneEq->d;
		o->pFaces[i].visible = side > 0;
	}
	
	float *pBuffer = &o->pVertsArray[0];
	o->NumVertsVisible = 0;

	for(i = 0; i < o->NumFaces; ++i){
		if(o->pFaces[i].visible){
			for(j = 0; j < 3; ++j){
				k = o->pFaces[i].neigh[j];
				if((!k) || (!o->pFaces[k-1].visible)){
					// here we have an edge, we must draw a polygon
					p1 = o->pFaces[i].p[j];
					//jj = (j+1)%3;
					jj = (j+1);
					if(jj == 3){jj = 0;}
					p2 = o->pFaces[i].p[jj];

					//calculate the length of the vector
					v1.x = (o->pVerts[p1].x - lp[0]) * SHADOW_INFINITE;
					v1.y = (o->pVerts[p1].y - lp[1]) * SHADOW_INFINITE;
					v1.z = (o->pVerts[p1].z - lp[2]) * SHADOW_INFINITE;

					v2.x = (o->pVerts[p2].x - lp[0]) * SHADOW_INFINITE;
					v2.y = (o->pVerts[p2].y - lp[1]) * SHADOW_INFINITE;
					v2.z = (o->pVerts[p2].z - lp[2]) * SHADOW_INFINITE;
					
					pBuffer[0]  = o->pVerts[p1].x;
					pBuffer[1]  = o->pVerts[p1].y;
					pBuffer[2]  = o->pVerts[p1].z;
					pBuffer[3]  = o->pVerts[p1].x + v1.x;
					pBuffer[4]  = o->pVerts[p1].y + v1.y;
					pBuffer[5]  = o->pVerts[p1].z + v1.z;
					pBuffer[6]  = o->pVerts[p2].x + v2.x;
					pBuffer[7]  = o->pVerts[p2].y + v2.y;
					pBuffer[8]  = o->pVerts[p2].z + v2.z;
					pBuffer[9]  = o->pVerts[p2].x;
					pBuffer[10] = o->pVerts[p2].y;
					pBuffer[11] = o->pVerts[p2].z;
					pBuffer += 12;

					o->NumVertsVisible += 4;			
					/*pBuffer = FillVA(pBuffer, o->pPoints[p1].x, o->pPoints[p1].y, o->pPoints[p1].z);
					pBuffer = FillVA(pBuffer, o->pPoints[p1].x + v1.x, o->pPoints[p1].y + v1.y, o->pPoints[p1].z + v1.z);
					pBuffer = FillVA(pBuffer, o->pPoints[p2].x, o->pPoints[p2].y, o->pPoints[p2].z);

					pBuffer = FillVA(pBuffer, o->pPoints[p2].x + v2.x, o->pPoints[p2].y + v2.y, o->pPoints[p2].z + v2.z);
					pBuffer = FillVA(pBuffer, o->pPoints[p2].x, o->pPoints[p2].y, o->pPoints[p2].z);
					pBuffer = FillVA(pBuffer, o->pPoints[p1].x + v1.x, o->pPoints[p1].y + v1.y, o->pPoints[p1].z + v1.z);
					
					o->NumVertsVisible += 6;*/
				}
			}
		}
	}

	int Indx = o->NumVertsVisible * 3;
	ZeroMemory(&o->pVertsArray[Indx], (o->VertsBufSize - Indx) * sizeof(float));
}

void C3DShadowCaster::AllocateVertexArray(glObject *o)
{
	if(o->IsBuffersAllocated)
		return;

	o->pVertsArray = NULL;
	o->VertsArrayID = 0;

	o->VertsBufSize = o->NumFaces * 12 * 3;
	o->pVertsArray = new float[o->VertsBufSize];
	ZeroMemory(&o->pVertsArray[0], o->VertsBufSize * sizeof(float));

	o->IsBuffersAllocated = true;
}

void C3DShadowCaster::FreeObject(glObject *o)
{
	#ifdef USE_VBO_FOR_SHADOW
	OnDeleteVBO(o);
	#endif
	if(o->IsBuffersAllocated){
		SAFE_DELETE_ARRAY(o->pVerts);
		SAFE_DELETE_ARRAY(o->pFaces);
		SAFE_DELETE_ARRAY(o->pVertsArray);
	}
}

