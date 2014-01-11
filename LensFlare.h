#ifndef _CLENS_FLARE_H_
#define _CLENS_FLARE_H_
#ifdef __cplusplus

#include <Windows.h>
#include <Math.h>
#include <gl\gl.h>			
#include <gl\glu.h>			
#include "SafeKill.h"
#include <vector>
using namespace std;

#define FADE_IN_LENGTH    0.15f
#define FADE_OUT_LENGTH   0.40f

enum LensFlareType {SunGlow = 0, BigGlow, Halo, Hexa, Streak, RainBowHalo};

class CLensFlare {
private:
	struct FlareColor {float r,g,b,a;};
public:
	class FlareStruct {
	public:
		FlareStruct(){Initialize(SunGlow, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);}
		LensFlareType Type;
		float Dist;
		float Size;
		FlareColor Color;
		void Initialize(LensFlareType type, float dist, float size, float r, float g, float b, float a){
			Type = type;
			Dist = dist;
			Size = size;
			Color.r = r;
			Color.g = g;
			Color.b = b;
			Color.a = a;
		}
	};
private:
	struct CVector2D{float x,y;};
	CVector2D ProjSunPos;
public:
	CLensFlare();
	~CLensFlare();

	float FadeInTimeLeft;
	float FadeOutTimeLeft;
	GLfloat m_Frustum[6][4];
	vector <FlareStruct> Flares;

	void InitializeFlareObj();
	void GetSunProjposition(float *x, float *y);

	void UpdateFrustum();
	void UpdateFrustumFaster();
	//BOOL PointInFrustum(glPoint p);
	//BOOL SphereInFrustum(glPoint p, GLfloat Radius);
	bool PointInFrustum(GLfloat x, GLfloat y, GLfloat z);
	bool SphereInFrustum(GLfloat x, GLfloat y, GLfloat z, GLfloat Radius);
	bool IsOccluded(GLfloat x, GLfloat y, GLfloat z, GLfloat *a, bool FixVMWareBug = false);

	void RandomizeFunc();
};

#endif
#endif //_CLENS_FLARE_H_
