#ifndef _CCLOUDSNOISE_H_
#define _CCLOUDSNOISE_H_
#ifdef __cplusplus

#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdio.h>
#include <iostream> 
#include <fstream>
#include <math.h>

/*#ifdef _ECHECK3D
	#define MAKE_ALPHA_CHANNEL
#endif*/

//#define USE_ORIGINAL_NOISE_CODE
#define MAKE_ALPHA_CHANNEL

#ifdef MAKE_ALPHA_CHANNEL
	#define PN_CHANNEL_COUNT  2
#else
	#define PN_CHANNEL_COUNT  1
#endif

#define PN_SHADOW_CHANNEL_COUNT  1


#define RADIAN   0.01745329251994329576f
#define DEGTORAD(n) ((n) * RADIAN)

#define USE_BACK_BUFFER
//#define COLORIZE_NOISE

#define PN_BUFFER_WIDTH      512
#define PN_BUFFER_HEIGHT     1024

#define MAX_STEP      3906250
	

#define CLAMP_FLOAT_TO_CHAR(n) if((n) < 0.0f){(n) = 0.0f;}if((n) > 255.0f){(n) = 255.0f;}


class CCloudsNoise {
private:
	float Pixel[PN_CHANNEL_COUNT];
	float BackPixel[PN_CHANNEL_COUNT];
	//The^skyplane buffer
	GLubyte FrontBuffer[PN_BUFFER_HEIGHT][PN_BUFFER_WIDTH][PN_CHANNEL_COUNT];
	GLubyte BackBuffer [PN_BUFFER_WIDTH] [PN_BUFFER_WIDTH][PN_CHANNEL_COUNT];
	//The terrain buffer
	GLubyte FrontShadowBuffer[PN_BUFFER_HEIGHT][PN_BUFFER_WIDTH][PN_SHADOW_CHANNEL_COUNT];
	GLubyte BackShadowBuffer [PN_BUFFER_WIDTH] [PN_BUFFER_WIDTH][PN_SHADOW_CHANNEL_COUNT];

	//Used to load/save the front buffers
	GLubyte TmpBuffer[PN_BUFFER_HEIGHT][PN_BUFFER_WIDTH][PN_CHANNEL_COUNT];
	GLubyte TmpShadowBuffer[PN_BUFFER_HEIGHT][PN_BUFFER_WIDTH][PN_SHADOW_CHANNEL_COUNT];
	
	//Our private pointer to back buffers
	GLubyte *pBackBuffer;
	GLubyte *pBackShadowBuffer;
public:
	CCloudsNoise();
	~CCloudsNoise();

	//Make the images
	void MakeImage();
	void MakeBackBufferImage();

	//Load/Save the 1st front Buffers data
	void LoadPreProcessingData(char *fname);
	void LoadPreProcessingData2(char *fname);
	void SavePreProcessingData(char *fname);

	//Load Texture in OpenGL
	void BindTexureToOpenGL(UINT *TexID, UINT *ShadowTexID);

	//Control multithreading
	void SkipInit(); 
	void StartBackgroundProcess(); 
	void CancelBackgroundProcess();

	//Globals pointers to our front buffers
	GLubyte *pImage;
	GLubyte *pShadowImage;
private:
	//Other noise generator related stuff...
	float InterPolation(float a, float b, float c);
	float InterLinear(float a, float b, float c);
	float Noise(int x);
	float CloudsNoise(float x,float y,int width,int octaves,int seed,float periode);
	void  Colorize(float valor, float *pPixel);
	//Swap the buffer data in multithreading mode
	void  SwapBuffer();
};

#endif
#endif //--_CCLOUDSNOISE_H_
