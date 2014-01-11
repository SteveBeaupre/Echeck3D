#include "CloudsNoise.h"

CCloudsNoise *pCloudsNoise;

DWORD  dwUpdateNoise;
DWORD  WINAPI UpdateNoiseThread(void*);
HANDLE hUpdateNoise = NULL;
HANDLE hTerminateUpdateNoiseThreadEvent = NULL;

const int ImageWidth  = PN_BUFFER_WIDTH;
const int ImageHeight = PN_BUFFER_HEIGHT;
const int ImageChan   = PN_CHANNEL_COUNT;

const int seed = 63;

const float ShadowBufferScale = 0.75f;

UINT Step = 0;

#ifdef COLORIZE_NOISE
	int NoiseColor = 0;
#endif

//Constructor
CCloudsNoise::CCloudsNoise()
{
	hTerminateUpdateNoiseThreadEvent = CreateEvent(NULL, 0, 0, "TerminateUpdateNoiseThreadEvent");
	//Get a pointer to this class(for the thread)
	pCloudsNoise = this;
	//Get a pointer to the front and back buffer
	pImage = &FrontBuffer[0][0][0];
	pBackBuffer = &BackBuffer[0][0][0];
	//Get a pointer to the front and back shadow buffer
	pShadowImage = &FrontShadowBuffer[0][0][0];
	pBackShadowBuffer = &BackShadowBuffer[0][0][0];
	//Fill the Front buffer
	//MakeImage();
}

//Destructor
CCloudsNoise::~CCloudsNoise()
{
	//Wait for the thread to terminate is task before closing
	WaitForSingleObject(hUpdateNoise, INFINITE);
	CloseHandle(hUpdateNoise);
	CloseHandle(hTerminateUpdateNoiseThreadEvent);
	hUpdateNoise = NULL;
	hTerminateUpdateNoiseThreadEvent = NULL;
}

//This is the main function 
void CCloudsNoise::MakeImage()
{
	static bool FirstImg = Step == 0;

	int x,y, xStartValue;
	int width;
	float scale;
	float disp1,disp2,disp3,disp4,disp5,disp6;
	
	scale = 1;
	width = 12413;

	if(FirstImg){
		xStartValue = 0;
	} else {
		#ifdef USE_BACK_BUFFER
 			SwapBuffer();
			return ;
		#else
			xStartValue = ImageWidth;
			int ImgSizeDiv2 = (ImageWidth*ImageWidth*ImageChan);
			memcpy(&FrontBuffer[0][0][0], &FrontBuffer[0][0][0] + ImgSizeDiv2, ImgSizeDiv2);
		#endif
	}

	for(y=0;y< ImageWidth; y++){
        for(x=xStartValue;x<ImageHeight; x++){
			
			disp1 = CloudsNoise((x+(ImageWidth*Step))*scale,y*scale,width,1,seed,100);
			disp2 = CloudsNoise((x+(ImageWidth*Step))*scale,y*scale,width,1,seed,25);
			disp3 = CloudsNoise((x+(ImageWidth*Step))*scale,y*scale,width,1,seed,12.5f);
			disp4 = CloudsNoise((x+(ImageWidth*Step))*scale,y*scale,width,1,seed,6.25f);
			disp5 = CloudsNoise((x+(ImageWidth*Step))*scale,y*scale,width,1,seed,3.125f);
			disp6 = CloudsNoise((x+(ImageWidth*Step))*scale,y*scale,width,1,seed,1.56f);

			Colorize((float)((int)disp1 + (int)(disp2*.25) + (int)(disp3*.125) + (int)(disp4*.0625) + (int)(disp5*.03125) + (int)(disp6*.0156)), &Pixel[0]);
			
			FrontBuffer[x][y][0] = (UCHAR)Pixel[0];
			#ifdef MAKE_ALPHA_CHANNEL
				FrontBuffer[x][y][1] = (UCHAR)Pixel[1]; 

				FrontShadowBuffer[x][y][0] = (UCHAR)(Pixel[1] * ShadowBufferScale);
			#else
				FrontShadowBuffer[x][y][0] = (UCHAR)(Pixel[0] * ShadowBufferScale);
			#endif
		}
	}


	// Change noise color
	#ifdef COLORIZE_NOISE
		if(NoiseColor++ >= 2){NoiseColor = 0;}
	#endif
	//Only happen the first time this function is called
	if(FirstImg){
		FirstImg = false;
		Step++;
	} 
	// Inc Step by one
	Step++; 

	#ifdef USE_BACK_BUFFER
		// Start a thread that will generate the next image in the back buffer
		StartBackgroundProcess();
	#endif

		
}        

///////////////////////////////////////////////////////////////
//////////The 5 functions that make our clouds noise///////////
///////////////////////////////////////////////////////////////

inline float CCloudsNoise::InterPolation(float a, float b, float c)
{    
    return a+(b-a)*c*c*(3-2*c);                
}


inline float CCloudsNoise::InterLinear(float a, float b, float c)
{     
	return a*(1-c)+b*c;                                          
}

inline float CCloudsNoise::Noise(int x)
{
    x = (x<<13)^x;
    return (float)(((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 2147483648.0);
}

float CCloudsNoise::CloudsNoise(float x,float y,int width,int octaves,int seed,float periode)
{
	float a,b,value,freq,zone_x,zone_y;
	int s,box,num,step_x,step_y;
	static const int amplitude = 128;
	int noisedata;
	
	freq = 1.0f/periode;
	
	for(s = 0; s < octaves; s++){
        num    = (int)(width*freq);
		step_x = (int)(x*freq);
		step_y = (int)(y*freq);
		zone_x = x*freq-step_x;
		zone_y = y*freq-step_y;
		box    = step_x+step_y*num;
        noisedata = (box+seed);
		a      = InterPolation(Noise(noisedata),Noise(noisedata+1),zone_x);
		b      = InterPolation(Noise(noisedata+num),Noise(noisedata+1+num),zone_x);
		value  = InterPolation(a,b,zone_y)*amplitude;
		
	}
	return value;
}


inline void CCloudsNoise::Colorize(float valor, float *pPixel)
{
	const int Amplitude = 100;
	float fVal, fAlphaVal, DegVal, Dif;
	float Darkness = 1.5f;

	//Get the pixel color
	fVal = InterLinear(valor,0,0);

	
	fAlphaVal = fVal; //Modified

	#ifndef USE_ORIGINAL_NOISE_CODE
	//This filter is too fucking good(and me too!)!!!
	//That what i needed to shape the cloud!
	{
		//Check if the pixel value is lower than 127
		bool IsLowerThan127;
		IsLowerThan127 = fVal <= 127.0f;

		//convert value in the range 0-255 to 0-360
		DegVal = (fVal / 255.0f) * 360.0f;
		
		//Get the sin number we must substract
		Dif = sin(DEGTORAD(DegVal)) * Amplitude;
		
		//The good formula(Do white  centered cloud)
		//fVal <= 127.0f ? fVal -= Dif : fVal -= Dif * Darkness;
		//Too cool, discoved by accident!(Do dark centered cloud)
		IsLowerThan127 ? fVal -= Dif: fVal += Dif * Darkness;
		//
		//IsLowerThan127 ? fVal = 0.0 : fVal += Dif * Darkness;
		//Avoid under/over flow
		CLAMP_FLOAT_TO_CHAR(fVal)


		#ifdef MAKE_ALPHA_CHANNEL
			//The original formula
			//IsLowerThan127 ? fAlphaVal = fVal : fAlphaVal = 255.0f;
			
			/*IsLowerThan127 ? fAlphaVal = fVal : fAlphaVal = 127.0f;

			fAlphaVal *= (Darkness*2.5f);*/

			//good!
			//fAlphaVal <= 127.0f ? fAlphaVal -= Dif-64 : fAlphaVal = 192.0f - (Dif / 1.5f);

			//Exellent!
			fAlphaVal <= 127.0f ? fAlphaVal -= Dif-32 : fAlphaVal = 160.0f - Dif;
			fAlphaVal *= 1.3f;

			//Avoid under/over flow
			CLAMP_FLOAT_TO_CHAR(fAlphaVal)

		#endif

		//Brighten our cloud by 1/2
		fVal *= Darkness;
		//fAlphaVal *= Darkness;
		//Avoid under/over flow
		CLAMP_FLOAT_TO_CHAR(fVal)
		
	}

	#endif

	/*#ifndef COLORIZE_NOISE
		pPixel[0] = fVal;
		pPixel[1] = fVal;
		pPixel[2] = fVal;
	#else
		NoiseColor == 0 ? pPixel[0] = fVal : pPixel[0] = 0;
		NoiseColor == 1 ? pPixel[1] = fVal : pPixel[1] = 0;
		NoiseColor == 2 ? pPixel[2] = fVal : pPixel[2] = 0;
	#endif*/

	pPixel[0] = fVal;
	#ifdef MAKE_ALPHA_CHANNEL
		pPixel[1] = fAlphaVal;
	#endif
}

///////////////////////////////////////////////////////////////
///////////////These function are for multithreading///////////
///////////////////////////////////////////////////////////////
//Start the thread 
void CCloudsNoise::StartBackgroundProcess()
{
	hUpdateNoise = CreateThread(NULL, 0, UpdateNoiseThread, NULL, 0, &dwUpdateNoise);	
}


//The second function that will create our image, used by the other thread
void CCloudsNoise::MakeBackBufferImage()
{
	int x,y;
	int width;
	float disp1,disp2,disp3,disp4,disp5,disp6;
	float scale;
	
	scale = 1;
	width = 12413;

	//HWND hWnd = FindWindow(NULL, "Procedural Noise - By Vortex");
	
	for(y=0;y< ImageWidth; y++){
        for(x=0;x<ImageWidth; x++){
			
			disp1 = CloudsNoise((x+(ImageWidth*Step))*scale,y*scale,width,1,seed,100);
			disp2 = CloudsNoise((x+(ImageWidth*Step))*scale,y*scale,width,1,seed,25);
			disp3 = CloudsNoise((x+(ImageWidth*Step))*scale,y*scale,width,1,seed,12.5f);
			disp4 = CloudsNoise((x+(ImageWidth*Step))*scale,y*scale,width,1,seed,6.25f);
			disp5 = CloudsNoise((x+(ImageWidth*Step))*scale,y*scale,width,1,seed,3.125f);
			disp6 = CloudsNoise((x+(ImageWidth*Step))*scale,y*scale,width,1,seed,1.56f);
			
			Colorize((float)((int)disp1 + (int)(disp2*.25) + (int)(disp3*.125) + (int)(disp4*.0625) + (int)(disp5*.03125) + (int)(disp6*.0156)), &BackPixel[0]);
			
			BackBuffer[x][y][0] = (UCHAR)BackPixel[0];
			#ifdef MAKE_ALPHA_CHANNEL
				BackBuffer[x][y][1] = (UCHAR)BackPixel[1]; 

				BackShadowBuffer[x][y][0] = (UCHAR)(BackPixel[1] * ShadowBufferScale);
			#else
				BackShadowBuffer[x][y][0] = (UCHAR)(BackPixel[0] * ShadowBufferScale);
			#endif
			/*if(x == 256)
				Sleep(1);*/

		}
		DWORD WaitRes;
		WaitRes = WaitForSingleObject(hTerminateUpdateNoiseThreadEvent, 1);
		// Stop the thread?
		if(WaitRes == WAIT_OBJECT_0)
			goto ExitLoop;
	}
	ExitLoop:

	// Change noise color
	#ifdef COLORIZE_NOISE
		if(NoiseColor++ >= 2){NoiseColor = 0;}
	#endif
	// Inc Step by one
	Step++; 
}

//Copy the back buffer image to the front buffer
void CCloudsNoise::SwapBuffer()
{
	//get the half of the front buffer size, witch is 2 time the back buffer size
	static const int ImgSizeDiv2       = ImageWidth * ImageWidth * ImageChan;
	static const int ShadowImgSizeDiv2 = ImageWidth * ImageWidth * PN_SHADOW_CHANNEL_COUNT;

	// Wait 'till the back buffer is filled, 
	// stalling the main thread if not
	WaitForSingleObject(hUpdateNoise, INFINITE);
	CloseHandle(hUpdateNoise);
	hUpdateNoise = NULL;

	
	// Fill the 1st half of the front buffer with the 2nd half of this buffer
	memcpy(pImage,             pImage+ImgSizeDiv2, ImgSizeDiv2);	
	// Copy the back buffer to the 2nd half of the front buffer
	memcpy(pImage+ImgSizeDiv2, pBackBuffer,        ImgSizeDiv2);	
	

	// Fill the 1st half of the front shadow buffer with the 2nd half of this buffer
	memcpy(pShadowImage,                   pShadowImage+ShadowImgSizeDiv2, ShadowImgSizeDiv2);	
	// Copy the back shadow buffer to the 2nd half of the front shadow buffer
	memcpy(pShadowImage+ShadowImgSizeDiv2, pBackShadowBuffer,              ShadowImgSizeDiv2);	
	
	
	// Redraw the back buffer, in another thread
	StartBackgroundProcess();
}

void CCloudsNoise::CancelBackgroundProcess()
{
	// if the last thread isn't fisished yet, this will tell it to do so, 
	// and will wait for it to die, then we reset the event...
	SetEvent(hTerminateUpdateNoiseThreadEvent);
	WaitForSingleObject(hUpdateNoise, INFINITE);
	ResetEvent(hTerminateUpdateNoiseThreadEvent);
}

// Create the next image in background
DWORD WINAPI UpdateNoiseThread(void*)
{
	//Give a lower priority to this thread than the main thread
	SetThreadPriority(hUpdateNoise, THREAD_PRIORITY_LOWEST);
	
	//Generate the image
	pCloudsNoise->MakeBackBufferImage();

	//MessageBeep(MB_ICONASTERISK);

	return 0;
}

///////////////////////////////////////////////////////////////
///////////////Bind the texture to OpenGL//////////////////////
///////////////////////////////////////////////////////////////
void CCloudsNoise::BindTexureToOpenGL(UINT *TexID, UINT *ShadowTexID)
{
	glEnable(GL_TEXTURE_2D);   

	static bool Init = false;
	// Generate the new one
	if(!Init){
		Init = true;

		glGenTextures(1, TexID);
		// Bind it to texture object
		glBindTexture(GL_TEXTURE_2D, *TexID);
		// Bilinear filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Load the texture in memory
		glTexImage2D(GL_TEXTURE_2D, 0, ImageChan, PN_BUFFER_WIDTH, PN_BUFFER_HEIGHT, 0, ImageChan == 1 ? GL_LUMINANCE : GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, pImage);

		// Generate the new one
		glGenTextures(1, ShadowTexID);
		// Bind it to texture object
		glBindTexture(GL_TEXTURE_2D, *ShadowTexID);
		// Bilinear filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Load the texture in memory
		glTexImage2D(GL_TEXTURE_2D, 0, PN_SHADOW_CHANNEL_COUNT, PN_BUFFER_WIDTH, PN_BUFFER_HEIGHT, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pShadowImage);

	} else {
		glBindTexture(GL_TEXTURE_2D, *TexID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, PN_BUFFER_WIDTH, PN_BUFFER_HEIGHT, ImageChan == 1 ? GL_LUMINANCE : GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, pImage);
		
		glBindTexture(GL_TEXTURE_2D, *ShadowTexID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, PN_BUFFER_WIDTH, PN_BUFFER_HEIGHT, GL_LUMINANCE, GL_UNSIGNED_BYTE, pShadowImage);
	}
}

///////////////////////////////////////////////////////////////
//////Init and save the first content of the two buffer///////
///////////////////////////////////////////////////////////////

// Update the Step value correctly after using the 
// LoadPreProcessingData function(must be called manually)
/*void CCloudsNoise::SkipInit()
{
	#ifdef USE_BACK_BUFFER
		Step += 3;
	#else
		Step += 2;
	#endif
}*/

void CCloudsNoise::LoadPreProcessingData(char *fname)
{
	//Open the file
	std::ifstream fin(fname, std::ios_base::binary);

	//Load data
	int ToRead       = ImageHeight * ImageWidth * ImageChan;
	int ToReadShadow = ImageHeight * ImageWidth * PN_SHADOW_CHANNEL_COUNT;
	fin.read((char *) pImage, ToRead);
	fin.read((char *) pShadowImage, ToReadShadow);

	//Set Step correctly
	Step += 2;
	#ifdef USE_BACK_BUFFER
		++Step;
	#endif
}

void CCloudsNoise::LoadPreProcessingData2(char *fname)
{
	// Create a file pointer and open the image file
	FILE *file = fopen(fname, "rb");

	if(file != NULL){

		int ToRead       = ImageWidth * ImageHeight * ImageChan;
		int ToReadShadow = ImageWidth * ImageHeight * PN_SHADOW_CHANNEL_COUNT;

		fread(pImage,1,ToRead,file);				// If So Read Next 6 Header Bytes
		fread(pShadowImage,1,ToReadShadow,file);	// If So Read Next 6 Header Bytes
	}
	
	//close the file
	fclose(file);	
	
	//Set Step correctly
	Step += 2;
	#ifdef USE_BACK_BUFFER
		Step++;
	#endif
}

void CCloudsNoise::SavePreProcessingData(char *fname)
{
	//Generate data
	int x,y;
	int width;
	float scale;
	float disp1,disp2,disp3,disp4,disp5,disp6;
	
	scale = 1;
	width = 12413;


	for(y=0;y< ImageWidth; y++){
        for(x=0;x<ImageHeight; x++){
			
			disp1 = CloudsNoise((x+ImageWidth)*scale,y*scale,width,1,seed,100);
			disp2 = CloudsNoise((x+ImageWidth)*scale,y*scale,width,1,seed,25);
			disp3 = CloudsNoise((x+ImageWidth)*scale,y*scale,width,1,seed,12.5f);
			disp4 = CloudsNoise((x+ImageWidth)*scale,y*scale,width,1,seed,6.25f);
			disp5 = CloudsNoise((x+ImageWidth)*scale,y*scale,width,1,seed,3.125f);
			disp6 = CloudsNoise((x+ImageWidth)*scale,y*scale,width,1,seed,1.56f);
			
			Colorize((float)((int)disp1 + (int)(disp2*.25) + (int)(disp3*.125) + (int)(disp4*.0625) + (int)(disp5*.03125) + (int)(disp6*.0156)), &Pixel[0]);
			
			TmpBuffer[x][y][0] = (UCHAR)Pixel[0];
			#ifdef MAKE_ALPHA_CHANNEL
				TmpBuffer[x][y][1] = (UCHAR)Pixel[1]; 

				TmpShadowBuffer[x][y][0] = (UCHAR)(Pixel[1] * ShadowBufferScale);
			#else
				TmpShadowBuffer[x][y][0] = (UCHAR)(Pixel[0] * ShadowBufferScale);
			#endif
		}
	}

	//Create the file
	std::ofstream fout(fname, std::ios_base::trunc | std::ios_base::binary);

	//Save data
	fout.write((char *) &TmpBuffer[0][0][0],       ImageWidth * ImageHeight * ImageChan);
	fout.write((char *) &TmpShadowBuffer[0][0][0], ImageWidth * ImageHeight * PN_SHADOW_CHANNEL_COUNT);
}

