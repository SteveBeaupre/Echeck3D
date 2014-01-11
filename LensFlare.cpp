#include "LensFlare.h"

CLensFlare::CLensFlare()
{
	//
	FadeInTimeLeft  = 0.0f;
	FadeOutTimeLeft = 0.0f;
	InitializeFlareObj();
}


CLensFlare::~CLensFlare(){}


void CLensFlare::InitializeFlareObj()
{
	FlareStruct TmpFlare;
	//Do the other flares
	TmpFlare.Initialize(BigGlow, 0.05f, 25.0f, 1.0f, 1.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(Hexa,    0.07f, 10.0f, 0.0f, 1.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(Halo,    0.10f, 20.0f, 1.0f, 1.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(Hexa,    0.15f, 8.00f, 0.0f, 0.0f, 1.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(Halo,    0.16f, 15.0f, 1.0f, 0.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(BigGlow, 0.20f, 10.0f, 1.0f, 1.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(Halo,    0.24f, 25.0f, 0.0f, 1.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(BigGlow, 0.25f, 15.0f, 1.0f, 0.5f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(BigGlow, 0.29f, 15.0f, 1.0f, 0.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(BigGlow, 0.30f, 25.0f, 0.0f, 0.5f, 1.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(Hexa,    0.35f, 14.0f, 1.0f, 0.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(Halo,    0.36f, 30.0f, 0.0f, 1.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(BigGlow, 0.37f, 20.0f, 0.0f, 0.5f, 1.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(Halo,    0.42f, 25.0f, 0.0f, 1.0f, 1.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(BigGlow, 0.45f, 8.00f, 1.0f, 0.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(Halo,    0.49f, 30.0f, 0.0f, 1.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(BigGlow, 0.54f, 10.0f, 1.0f, 1.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(Halo,    0.60f, 14.0f, 0.0f, 0.0f, 1.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(BigGlow, 0.62f, 13.0f, 0.0f, 1.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(Hexa,    0.65f, 24.0f, 1.0f, 0.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(BigGlow, 0.66f, 17.0f, 0.0f, 1.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(Halo,    0.70f, 28.0f, 0.0f, 0.0f, 1.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(Halo,    0.75f, 20.0f, 1.0f, 1.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(BigGlow, 0.78f, 27.0f, 0.0f, 1.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(Halo,    0.82f, 35.0f, 0.0f, 0.0f, 1.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(BigGlow, 0.83f, 22.0f, 1.0f, 1.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(Hexa,    0.84f, 18.0f, 0.0f, 1.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(Halo,    0.89f, 13.0f, 0.0f, 0.0f, 1.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(Halo,    0.94f, 20.0f, 0.0f, 1.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(Hexa,    0.98f, 10.0f, 1.0f, 1.0f, 0.0f, 0.5f); Flares.push_back(TmpFlare);

	/*
	//Make the sun  (Initialize = Type, pos, Size, color)
	TmpFlare.Initialize(Streak,      0.0f, 250.0f, 1.0f, 1.0f, 1.0f, 0.3f);  Flares.push_back(TmpFlare);
	TmpFlare.Initialize(BigGlow,     0.0f, 250.0f, 1.0f, 1.0f, 1.0f, 0.4f);  Flares.push_back(TmpFlare);
	TmpFlare.Initialize(SunGlow,     0.0f, 100.0f, 1.0f, 1.0f, 1.0f, 0.25f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(RainBowHalo, 0.0f, 200.0f, 1.0f, 1.0f, 1.0f, 0.1f);  Flares.push_back(TmpFlare);
	*/

	//Make the sun  (Initialize = Type, pos, Size, color)
	TmpFlare.Initialize(Streak,      0.0f, 250.0f, 1.0f, 1.0f, 1.0f, 0.5f);  Flares.push_back(TmpFlare);
	TmpFlare.Initialize(BigGlow,     0.0f, 250.0f, 1.0f, 1.0f, 1.0f, 0.6f);  Flares.push_back(TmpFlare);
	TmpFlare.Initialize(SunGlow,     0.0f, 100.0f, 1.0f, 1.0f, 1.0f, 0.25f); Flares.push_back(TmpFlare);
	TmpFlare.Initialize(RainBowHalo, 0.0f, 200.0f, 1.0f, 1.0f, 1.0f, 0.15f); Flares.push_back(TmpFlare);
}


void CLensFlare::RandomizeFunc()
{
	//Init the seed of Rand
	//srand((unsigned)time(NULL));

	for(UINT Cpt = 0; Cpt < Flares.size()-4; Cpt++){

		UINT  RandValue;

		RandValue = rand() % 2;
		switch(RandValue)
		{
		case 0: Flares[Cpt].Color.r = 0.0f; break;
		case 1: Flares[Cpt].Color.r = 0.5f; break;
		case 2: Flares[Cpt].Color.r = 1.0f; break;
		}
		RandValue = rand() % 2;
		switch(RandValue)
		{
		case 0: Flares[Cpt].Color.g = 0.0f; break;
		case 1: Flares[Cpt].Color.g = 0.5f; break;
		case 2: Flares[Cpt].Color.g = 1.0f; break;
		}
		RandValue = rand() % 2;
		switch(RandValue)
		{
		case 0: Flares[Cpt].Color.g = 0.0f; break;
		case 1: Flares[Cpt].Color.g = 0.5f; break;
		case 2: Flares[Cpt].Color.g = 1.0f; break;
		}
	}
}

void CLensFlare::GetSunProjposition(float *x, float *y)
{
	*x = ProjSunPos.x;
	*y = ProjSunPos.y;
}

void CLensFlare::UpdateFrustum()
{
    GLfloat   clip[16];
	GLfloat   proj[16];
    GLfloat   modl[16];
    GLfloat   t;

    /* Get the current PROJECTION matrix from OpenGL */
    glGetFloatv(GL_PROJECTION_MATRIX, proj);

    /* Get the current MODELVIEW matrix from OpenGL */
    glGetFloatv(GL_MODELVIEW_MATRIX, modl);

    /* Combine the two matrices (multiply projection by modelview) */
    clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
    clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
    clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
    clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

    clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
    clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
    clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
    clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

    clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
    clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
    clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
    clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

    clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
    clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
    clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
    clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

    /* Extract the numbers for the RIGHT plane */
    m_Frustum[0][0] = clip[ 3] - clip[ 0];
    m_Frustum[0][1] = clip[ 7] - clip[ 4];
    m_Frustum[0][2] = clip[11] - clip[ 8];
    m_Frustum[0][3] = clip[15] - clip[12];

    /* Normalize the result */
    t = GLfloat(sqrt(m_Frustum[0][0] * m_Frustum[0][0] + m_Frustum[0][1] * m_Frustum[0][1] + m_Frustum[0][2] * m_Frustum[0][2]));
    m_Frustum[0][0] /= t;
    m_Frustum[0][1] /= t;
    m_Frustum[0][2] /= t;
    m_Frustum[0][3] /= t;

    /* Extract the numbers for the LEFT plane */
    m_Frustum[1][0] = clip[ 3] + clip[ 0];
    m_Frustum[1][1] = clip[ 7] + clip[ 4];
    m_Frustum[1][2] = clip[11] + clip[ 8];
    m_Frustum[1][3] = clip[15] + clip[12];

    /* Normalize the result */
    t = GLfloat(sqrt(m_Frustum[1][0] * m_Frustum[1][0] + m_Frustum[1][1] * m_Frustum[1][1] + m_Frustum[1][2] * m_Frustum[1][2]));
    m_Frustum[1][0] /= t;
    m_Frustum[1][1] /= t;
    m_Frustum[1][2] /= t;
    m_Frustum[1][3] /= t;

    /* Extract the BOTTOM plane */
    m_Frustum[2][0] = clip[ 3] + clip[ 1];
    m_Frustum[2][1] = clip[ 7] + clip[ 5];
    m_Frustum[2][2] = clip[11] + clip[ 9];
    m_Frustum[2][3] = clip[15] + clip[13];

    /* Normalize the result */
    t = GLfloat(sqrt(m_Frustum[2][0] * m_Frustum[2][0] + m_Frustum[2][1] * m_Frustum[2][1] + m_Frustum[2][2] * m_Frustum[2][2]));
    m_Frustum[2][0] /= t;
    m_Frustum[2][1] /= t;
    m_Frustum[2][2] /= t;
    m_Frustum[2][3] /= t;

    /* Extract the TOP plane */
    m_Frustum[3][0] = clip[ 3] - clip[ 1];
    m_Frustum[3][1] = clip[ 7] - clip[ 5];
    m_Frustum[3][2] = clip[11] - clip[ 9];
    m_Frustum[3][3] = clip[15] - clip[13];

    /* Normalize the result */
    t = GLfloat(sqrt(m_Frustum[3][0] * m_Frustum[3][0] + m_Frustum[3][1] * m_Frustum[3][1] + m_Frustum[3][2] * m_Frustum[3][2]));
    m_Frustum[3][0] /= t;
    m_Frustum[3][1] /= t;
    m_Frustum[3][2] /= t;
    m_Frustum[3][3] /= t;

    /* Extract the FAR plane */
    m_Frustum[4][0] = clip[ 3] - clip[ 2];
    m_Frustum[4][1] = clip[ 7] - clip[ 6];
    m_Frustum[4][2] = clip[11] - clip[10];
    m_Frustum[4][3] = clip[15] - clip[14];

    /* Normalize the result */
    t = GLfloat(sqrt(m_Frustum[4][0] * m_Frustum[4][0] + m_Frustum[4][1] * m_Frustum[4][1] + m_Frustum[4][2] * m_Frustum[4][2]));
    m_Frustum[4][0] /= t;
    m_Frustum[4][1] /= t;
    m_Frustum[4][2] /= t;
    m_Frustum[4][3] /= t;

    /* Extract the NEAR plane */
    m_Frustum[5][0] = clip[ 3] + clip[ 2];
    m_Frustum[5][1] = clip[ 7] + clip[ 6];
    m_Frustum[5][2] = clip[11] + clip[10];
    m_Frustum[5][3] = clip[15] + clip[14];

    /* Normalize the result */
    t = GLfloat(sqrt(m_Frustum[5][0] * m_Frustum[5][0] + m_Frustum[5][1] * m_Frustum[5][1] + m_Frustum[5][2] * m_Frustum[5][2]));
    m_Frustum[5][0] /= t;
    m_Frustum[5][1] /= t;
    m_Frustum[5][2] /= t;
    m_Frustum[5][3] /= t;
}

void CLensFlare::UpdateFrustumFaster()
{
	GLfloat   clip[16];
	GLfloat   proj[16];
	GLfloat   modl[16];
	GLfloat   t;

	// Get The Current PROJECTION Matrix From OpenGL
	glGetFloatv(GL_PROJECTION_MATRIX, proj);

	// Get The Current MODELVIEW Matrix From OpenGL
	glGetFloatv(GL_MODELVIEW_MATRIX, modl);

	// Combine The Two Matrices (Multiply Projection By Modelview) 
	// But Keep In Mind This Function Will Only Work If You Do NOT
	// Rotate Or Translate Your Projection Matrix
	clip[ 0] = modl[ 0] * proj[ 0];
	clip[ 1] = modl[ 1] * proj[ 5];
	clip[ 2] = modl[ 2] * proj[10] + modl[ 3] * proj[14];
	clip[ 3] = modl[ 2] * proj[11];

	clip[ 4] = modl[ 4] * proj[ 0];
	clip[ 5] = modl[ 5] * proj[ 5];
	clip[ 6] = modl[ 6] * proj[10] + modl[ 7] * proj[14];
	clip[ 7] = modl[ 6] * proj[11];

	clip[ 8] = modl[ 8] * proj[ 0];
	clip[ 9] = modl[ 9] * proj[ 5];
	clip[10] = modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[10] * proj[11];

	clip[12] = modl[12] * proj[ 0];
	clip[13] = modl[13] * proj[ 5];
	clip[14] = modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[14] * proj[11];

	// Extract The Numbers For The RIGHT Plane
	m_Frustum[0][0] = clip[ 3] - clip[ 0];
	m_Frustum[0][1] = clip[ 7] - clip[ 4];
	m_Frustum[0][2] = clip[11] - clip[ 8];
	m_Frustum[0][3] = clip[15] - clip[12];

	// Normalize The Result
	t = GLfloat(sqrt(m_Frustum[0][0] * m_Frustum[0][0] + m_Frustum[0][1] * m_Frustum[0][1] + m_Frustum[0][2] * m_Frustum[0][2]));
	m_Frustum[0][0] /= t;
	m_Frustum[0][1] /= t;
	m_Frustum[0][2] /= t;
	m_Frustum[0][3] /= t;

	// Extract The Numbers For The LEFT Plane
	m_Frustum[1][0] = clip[ 3] + clip[ 0];
	m_Frustum[1][1] = clip[ 7] + clip[ 4];
	m_Frustum[1][2] = clip[11] + clip[ 8];
	m_Frustum[1][3] = clip[15] + clip[12];

	// Normalize The Result
	t = GLfloat(sqrt(m_Frustum[1][0] * m_Frustum[1][0] + m_Frustum[1][1] * m_Frustum[1][1] + m_Frustum[1][2] * m_Frustum[1][2]));
	m_Frustum[1][0] /= t;
	m_Frustum[1][1] /= t;
	m_Frustum[1][2] /= t;
	m_Frustum[1][3] /= t;

	// Extract The BOTTOM Plane
	m_Frustum[2][0] = clip[ 3] + clip[ 1];
	m_Frustum[2][1] = clip[ 7] + clip[ 5];
	m_Frustum[2][2] = clip[11] + clip[ 9];
	m_Frustum[2][3] = clip[15] + clip[13];

	// Normalize The Result
	t = GLfloat(sqrt(m_Frustum[2][0] * m_Frustum[2][0] + m_Frustum[2][1] * m_Frustum[2][1] + m_Frustum[2][2] * m_Frustum[2][2]));
	m_Frustum[2][0] /= t;
	m_Frustum[2][1] /= t;
	m_Frustum[2][2] /= t;
	m_Frustum[2][3] /= t;

	// Extract The TOP Plane
	m_Frustum[3][0] = clip[ 3] - clip[ 1];
	m_Frustum[3][1] = clip[ 7] - clip[ 5];
	m_Frustum[3][2] = clip[11] - clip[ 9];
	m_Frustum[3][3] = clip[15] - clip[13];

	// Normalize The Result
	t = GLfloat(sqrt(m_Frustum[3][0] * m_Frustum[3][0] + m_Frustum[3][1] * m_Frustum[3][1] + m_Frustum[3][2] * m_Frustum[3][2]));
	m_Frustum[3][0] /= t;
	m_Frustum[3][1] /= t;
	m_Frustum[3][2] /= t;
	m_Frustum[3][3] /= t;

	// Extract The FAR Plane
	m_Frustum[4][0] = clip[ 3] - clip[ 2];
	m_Frustum[4][1] = clip[ 7] - clip[ 6];
	m_Frustum[4][2] = clip[11] - clip[10];
	m_Frustum[4][3] = clip[15] - clip[14];

	// Normalize The Result
	t = GLfloat(sqrt(m_Frustum[4][0] * m_Frustum[4][0] + m_Frustum[4][1] * m_Frustum[4][1] + m_Frustum[4][2] * m_Frustum[4][2]));
	m_Frustum[4][0] /= t;
	m_Frustum[4][1] /= t;
	m_Frustum[4][2] /= t;
	m_Frustum[4][3] /= t;

	// Extract The NEAR Plane
	m_Frustum[5][0] = clip[ 3] + clip[ 2];
	m_Frustum[5][1] = clip[ 7] + clip[ 6];
	m_Frustum[5][2] = clip[11] + clip[10];
	m_Frustum[5][3] = clip[15] + clip[14];

	// Normalize The Result
	t = GLfloat(sqrt(m_Frustum[5][0] * m_Frustum[5][0] + m_Frustum[5][1] * m_Frustum[5][1] + m_Frustum[5][2] * m_Frustum[5][2]));
	m_Frustum[5][0] /= t;
	m_Frustum[5][1] /= t;
	m_Frustum[5][2] /= t;
	m_Frustum[5][3] /= t;
}

// This member function checks to see if a sphere is in
// the viewing volume.  
/*BOOL CLensFlare::SphereInFrustum(glPoint p, GLfloat Radius)
{
	int i;

	// The idea here is the same as the PointInFrustum function.

	for(i = 0; i < 6; i++){
		// If the point is outside of the plane then its not in the viewing volume.
		if(m_Frustum[i][0] * p.x + m_Frustum[i][1] * p.y + m_Frustum[i][2] * p.z + m_Frustum[i][3] <= -Radius){
			return(FALSE);
		}
	}
	
	return(TRUE);
}

// This member fuction checks to see if a point is in
// the viewing volume.
BOOL CLensFlare::PointInFrustum(glPoint p)
{
	int i;

	// The idea behind this algorithum is that if the point
	// is inside all 6 clipping planes then it is inside our
	// viewing volume so we can return true.

	for(i = 0; i < 6; i++){
		if(m_Frustum[i][0] * p.x + m_Frustum[i][1] * p.y + m_Frustum[i][2] * p.z + m_Frustum[i][3] <= 0){
			return(FALSE);
		}
	}

    return(TRUE);
}*/

// This member function checks to see if a sphere is in
// the viewing volume.  
bool CLensFlare::SphereInFrustum(GLfloat x, GLfloat y, GLfloat z, GLfloat Radius)
{
	// The idea here is the same as the PointInFrustum function.
	for(int i = 0; i < 6; i++){
		// If the point is outside of the plane then its not in the viewing volume.
		if(m_Frustum[i][0] * x + m_Frustum[i][1] * y + m_Frustum[i][2] * z + m_Frustum[i][3] <= -Radius){
			return false;
		}
	}
	
	return true;
}

// This member fuction checks to see if a point is in
// the viewing volume.
bool CLensFlare::PointInFrustum(GLfloat x, GLfloat y, GLfloat z)
{
	// The idea behind this algorithum is that if the point
	// is inside all 6 clipping planes then it is inside our
	// viewing volume so we can return true.
	for(int i = 0; i < 6; i++){	// Loop through all our clipping planes
		// If the point is outside of the plane then its not in the viewing volume.
		if(m_Frustum[i][0] * x + m_Frustum[i][1] * y + m_Frustum[i][2] * z + m_Frustum[i][3] <= 0){
			return false;
		}
	}

    return true;
}

bool CLensFlare::IsOccluded(GLfloat x, GLfloat y, GLfloat z, GLfloat *a, bool FixVMWareBug)
{
    GLint viewport[4];						//space for viewport data
    GLdouble mvmatrix[16], projmatrix[16];  //space for transform matricex
	GLfloat bufferZ;						//here we will store the read Z from the buffer
	double ProjSunPosX, ProjSunPosY, ProjSunPosZ;

	// Now we will ask OGL to project some geometry for us using the gluProject function.
	// Practically we ask OGL to guess where a point in space will be projected in our current viewport,
	// using arbitrary viewport and transform matrices we pass to the function.
	// If we pass to the function the current matrices  (retrievede with the glGet funcs)
	// we will have the real position on screen where the dot will be drawn.
	// The interesting part is that we also get a Z value back, this means that 
	// reading the REAL buffer for Z values we can discover if the flare is in front or
	// if it's occluded by some objects.
	glGetIntegerv(GL_VIEWPORT, viewport);			//get actual viewport
    glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);	//get actual model view matrix
    glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);	//get actual projiection matrix

	// this asks OGL to guess the 2d position of a 3d point inside the viewport
	gluProject(x, y, z, mvmatrix, projmatrix, viewport, &ProjSunPosX, &ProjSunPosY, &ProjSunPosZ);
	ProjSunPos.x = (float)ProjSunPosX;
	ProjSunPos.y = (float)ProjSunPosY;

	int w = viewport[2];
	int h = viewport[3];

	int sx = (int)ProjSunPosX;
	int sy = (int)ProjSunPosY;

	if(sx < w && sx > 0 && sy < h && sy > 0){
		if(!FixVMWareBug){
			// we read back one pixel from the depth buffer (exactly where our flare should be drawn)
			glReadPixels(sx, sy,1,1,GL_DEPTH_COMPONENT, GL_FLOAT, &bufferZ);
			// now we read it's alpha value
			glReadPixels(sx, sy,1,1,GL_ALPHA, GL_FLOAT, a);
		} else {
			float *pBufferZ = new float[w * h];
			float *pAlpha = new float[w * h];
			// we read back one pixel from the depth buffer (exactly where our flare should be drawn)
			glReadPixels(0, 0, w, h, GL_DEPTH_COMPONENT, GL_FLOAT, pBufferZ);
			// now we read it's alpha value
			glReadPixels(0, 0, w, h, GL_ALPHA, GL_FLOAT, pAlpha);

			bufferZ = pBufferZ[(sy * w) + sx];
			*a = pAlpha[(sy * w) + sx];

			SAFE_DELETE_ARRAY(pBufferZ);
			SAFE_DELETE_ARRAY(pAlpha);
		}
	}
	*a = 1.0f - (*a);

	// if the buffer Z is lower than our flare guessed Z then don't draw 
	// this means there is something in front of our flare
	return (bufferZ < ProjSunPosZ);
}


