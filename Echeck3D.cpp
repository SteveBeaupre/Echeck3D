#include "Echeck3D.h"

// Initialization info...
CInitInfo InitInfo;
// Global pointer to the game engine
CGraphicsEngine *pEngine = NULL;

CE3DLog Log;

//-----------------------------------------------------------------------------
// Name : WinMain() (Application Entry Point)
// Desc : Entry point for program, App flow starts here, end here.
//-----------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Create and open the log file
	Log.OpenForWriting("Log.txt", false);

	#ifdef RUN_WINDOWED
		Log.LogArgs("Warning: RUN_WINDOWED Macro defined...\n");
	#endif
	#ifdef LITE_MODE
		Log.LogArgs("Warning: LITE_MODE Macro defined...");
	#endif

	// Init. winsock
	bool Res = InitializeWinsock();
	Log.LogArgs("Initialising Winsock........ %s", Res ? "Sucess!" : "Failed...");

	// Init. FMod
	Res = InitializeSoundFXEngine(44100, 32);
	Log.LogArgs("Initialising Sound Engine... %s\n", Res ? "Sucess!" : "Failed...");

	// Create the game engine object
	pEngine = new CGraphicsEngine;
	
	pEngine->pLog = &Log;
	pEngine->GameState = MAIN_MENU;
	pEngine->Menus.SetGameStatePtr(&pEngine->GameState);
	pEngine->Menus.SetPreCalcPtr(&pEngine->PreCalc);
	pEngine->Menus.LoadGameSettings();

	pEngine->SetUseEscKeyToQuit(false);
	pEngine->LoadAIDLL();

	#ifndef RUN_WINDOWED
		InitInfo.FullScreen = true;
	#else
		InitInfo.FullScreen = false;
	#endif
	InitInfo.StartMaximized = 0;
	
	InitInfo.Width  = GetSystemMetrics(SM_CXSCREEN);
	InitInfo.Height = GetSystemMetrics(SM_CYSCREEN);
	#ifdef SMALL_GAME_WINDOW
		if(!InitInfo.FullScreen){
			InitInfo.Width  /= 2;
			InitInfo.Height /= 2;
		}
	#endif


	//InitInfo.WideScreen = false;
	float Ratio = (float)InitInfo.Width / (float)InitInfo.Height;
	InitInfo.WideScreen = Ratio >= 1.35f;
	pEngine->GameWindow.SetDevSettings(InitInfo.WideScreen ? 1360 : 1024, 768);
	pEngine->PreCalc.BuildMenusStuffs((int)((float)InitInfo.Width - (1024.0f * ((float)InitInfo.Height / 768.0f))) / 2);

	// Initialize various data to init the engine
	InitInfo.hInst = hInstance;
	sprintf(InitInfo.ClassName,     "Echeck3D v3.0 Class"); 
	sprintf(InitInfo.WindowCaption, "Echeck3D v3.0"); 
	sprintf(InitInfo.IconName,      "Echeck3D.ico"); 
	sprintf(InitInfo.IconSmName,    "Echeck3D.ico"); 
	InitInfo.IconID   = NULL;
	InitInfo.IconSmID = NULL;

	Log.LogArgs("Game Window Settings = {");
	Log.LogArgs("  Screen Size = %dx%d", InitInfo.Width, InitInfo.Height);
	Log.LogArgs("  Window Mode = %s", InitInfo.FullScreen ? "FullScreen" : "Windowed");
	Log.LogArgs("  Wide Screen = %s", InitInfo.WideScreen ? "Yes" : "No");
	Log.LogArgs("  Caption     = \"%s\"", InitInfo.WindowCaption);
	Log.LogArgs("}\n");

	// Initialise and start the engine msg loop
	Log.LogArgs("Initializing Engine...\n");
	if(pEngine->InitEngine(&InitInfo)){
		Log.LogArgs("Engine Initialized Successfully!\n");
		pEngine->StartEngine();
	} else {
		Log.LogArgs("Initializing Engine Failed...\n");
	}
	// Shutdown our engine
	Log.LogArgs("Shuting Down Game Engine...");
	pEngine->ShutdownEngine();
	
	Log.LogArgs("Freeing AI Dll...");
	pEngine->FreeAIDLL();

	// Save game settings
	pEngine->Menus.SaveGameSettings();

	// Delete the game engine object
	Log.LogArgs("Deleting game engine...");
	SAFE_DELETE_OBJECT(pEngine);
	
	// Shutdown FMOD
	Log.LogArgs("Shuting Down Sound System...");
	ShutdownSoundFXEngine();

	// Shutdown winsock
	Log.LogArgs("Shuting Down Winsock...");
	ShutdownWinsock();

	// Close the log file
	Log.Close();

	return 0;
}

bool CGraphicsEngine::CanRenderSky()
{
	return !LiteMode && Menus.Options.ShowSky;
}
	
bool CGraphicsEngine::CanRenderClouds()
{
	return !LiteMode && Menus.Options.ShowClouds;
}
	
bool CGraphicsEngine::CanRenderTerrain()
{
	return !LiteMode && Menus.Options.ShowTerrain;
}
	
/*bool CGraphicsEngine::CanRenderTrees()
{
	return !LiteMode && Menus.Options.ShowTrees;
}*/
	
bool CGraphicsEngine::CanRenderRocks()
{
	return !LiteMode && Menus.Options.ShowRocks;
}
	
bool CGraphicsEngine::CanRenderWater()
{
	return !LiteMode && Menus.Options.ShowWater;
}
	
bool CGraphicsEngine::CanRenderChessBoard()
{
	return Menus.Options.ShowChessboard;
}
	
bool CGraphicsEngine::CanRenderColumn()
{
	return !LiteMode && Menus.Options.ShowColumn;
}
	
bool CGraphicsEngine::CanRenderChessPieces()
{
	return Menus.Options.ShowChessPieces;
}
	
bool CGraphicsEngine::CanRenderChessPiecesNextMoves()
{
	return Menus.Options.ShowChessPieces && Menus.Options.ShowNextMoves && CurrentSel > -1;
}
	
bool CGraphicsEngine::CanRenderLensFlares()
{
	return !LiteMode && Menus.Options.ShowLensFlares;
}
	
bool CGraphicsEngine::CanUseShaders()
{
	return !LiteMode && IsShadersSupported() && Menus.Options.UseShader;
}
	
bool CGraphicsEngine::CanUseShadow()
{
	return !LiteMode && !Menus.Options.UseLODModels && Menus.Options.ShowShadows;
}
	
bool CGraphicsEngine::CanUseFog()
{
	return !LiteMode && IsFogExtSupported() && Menus.Options.ShowFog;
}
	
bool CGraphicsEngine::ShowPiecesReflection()
{
	return !LiteMode && Menus.Options.ShowReflections;
}
	
bool CGraphicsEngine::ShowGhostPieces()
{
	return !LiteMode && Menus.Options.ShowNextMoves;
}
	
bool CGraphicsEngine::UseLodModels()
{
	return Menus.Options.UseLODModels;
}
	
///////////////////////////////////////////////////////////////////////////////////////////////////////

void CGraphicsEngine::RenderLoadScreen(int PercentDone)
{
	//Clear The Color, depth and stencil buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	CFBO *pFBO = &LoadingFBO;
	pFBO->EnableFBO(true);
	// Set OpenGL for 2D Drawing
	if(pFBO->IsEnabled()){
		int VPCoords[4] = {0, 0, pFBO->GetTexWidth(), pFBO->GetTexHeight()};
		pEngine->Set2DMode(&VPCoords[0]);	
	} else {
		pEngine->Set2DMode();	
	}

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);  
	glEnable(GL_TEXTURE_2D);							
	
	bool Stretch = !pFBO->IsInitialized();
	//Draw the background
	CQuadRect MainRect((float)PreCalc.GetWideScreenOffset(), 0.0f, 1024.0f, 768.0f);
	DrawTexQuads(Textures.LoadScreen[0], &MainRect, NULL, Stretch);

	float Ratio = (float)PercentDone / 100.0f;
	float4 TexCoords;
	ZeroMemory(&TexCoords, sizeof(float4));
	TexCoords.Y = 4.0f * Ratio;
	TexCoords.A = 1.0f;
	CQuadRect BarRect(47.0f + (float)PreCalc.GetWideScreenOffset(), 710.0f, Ratio * 932.0f, 7.0f);
	DrawTexQuads(Textures.LoadScreen[1], &BarRect, &TexCoords, Stretch);
	
	if(pFBO->IsEnabled()){
		pFBO->EnableFBO(false);
		pFBO->DrawFBO(GameWindow.GetWidth(), GameWindow.GetHeight());
	}
}

void CGraphicsEngine::Cast3DShadows(CE3DCamera *pCam)
{
	glClear(GL_STENCIL_BUFFER_BIT);	

	if(!CanUseShadow())
		return;

	float Minv[16];
	float wlp[4], lp[4];

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glPushMatrix();
		glLoadIdentity();									// Reset Matrix

		glGetFloatv(GL_MODELVIEW_MATRIX,Minv);				// Retrieve ModelView Matrix (Stores In Minv)
		lp[0] = SunPosition.X * 100000.0f;					// Store Light Position X In lp[0]
		lp[1] = SunPosition.Y * 100000.0f;					// Store Light Position Y In lp[1]
		lp[2] = SunPosition.Z * 100000.0f;					// Store Light Position Z In lp[2]
		lp[3] = 1.0f;										// Store Light Direction In lp[3]
		Shadows3D.VMatMult(Minv, lp);						// We Store Rotated Light Vector In 'lp' Array
		glGetFloatv(GL_MODELVIEW_MATRIX,Minv);				// Retrieve ModelView Matrix From Minv
		wlp[0] = 0.0f;										// World Local Coord X To 0
		wlp[1] = 0.0f;										// World Local Coord Y To 0
		wlp[2] = 0.0f;										// World Local Coord Z To 0
		wlp[3] = 1.0f;
		Shadows3D.VMatMult(Minv, wlp);						// We Store The Position Of The World Origin Relative To The														// Local Coord. System In 'wlp' Array
		lp[0] += wlp[0];									// Adding These Two Gives Us The
		lp[1] += wlp[1];									// Position Of The Light Relative To
		lp[2] += wlp[2];									// The Local Coordinate System
			
		//
		glLoadIdentity();									// Reset Matrix
		gluLookAt(0.0f, 0.0f, pCam->ModCamDist, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(pCam->GetYRotation(), 1.0f, 0.0f, 0.0f);
		glRotatef(pCam->GetXRotation(), 0.0f, 1.0f, 0.0f);

		Shadows3D.CastShadows(&SVM.StaticObjs, lp, true);
		RenderChessPiecesFor3DShadows(lp);

	glPopMatrix();
	glEnable(GL_TEXTURE_2D);	
	glEnable(GL_LIGHTING);
}

////////////////////////////////////////////////////////////////////////////////////////////

void CGraphicsEngine::CastPlanarShadow()
{
	glClear(GL_STENCIL_BUFFER_BIT);	

	if(!CanUseShadow())
		return;

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);

	glEnable(GL_STENCIL_TEST);
	glColorMask(0, 0, 0, 0);
	
	glStencilFunc(GL_NOTEQUAL, 1, 0xFFFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	Models.ChessBoard[CHESSBOARD_CASES].Draw(VA_VERTEX);
	Models.ChessBoard[CHESSBOARD_TOP].Draw(VA_VERTEX);
	Models.ChessBoard[CHESSBOARD_BOTTOM].Draw(VA_VERTEX);

	//static bool IsShadowMapInit = false; //A enlever si on veut bouger les shadows des pcx
	glPushMatrix();
		/*glTranslatef(0.0f, 1.5f, 0.0f); //Offset the shadow a little to avoid z fighting
		if(!IsShadowMapInit){
			GenerateShadowMatrix(&ShadowMatrix[0]);
			IsShadowMapInit = true;
		}*/
		GenerateShadowMatrix(&ShadowMatrix[0]);
		glMultMatrixf(&ShadowMatrix[0]);               // Add the shadow matrix to the ModelView
		glEnable(GL_DEPTH_TEST);
		glStencilFunc(GL_EQUAL, 1, 0xFFFFFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		RenderChessPiecesForPlanarShadowMapping(UseLodModels());
	glPopMatrix();
	

	glStencilFunc(GL_GEQUAL, 1, 0xFFFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	Models.ChessBoard[CHESSBOARD_CASES].Draw(VA_VERTEX);
	Models.ChessBoard[CHESSBOARD_TOP].Draw(VA_VERTEX);
	Models.ChessBoard[CHESSBOARD_BOTTOM].Draw(VA_VERTEX);

	glColorMask(1, 1, 1, 1);
	
	glDepthMask(GL_TRUE);
	glDisable(GL_STENCIL_TEST);
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
}

////////////////////////////////////////////////////////////////////////////////////////////

void CGraphicsEngine::RenderShadowMap()
{
	if(!CanUseShadow())
		return;

	glColor4f(0.0f, 0.0f, 0.0f, 0.35f * ShadowMapAlpha);

	glEnable(GL_STENCIL_TEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glStencilFunc(GL_NOTEQUAL, 0, 0xFFFFFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	bool Stretch = !FBO.IsInitialized();
	CQuadRect Rect(0.0f, 0.0f, 1024.0f, 768.0f);
	DrawQuads(&Rect, NULL, Stretch);

	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);
	
	// ???
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	glColor3f(1.0f, 1.0f, 1.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////

void CGraphicsEngine::RenderSkyDome()
{
	if(!CanRenderSky())
		return;

	glColor3f(1.0f, 1.0f, 1.0f);
	
	/*if(CanUseShaders()){
		Shader.glUniform4fARB(Shader.hTex0Scale,     1.0f, 1.0f, 0.0f, 0.0f);
		Shader.glUniform4fARB(Shader.hTex1Scale,     1.0f, 1.0f, 0.0f, 0.0f);
		Shader.glUniform4fARB(Shader.hTex0Translate, 0.0f, 0.0f, 0.0f, 0.0f);
		Shader.glUniform4fARB(Shader.hTex1Translate, 0.0f, 0.0f, 0.0f, 0.0f);
	}*/

	glColorMask(1,1,1,0);

	//Enable 1D texture
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_1D);

	glColor3f(1.0f, 1.0f, 1.0f);

	//Load the SkyDome texture
	{
		//Find the offset of the two texture to average
		static int TexOffset;
		TexOffset = (int)GameTime;

		//Upate the color based on the game time
		static float TmpTex[2][3];
		TmpTex[0][0] = SkyDomeColor[0][(TexOffset * 3) + 2] / 255.0f;
		TmpTex[0][1] = SkyDomeColor[0][(TexOffset * 3) + 1] / 255.0f;
		TmpTex[0][2] = SkyDomeColor[0][(TexOffset * 3) + 0] / 255.0f;
		TmpTex[1][0] = SkyDomeColor[1][(TexOffset * 3) + 2] / 255.0f;
		TmpTex[1][1] = SkyDomeColor[1][(TexOffset * 3) + 1] / 255.0f;
		TmpTex[1][2] = SkyDomeColor[1][(TexOffset * 3) + 0] / 255.0f;

		static UINT SkyDomeTexID = 0;
		//If it's the first time this happenning, then we'll create our texture, otherwise, we just update it...
		static bool Init = false;
		if(!Init){
			Init = true;

			//glDeleteTextures(1, &SkyDomeTexID);
			glGenTextures(1, &SkyDomeTexID);
			glBindTexture(GL_TEXTURE_1D, SkyDomeTexID);

			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	

			glTexImage1D(GL_TEXTURE_1D, 0, 3, 2, 0, GL_RGB, GL_FLOAT, TmpTex);
		} else {
			glBindTexture(GL_TEXTURE_1D, SkyDomeTexID);

			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	

			glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 2, GL_RGB, GL_FLOAT, TmpTex);		
		}
	}

	//
	//Models.SkyDome.BindVAs(VA_VT);
	
	if(CanUseMultiTexture()){
		pEngine->EnableVAs(VA_AUXTEXTURE, &Models.SkyDome);
		SetCurrentTextureUnit(1);
		glEnable(GL_TEXTURE_2D);
		//pEngine->EnableAuxiliaryTexture();

		//Control the intensity of interpolation with g_SkyDomeAlpha
		float Col[4] = {0.0f, 0.0f, 0.0f, SkyDomeAlpha};
		//Set texture env setting
		glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, Col);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA);

		//Texture 1 on TU 1
		Models.SkyDome.BindAuxiliaryTexture();
	}

	//
	glPushMatrix();
		glFrontFace(GL_CW);
		glEnable(GL_CULL_FACE);
		//glTranslatef(-SingleViewCamera.xTranslation, -SingleViewCamera.yTranslation, -SingleViewCamera.zTranslation);
		if(!CanUseMultiTexture()){
			Models.SkyDome.Draw(VA_VERTEX | VA_TEXTURE);
		} else {
			Models.SkyDome.Draw(VA_VERTEX | VA_TEXTURE | VA_AUXTEXTURE);
		}
		glDisable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
	glPopMatrix();


	if(CanUseMultiTexture()){
		glDisable(GL_TEXTURE_2D);
		SetCurrentTextureUnit(0);
		pEngine->DisableVAs(VA_AUXTEXTURE, &Models.SkyDome);
		//pEngine->DisableAuxiliaryTexture();
	}
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glColorMask(1,1,1,1);

	glEnable(GL_LIGHTING);
	//Disable 1D texture
	glDisable(GL_TEXTURE_1D);
	glEnable(GL_TEXTURE_2D);
	
	glColor3f(1.0f, 1.0f, 1.0f);
}

void CGraphicsEngine::RenderSkyPlane()
{
	if(!CanRenderClouds())
		return;

	glColor3f(SkyDomeAlpha, SkyDomeAlpha, SkyDomeAlpha);

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	Models.SkyPlane.BindPrimaryTexture();
	//Reset the texture env
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glPushMatrix();

		/*if(CanUseShaders()){
			Shader.glUniform4fARB(Shader.hTex0Scale, 1.0f, 1.0f, 0.0f, 0.0f);
			Shader.glUniform4fARB(Shader.hTex1Scale, 1.0f, 1.0f, 0.0f, 0.0f);
			Shader.glUniform4fARB(Shader.hTex0Translate, 0.0f, g_SkyPlaneTexOffset / (SKYPLANE_TRANSITION_LENGTH * 2), 0.0f, 0.0f);
			Shader.glUniform4fARB(Shader.hTex1Translate, 0.0f, 0.0f, 0.0f, 0.0f);
		} else {*/
			glMatrixMode(GL_TEXTURE);
			glTranslatef(0.0f, SkyPlaneTexOffset / (SKYPLANE_TRANSITION_LENGTH * 2), 0.0f);
			glMatrixMode(GL_MODELVIEW);
		//}

		//Draw the SkyPlane
		glFrontFace(GL_CW);
		glEnable(GL_CULL_FACE);
		glTranslatef(0.0f, -3000.0f, 0.0f);
		Models.SkyPlane.Draw(VA_VT);
		glDisable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		
		//Reset the texture matrix
		/*if(CanUseShaders()){
			Shader.glUniform4fARB(Shader.hTex0Scale,     1.0f, 1.0f, 0.0f, 0.0f);
			Shader.glUniform4fARB(Shader.hTex0Translate, 0.0f, 0.0f, 0.0f, 0.0f);
		} else {*/
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
		//}

	glPopMatrix();
	
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	
	glColor3f(1.0f, 1.0f, 1.0f);
}

void CGraphicsEngine::RenderColumn()
{
	glClear(GL_STENCIL_BUFFER_BIT);	

	if(!CanRenderColumn())
		return;

	if(CanUseShaders()){
		Shader.glUseProgramObjectARB(Shader.m_Program);
		Shader.glUniform1iARB(Shader.hUseLighting, 1);
		if(CanUseFog())
			Shader.glUniform1iARB(Shader.hUsePerVertexFog, 1);
	}

	glEnable(GL_STENCIL_TEST);
	glColorMask(0, 0, 0, 0);
	
	glStencilFunc(GL_NEVER, 1, 1);  // No "real" drawing.
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	
	glEnable(GL_CULL_FACE);
	Models.ChessBoard[CHESSBOARD_CASES].Draw(VA_VERTEX);
	glDisable(GL_CULL_FACE);

	glColorMask(1, 1, 1, 1);

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_NOTEQUAL, 1, 1);

	float ColumnAlpha = ClampColor(SkyDomeAlpha + 0.25f);
	glColor3f(ColumnAlpha, ColumnAlpha, ColumnAlpha);

	Models.Column.BindPrimaryTexture();

	if(CanUseMultiTexture()){
		pEngine->EnableVAs(VA_AUXTEXTURE, &Models.SkyDome);
		SetCurrentTextureUnit(1);
		glEnable(GL_TEXTURE_2D);

		//Texture 1 on TU 1
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD_SIGNED);

		Models.Column.SetAuxiliaryTexture(Textures.WaterReclect[(int)CurrentCaustIndx]);
		Models.Column.BindAuxiliaryTexture();
	}

	DWORD Flags = VA_VTN;
	if(CanUseMultiTexture())
		Flags |= VA_AUXTEXTURE;
	if(CanUseFog()){
		glEnable(GL_FOG);
		Flags |= VA_FOG;
	}

	Models.Column.Draw(Flags);
	
	if(CanUseFog())
		glDisable(GL_FOG);

	if(CanUseMultiTexture()){
		glDisable(GL_TEXTURE_2D);
		SetCurrentTextureUnit(0);
		pEngine->DisableVAs(VA_AUXTEXTURE, &Models.Column);
	}

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if(CanUseShaders()){
		Shader.glUniform1iARB(Shader.hUseLighting, 0);
		Shader.glUniform1iARB(Shader.hUsePerVertexFog, 0);
		Shader.glUseProgramObjectARB(NULL);
	}

	glDisable(GL_STENCIL_TEST);
	glColor3f(1.0f, 1.0f, 1.0f);
}

void CGraphicsEngine::RenderTerrain(BOOL LoRes)
{
	if(!CanRenderTerrain())
		return;

	if(CanUseShaders()){
		Shader.glUseProgramObjectARB(Shader.m_Program);
		Shader.glUniform1iARB(Shader.hUseLighting, 1);
		Shader.glUniform4fARB(Shader.hTex1Translate, 0.0f, SkyPlaneTexOffset / (SKYPLANE_TRANSITION_LENGTH * 2), 0.0f, 0.0f);
		if(CanUseFog())
			Shader.glUniform1iARB(Shader.hUsePerVertexFog, 1);
	}

	float TerrainAlpha = ClampColor(SkyDomeAlpha + 0.125f);
	glColor3f(TerrainAlpha, TerrainAlpha, TerrainAlpha);
	
	//Bind the grass texture
	Models.Terrain[LoRes].BindPrimaryTexture();
	
	if(CanUseMultiTexture()){
		pEngine->EnableVAs(VA_AUXTEXTURE, &Models.Terrain[LoRes]);
		SetCurrentTextureUnit(1);
		glEnable(GL_TEXTURE_2D);

		if(!CanUseShaders()){
			glMatrixMode(GL_TEXTURE);
			glTranslatef(0.0f, SkyPlaneTexOffset / (SKYPLANE_TRANSITION_LENGTH * 2), 0.0f);
			glMatrixMode(GL_MODELVIEW);
		}

		//Texture 1 on TU 1
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);

		Models.Terrain[LoRes].BindAuxiliaryTexture();
	}
	
	DWORD Flags = VA_VTN;
	if(CanUseMultiTexture())
		Flags |= VA_AUXTEXTURE;
	if(CanUseFog()){
		glEnable(GL_FOG);
		Flags |= VA_FOG;
	}
	
	glPushMatrix();
		glEnable(GL_CULL_FACE);
		Models.Terrain[LoRes].Draw(Flags);
		glDisable(GL_CULL_FACE);
	glPopMatrix();

	if(CanUseFog())
		glDisable(GL_FOG);

	//Switch off TU 1
	if(CanUseMultiTexture()){
		//Reset the texture matrix
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);

		glDisable(GL_TEXTURE_2D);
		SetCurrentTextureUnit(0);
		pEngine->DisableVAs(VA_AUXTEXTURE, &Models.Terrain[LoRes]);
		//Set the tex env to GL_MODULATE
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	if(CanUseShaders()){
		Shader.glUniform1iARB(Shader.hUseLighting, 0);
		Shader.glUniform4fARB(Shader.hTex1Translate, 0.0f, 0.0f, 0.0f, 0.0f);
		if(CanUseFog())
			Shader.glUniform1iARB(Shader.hUsePerVertexFog, 0);
		Shader.glUseProgramObjectARB(NULL);
	}

	glColor3f(1.0f, 1.0f, 1.0f);
}


////////////////////////////////////////////////////////////////////////////////////////////

void CGraphicsEngine::RenderTerrainForCamCollision(BOOL LoRes)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	
	
	//This will draw in a buffer 10 time smaller than our real screen size,
	//resolving the speed problem, and keeping good accuracy...
	static const int DivRatio    = 4;
	static const int WidthDiv10  = GameWindow.GetWidth()  / DivRatio;
	static const int HeightDiv10 = GameWindow.GetHeight() / DivRatio;

	// Set OpenGL to 3D mode
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0f, GameWindow.GetRatio(), NEAR_PLANE, pCurrentCam->GetDistance());
	glMatrixMode(GL_MODELVIEW);
	// Set viewport
	glViewport(0, 0, WidthDiv10, HeightDiv10);					

	glColor3f(1.0f, 1.0f, 1.0f);

	//Disable stuff we don't need
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);

	//We need depth testing
	glEnable(GL_DEPTH_TEST); 
	
	glPushMatrix();	

		// Set the camera position
		pCurrentCam->SetCamera();

		//Enable face culling and set it to see only the back-facing tris
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		//Render the mesh
		Models.Terrain[LoRes].Draw(VA_VERTEX);
		//Disable face culling and set it to normal mode
		glCullFace(GL_BACK);
		glDisable(GL_CULL_FACE);


		/////////////////////////////
		//Now we analyse the scene...
		/////////////////////////////
		//Read the pixel color in the center of the screen
		float PixelColor[3];
		#ifndef FIX_VMWARE_PICKING_BUG
		glReadPixels((GameWindow.GetWidth() / 2) / DivRatio, (GameWindow.GetHeight() / 2) / DivRatio, 1,1, GL_RGB, GL_FLOAT, &PixelColor);
		#else
		int x, y;
		x = WidthDiv10 / 2;
		y = HeightDiv10 / 2;
		float *pPixelColor = new float[WidthDiv10 * HeightDiv10 * 3];
		glReadPixels(0, 0, WidthDiv10, HeightDiv10, GL_RGB, GL_FLOAT, pPixelColor);
		memcpy(&PixelColor, &pPixelColor[((y * WidthDiv10) + x) * 3], sizeof(float) * 3);
		SAFE_DELETE_ARRAY(pPixelColor);
		#endif
		
		//Check if the pixel drawn is white, the terrain color
		if(PixelColor[0] == 1.0f && PixelColor[1] == 1.0f && PixelColor[2] == 1.0f){
			
			GLint viewport[4];
			GLdouble modelview[16];
			GLdouble projection[16];
			static GLfloat winX = (float)((GameWindow.GetWidth() / 2) / DivRatio);
			static GLfloat winY = (float)((GameWindow.GetHeight() / 2) / DivRatio);
			static GLfloat winZ;
			GLdouble posX, posY, posZ;

			//Gather OpenGL info 
			glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
			glGetDoublev(GL_PROJECTION_MATRIX, projection);
			glGetIntegerv(GL_VIEWPORT, viewport);

			//Get the depth of the middle pixel for gluUnProject
			#ifndef FIX_VMWARE_PICKING_BUG
			glReadPixels((int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
			#else
			int x, y;
			x = (int)winX;
			y = (int)winY;
			float *pWinZ = new float[WidthDiv10 * HeightDiv10];
			glReadPixels(0, 0, WidthDiv10, HeightDiv10, GL_DEPTH_COMPONENT, GL_FLOAT, pWinZ);
			winZ = pWinZ[(y * WidthDiv10) + x];
			SAFE_DELETE_ARRAY(pWinZ);
			#endif

			//We are under the terrain, so will use gluUnProject to find the pixel position in 3d coordinates
			gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

			//Now, we now where the pixel is drawn, so calculate it's vector lenght from the origin
			GLdouble AbsPosX = abs(posX/* + SingleViewCamera.GetXTranslation()*/), AbsPosY = abs(posY/* + SingleViewCamera.GetYTranslation()*/), AbsPosZ = abs(posZ/* + SingleViewCamera.GetZTranslation()*/);
			pCurrentCam->ModCamDist = (float)(sqrt(POW2(AbsPosX)+POW2(AbsPosY)+POW2(AbsPosZ)) * 0.95f);
		} else {
			//We are not under the terrain, so there's no need to ajust the camera
			pCurrentCam->ModCamDist = pCurrentCam->GetDistance();
		}

	glPopMatrix();
}

void CGraphicsEngine::RenderRocks()
{
	if(!CanRenderRocks())
		return;

	float RocksAlpha = ClampColor(SkyDomeAlpha + 0.25f);
	glColor3f(RocksAlpha, RocksAlpha, RocksAlpha);

	DWORD Flags = VA_VTN;
	if(CanUseFog()){
		glEnable(GL_FOG);
		Flags |= VA_FOG;
	}

	for(int Cpt = 0; Cpt < NUM_ROCKS_MODELS; Cpt++){
		Models.Rocks[Cpt].BindPrimaryTexture();
		Models.Rocks[Cpt].Draw(Flags);
	}

	if(CanUseFog())
		glDisable(GL_FOG);

	glColor3f(1.0f, 1.0f, 1.0f);
}

void CGraphicsEngine::RenderWater(BOOL LoRes)
{
	if(!CanRenderWater())
		return;

	glEnable(GL_LIGHTING);                

	float Transparency = 0.8f;
	if(CanUseShaders())
		Transparency -= 0.1f; //We need to decrase it a little because of the shader

	float WaterAlpha = ClampColor(SkyDomeAlpha + 0.25f);
	glColor4f(WaterAlpha, WaterAlpha, WaterAlpha, Transparency);

	//Translate the texture
	if(CanUseShaders()){
		Shader.glUseProgramObjectARB(Shader.m_Program);
		Shader.glUniform1iARB(Shader.hUseLighting, 1);
		Shader.glUniform1iARB(Shader.hUseWaterWave, 1);
		Shader.glUniform4fARB(Shader.hTex0Translate, WaterTexOffset, 0.0f, 0.0f, 0.0f);
		Shader.glUniform1fARB(Shader.hWaveMovement, WaveMovement);
	} else {
		glMatrixMode(GL_TEXTURE);
		glTranslatef(WaterTexOffset, 0.0f, 0.0f);
		glMatrixMode(GL_MODELVIEW);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Models.Water[LoRes].BindPrimaryTexture();
	
	if(CanUseMultiTexture()){
		pEngine->EnableVAs(VA_AUXTEXTURE, &Models.Water[LoRes]);
		SetCurrentTextureUnit(1);
		glEnable(GL_TEXTURE_2D);

		//Set the texture combiner
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD_SIGNED);

		//Texture 1 on TU 1
		Models.Water[LoRes].SetAuxiliaryTexture(Textures.WaterReclect[(int)CurrentCaustIndx]);
		Models.Water[LoRes].BindAuxiliaryTexture();
	}

	DWORD Flags = VA_VTN;
	if(CanUseMultiTexture())
		Flags |= VA_AUXTEXTURE;
	if(CanUseFog()){
		glEnable(GL_FOG);
		Flags |= VA_FOG;
	}

	Models.Water[LoRes].Draw(Flags);
	
	if(CanUseFog())
		glDisable(GL_FOG);

	if(CanUseMultiTexture()){
		glDisable(GL_TEXTURE_2D);
		SetCurrentTextureUnit(0);
		pEngine->DisableVAs(VA_AUXTEXTURE, &Models.Water[LoRes]);
	}

	glDisable(GL_BLEND);
	
	//Set the tex env to GL_MODULATE
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if(CanUseShaders()){
		Shader.glUniform1iARB(Shader.hUseLighting, 0);
		Shader.glUniform1iARB(Shader.hUseWaterWave,  0);
		Shader.glUniform4fARB(Shader.hTex0Translate, 0.0f, 0.0f, 0.0f, 0.0f);
		Shader.glUniform1iARB(Shader.hUseWaterWave,  0);
		Shader.glUseProgramObjectARB(NULL);
	} else {
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
	}

	glColor3f(1.0f, 1.0f, 1.0f);
}

void CGraphicsEngine::RenderChessBoard()
{
	if(!CanRenderChessBoard())
		return;

	float ChessBoardAlpha = ClampColor(SkyDomeAlpha + 0.25f);
	glColor3f(ChessBoardAlpha, ChessBoardAlpha, ChessBoardAlpha);

	if(CanUseShaders())
		Shader.glUseProgramObjectARB(Shader.m_Program);

	int ReflectionIndx = (int)CurrentCaustIndx;
	DWORD Flags = VA_VT;

	if(CanUseMultiTexture() && !LiteMode)
		Flags |= VA_AUXTEXTURE;

	glDisable(GL_LIGHTING);

	Models.ChessBoard[CHESSBOARD_CASES].BindPrimaryTexture();
	if(CanUseMultiTexture() && !LiteMode){
		pEngine->EnableVAs(VA_AUXTEXTURE, &Models.ChessBoard[CHESSBOARD_CASES]);
		SetCurrentTextureUnit(1);
		glEnable(GL_TEXTURE_2D);
		//Texture 1 on TU 1
		Models.ChessBoard[CHESSBOARD_CASES].SetAuxiliaryTexture(Textures.WaterReclect[ReflectionIndx]);
		Models.ChessBoard[CHESSBOARD_CASES].BindAuxiliaryTexture();
	}
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	Models.ChessBoard[CHESSBOARD_CASES].Draw(Flags);
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	if(CanUseMultiTexture() && !LiteMode){
		glDisable(GL_TEXTURE_2D);
		SetCurrentTextureUnit(0);
		pEngine->DisableVAs(VA_AUXTEXTURE, &Models.ChessBoard[CHESSBOARD_CASES]);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	Models.ChessBoard[CHESSBOARD_CASES].BindPrimaryTexture();
	glEnable(GL_CULL_FACE);
	Models.ChessBoard[CHESSBOARD_CASES].Draw(VA_VT);
	glDisable(GL_CULL_FACE);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	Flags |= VA_NORMALS;
	glEnable(GL_LIGHTING);
	if(CanUseShaders())
		Shader.glUniform1iARB(Shader.hUseLighting, 1);

	Models.ChessBoard[CHESSBOARD_TOP].BindPrimaryTexture();
	Models.ChessBoard[CHESSBOARD_TOP].Draw(VA_VTN);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	Models.ChessBoard[CHESSBOARD_BOTTOM].BindPrimaryTexture();
	if(CanUseMultiTexture() && !LiteMode){
		pEngine->EnableVAs(VA_AUXTEXTURE, &Models.ChessBoard[CHESSBOARD_BOTTOM]);
		SetCurrentTextureUnit(1);
		glEnable(GL_TEXTURE_2D);
		//Texture 1 on TU 1
		Models.ChessBoard[CHESSBOARD_BOTTOM].SetAuxiliaryTexture(Textures.WaterReclect[ReflectionIndx]);
		Models.ChessBoard[CHESSBOARD_BOTTOM].BindAuxiliaryTexture();
	}
	Models.ChessBoard[CHESSBOARD_BOTTOM].Draw(Flags);

	if(CanUseMultiTexture() && !LiteMode){
		glDisable(GL_TEXTURE_2D);
		SetCurrentTextureUnit(0);
		pEngine->DisableVAs(VA_AUXTEXTURE, &Models.ChessBoard[CHESSBOARD_BOTTOM]);
	}

	if(CanUseShaders()){
		Shader.glUniform1iARB(Shader.hUseLighting, 0);
		Shader.glUseProgramObjectARB(NULL);
	}

	glColor3f(1.0f, 1.0f, 1.0f);
}

void CGraphicsEngine::RenderPlaneForPcxCoordinate()
{
	FBO.EnableFBO(true);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//Set3DMode(NEAR_PLANE, FAR_PLANE);
	glColor3f(1.0f, 1.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_FOG);

	//Get the cursor position, relative to window coordinate
	POINT CursorPos;
	GetCursorPos(&CursorPos);
	ScreenToClient(GethWnd(), &CursorPos);

	pCurrentCam->Set3DMode(pCurrentCam != &DualViewCamera[1] ? 0 : 1);

	glPushMatrix();

		// Set the camera position
		pCurrentCam->SetCamera();

		//Draw the plane the maximum size we can see
		static const float PlaneSize = FAR_PLANE;

		//Draw the plane
		glBegin(GL_QUADS);
			glVertex3f( PlaneSize, 0.0f,  PlaneSize);
			glVertex3f( PlaneSize, 0.0f, -PlaneSize);
			glVertex3f(-PlaneSize, 0.0f, -PlaneSize);
			glVertex3f(-PlaneSize, 0.0f,  PlaneSize);
		glEnd();

		static double PlaneX, PlaneY, PlaneZ;
		static bool RetVal;

		if(UseDualView){
			if(DualViewMode == DUAL_VIEW_MODE_H){
				if(CursorPos.y > GameWindow.GetHeight() / 2)
					CursorPos.y -= GameWindow.GetHeight() / 2;
			}
		}

		RetVal = GetMousePositionOnPlane((int)CursorPos.x, (int)CursorPos.y, &PlaneX, &PlaneY, &PlaneZ);

		if(RetVal){
			CurrentSelPos.X = (float)PlaneX;
			CurrentSelPos.Y = (float)PlaneZ;
		}

	glPopMatrix();
	FBO.EnableFBO(false);
}

void CGraphicsEngine::RenderChessPieces(BOOL LoRes)
{
	if(!CanRenderChessPieces())
		return;

	float ChessPiecesAlpha = ClampColor(SkyDomeAlpha + 0.25f);
	glColor3f(ChessPiecesAlpha, ChessPiecesAlpha, ChessPiecesAlpha);

	if(CanUseShaders()){
		Shader.glUseProgramObjectARB(Shader.m_Program);
		Shader.glUniform1iARB(Shader.hUseLighting, 1);
		Shader.glUniform4fARB(Shader.hTex0Scale, 1.0f, 1.0f, 0.0f, 0.0f);
		Shader.glUniform4fARB(Shader.hTex1Scale, 1.0f, 1.0f, 0.0f, 0.0f);
		Shader.glUniform4fARB(Shader.hTex0Translate, 0.0f, 0.0f, 0.0f, 0.0f);
		Shader.glUniform4fARB(Shader.hTex1Translate, 0.0f, 0.0f, 0.0f, 0.0f);
		Shader.glUniform1fARB(Shader.hWaveMovement, WaveMovement);
	}

	//Loop throught all the pcx
	int CptX, CptY;
	for(CptY = 0; CptY < 8; CptY++){
		for(CptX = 0; CptX < 8; CptX++){
			glPushMatrix();

				//Query info about a case, from the AI dll...
				int PcxType = 0;
				int PcxColor = 0;
				bool DrawGhost = false;
				AI.pQueryChessBoardInfo(CptX, CptY, &PcxType, &PcxColor, DrawGhost);

				//If there something here...
				if(PcxColor != 0){
					
					//Translate the pcx to where it is currently on the board
					if(!DrawGhost && CurrentSel == ((CptY<<3)+CptX))
						TRANSLATE(CurrentSelPos.X, CurrentSelPos.Y, 0);
					else
						TRANSLATE(PreCalc.PiecesPositionTable.BoardPosL[CptX], PreCalc.PiecesPositionTable.BoardPosT[CptY], 0.5f);	

					//Apply an angle rotation to our pcx 
					//glRotatef(Chessboard.ZShortedPcx[Cpt].Angle, 0.0f, 1.0f, 0.0f);
					
					//Draw the pcx
					switch((PcxColor-1))
					{
					case 0:  Models.ChessPieces[LoRes][PcxType-1].BindPrimaryTexture();   break;
					default: Models.ChessPieces[LoRes][PcxType-1].BindAuxiliaryTexture(); break;
					}
					
					Models.ChessPieces[LoRes][PcxType-1].Draw(VA_VTN);
				}

			glPopMatrix();
		}
	}

	if(CanUseShaders())
		Shader.glUseProgramObjectARB(NULL);

	glColor3f(1.0f, 1.0f, 1.0f);
}

void CGraphicsEngine::RenderChessPiecesNextMoves(BOOL LoRes)
{
	if(!CanRenderChessPiecesNextMoves())
		return;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);				
	
	float ChessPiecesAlpha = ClampColor(SkyDomeAlpha + 0.25f);
	glColor4f(ChessPiecesAlpha, ChessPiecesAlpha, ChessPiecesAlpha, 0.5f);

	if(CanUseShaders()){
		Shader.glUseProgramObjectARB(Shader.m_Program);
		Shader.glUniform1iARB(Shader.hUseLighting, 1);
		Shader.glUniform4fARB(Shader.hTex0Scale, 1.0f, 1.0f, 0.0f, 0.0f);
		Shader.glUniform4fARB(Shader.hTex1Scale, 1.0f, 1.0f, 0.0f, 0.0f);
		Shader.glUniform4fARB(Shader.hTex0Translate, 0.0f, 0.0f, 0.0f, 0.0f);
		Shader.glUniform4fARB(Shader.hTex1Translate, 0.0f, 0.0f, 0.0f, 0.0f);
		Shader.glUniform1fARB(Shader.hWaveMovement, WaveMovement);
	}

	//Loop throught all the pcx
	int CptX, CptY;
	for(CptY = 0; CptY < 8; CptY++){
		for(CptX = 0; CptX < 8; CptX++){
			glPushMatrix();

				//Query info about a case, from the AI dll...
				int PcxType = 0;
				int PcxColor = 0;
				bool DrawGhost = true;
				AI.pQueryChessBoardInfo(CptX, CptY, &PcxType, &PcxColor, DrawGhost);

				//If there something here...
				if(PcxColor != 0){
					
					//Translate the pcx to where it is currently on the board
					TRANSLATE(PreCalc.PiecesPositionTable.BoardPosL[CptX], PreCalc.PiecesPositionTable.BoardPosT[CptY], 0.5f);	

					//Apply an angle rotation to our pcx 
					//glRotatef(Chessboard.ZShortedPcx[Cpt].Angle, 0.0f, 1.0f, 0.0f);
					
					//Draw the pcx
					switch((PcxColor-1))
					{
					case 0:  Models.ChessPieces[LoRes][PcxType-1].BindPrimaryTexture();   break;
					default: Models.ChessPieces[LoRes][PcxType-1].BindAuxiliaryTexture(); break;
					}
					
					Models.ChessPieces[LoRes][PcxType-1].Draw(VA_VTN);
				}

			glPopMatrix();
		}
	}

	glDisable(GL_BLEND);

	if(CanUseShaders())
		Shader.glUseProgramObjectARB(NULL);

	glColor3f(1.0f, 1.0f, 1.0f);
}

void CGraphicsEngine::RenderReflectedPcx(BOOL LoRes)
{
	if(CanUseShaders()){
		Shader.glUseProgramObjectARB(Shader.m_Program);
		Shader.glUniform1iARB(Shader.hUseAlphaBlendingForReflection, 1);
	}

	glClear(GL_STENCIL_BUFFER_BIT);	

	if(!ShowPiecesReflection())
		return;

	//Those value will control the way the pcx will be drawn(Front to Back)
	static const int2 StartVal[4] = {{7, 0}, {7, 7}, {0, 7}, {0, 0}};
	static const int2 ValToAdd[4] = {{-1, 1}, {-1, -1}, {1, -1}, {1, 1}};

	glDisable(GL_DEPTH_TEST);

	// Clear and setup the stencil buffer
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NEVER, 1, 1);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	
	// Enable face culling
	glEnable(GL_CULL_FACE);
	// Draw only in the stencil buffer
	glColorMask(0, 0, 0, 0);
	// Draw the case model, so the reflected pcx will be rendered only where the case is drawn
	Models.ChessBoard[CHESSBOARD_CASES].Draw(VA_VERTEX);
	// Enable color buffer
	glColorMask(1, 1, 1, 1);
	// Enable face culling
	glDisable(GL_CULL_FACE);
	
	// Setup stencil buffer for the reflections
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_EQUAL, 1, 1);
	
	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);				

	// Set the pcx color and transparency value(alpha)
	float PcxCol = 0.25f + (SkyDomeAlpha * 0.75f);
	glColor4f(PcxCol, PcxCol, PcxCol, 0.33333f);
	

	//Check in wich quadran we are and set the counters accordingly
	int CptX, CptY, Angle = 3;
	Angle = 3 - (int)(SingleViewCamera.GetXRotation() / 90.0f);
	if(Angle < 0){Angle = 0;} else if(Angle > 3){Angle = 3;}
	CptX = StartVal[Angle].X;
	CptY = StartVal[Angle].Y;

	//That value will tell us when to break
	bool MustBreak = false;

	// Enable face culling
	glEnable(GL_CULL_FACE);
	//Draw only the front facing tris
	glCullFace(GL_FRONT);
	//Loop through all of the pcx in the pcx vector
	while(1){

		glPushMatrix();
			//Apply an Inverse Matrix to our Pcx
			glScalef(1.0f, -1.0f, 1.0f);

			//Query info about the current case
			int PcxType = 0;
			int PcxColor = 0;
			AI.pQueryChessBoardInfo(CptX, CptY, &PcxType, &PcxColor, FALSE);

			if(PcxColor != 0){
				
				// Translate the pcx where it suppose to be on the board
				if(CurrentSel == ((CptY*8)+CptX)){
					TRANSLATE(CurrentSelPos.X, CurrentSelPos.Y, 0);
				} else {
					TRANSLATE(PreCalc.PiecesPositionTable.BoardPosL[CptX], PreCalc.PiecesPositionTable.BoardPosT[CptY], 0);
				}
				
				// Apply an angle rotation to the pcx 
				//glRotatef(Chessboard.ZShortedPcx[Cpt].Angle, 0.0f, 1.0f, 0.0f);
				
				switch((PcxColor-1))
				{
				case 0: Models.ChessPieces[LoRes][PcxType-1].BindPrimaryTexture();   break;
				case 1: Models.ChessPieces[LoRes][PcxType-1].BindAuxiliaryTexture(); break;
				}
				
				// Draw the pcx
				Models.ChessPieces[LoRes][PcxType-1].Draw(VA_VTN);
			}

			// Control the counter...
			CptX += ValToAdd[Angle].X;
			if(CptX < 0 || CptX >= 8){
				CptX = StartVal[Angle].X;
				CptY += ValToAdd[Angle].Y;
				if(CptY < 0 || CptY >= 8)
					MustBreak = true;
			}

		glPopMatrix();

		// Break if we have finish drawing
		if(MustBreak){break;}
	}

	if(CanUseShaders()){
		Shader.glUniform1iARB(Shader.hUseAlphaBlendingForReflection, 0);
		Shader.glUseProgramObjectARB(NULL);
	}
	glColor3f(1.0f, 1.0f, 1.0f);

	// Revert face culling settings to normal
	glCullFace(GL_BACK);
	// Enable face culling
	glDisable(GL_CULL_FACE);

	// Disable blending
	glDisable(GL_BLEND);
	// Disable stenciling
	glDisable(GL_STENCIL_TEST);

	glEnable(GL_DEPTH_TEST);
}

void CGraphicsEngine::RenderChessPiecesForPlanarShadowMapping(BOOL LoRes)
{
	//Loop throught all the pcx
	int CptX, CptY;
	for(CptY = 0; CptY < 8; CptY++){
		for(CptX = 0; CptX < 8; CptX++){
			glPushMatrix();

				//Query info about a case, from the AI dll...
				int PcxType = 0;
				int PcxColor = 0;
				bool DrawGhost = false;
				AI.pQueryChessBoardInfo(CptX, CptY, &PcxType, &PcxColor, DrawGhost);

				//If there something here...
				if(PcxColor != 0){
					
					//Translate the pcx to where it is currently on the board
					if(!DrawGhost && CurrentSel == ((CptY<<3)+CptX))
						TRANSLATE(CurrentSelPos.X, CurrentSelPos.Y, 0);
					else
						TRANSLATE(PreCalc.PiecesPositionTable.BoardPosL[CptX], PreCalc.PiecesPositionTable.BoardPosT[CptY], 0.5f);	

					//Apply an angle rotation to our pcx 
					//glRotatef(Chessboard.ZShortedPcx[Cpt].Angle, 0.0f, 1.0f, 0.0f);
					
					//Draw the pcx
					Models.ChessPieces[LoRes][PcxType-1].Draw(VA_VERTEX);
				}

			glPopMatrix();
		}
	}
}

int CGraphicsEngine::RenderChessPiecesForPicking(int x, int y, BOOL LoRes)
{
	FBO.EnableFBO(true);
	// Clear depth and stencil buffers
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glColor3f(1.0f, 1.0f, 1.0f);
	glColorMask(0, 0, 0, 0);      // no "real" drawing

	// Set OpenGL for 3D Drawing
	//pEngine->Set3DMode(NEAR_PLANE, FAR_PLANE);
	pCurrentCam->Set3DMode(pCurrentCam != &DualViewCamera[1] ? 0 : 1);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	////////////////////////////////////////////////////////////////

	glEnable(GL_STENCIL_TEST);

	glPushMatrix();

		pCurrentCam->SetCam();

		int CurPcxCpt = 1;
		//Loop throught all of the pcx in the pcx vector
		for(int CptY = 0; CptY < 8; CptY++){
			for(int CptX = 0; CptX < 8; CptX++){

				int PcxType = 0;
				int PcxColor = 0;
				AI.pQueryChessBoardInfo(CptX, CptY, &PcxType, &PcxColor, FALSE);
				int Turn = AI.pGetCurrentTurn() + 1;

				bool CanDraw = PcxColor == Turn;
				if(CanDraw && GameType == GAME_TYPE_NETWORKED)
					CanDraw = ClientID == Turn;

				if(CanDraw){
					glStencilFunc(GL_ALWAYS, CurPcxCpt, 0xFFFFFFFF);
					glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

					glPushMatrix();
						// Translate the pcx to where it suppose to be on the board
						TRANSLATE(PreCalc.PiecesPositionTable.BoardPosL[CptX], PreCalc.PiecesPositionTable.BoardPosT[CptY], 0);
						//Apply an angle of rotation 
						//glRotatef(Chessboard.ZShortedPcx[Cpt].Angle, 0.0f, 1.0f, 0.0f);
						//Draw the pcx
						Models.ChessPieces[LoRes][PcxType-1].Draw(VA_VERTEX);
					glPopMatrix();
				}
				CurPcxCpt++;
			}
		}
		
	glPopMatrix();

	glDisable(GL_STENCIL_TEST);
	glColorMask(1, 1, 1, 1);

	////////////////////////////////////////////////////////

	BYTE Val = 0;
	int TrueX, TrueY, w, h, CurView;
	CurView = pCurrentCam != &DualViewCamera[1] ? 0 : 1;
	GetFrameBufferSize(&w, &h, CurView);
	GetPickingInfo(x, y, &TrueX, &TrueY, CurView);
	if(UseDualView && DualViewMode == DUAL_VIEW_MODE_H && CurView == 1)
		TrueY += h;
	#ifndef FIX_VMWARE_PICKING_BUG
	glReadPixels(TrueX, TrueY, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &Val);
	#else
	int ScrW, ScrH;
	GetScreenSize(&ScrW, &ScrH, CurView);
	BYTE *pVal = new BYTE[ScrW * ScrH];
	glReadPixels(0, 0, ScrW, ScrH, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, pVal);
	Val = pVal[(TrueY * ScrW) + TrueX];
	SAFE_DELETE_ARRAY(pVal);
	#endif

	FBO.EnableFBO(false);
	
	// Return the choosen pcx id or -1 if no pcx has been selected
	return Val - 1;
}

void CGraphicsEngine::RenderChessPiecesFor3DShadows(float *lp)
{
	for(int Cpt = 0; Cpt < NUM_CHESSPIECES; Cpt++)
		Shadows3D.BuildVertexArray(&SVM.ChessPieces[Cpt], lp);

	//Loop throught all the pcx
	int CptX, CptY;
	for(CptY = 0; CptY < 8; CptY++){
		for(CptX = 0; CptX < 8; CptX++){
			glPushMatrix();

				//Query info about a case, from the AI dll...
				int PcxType = 0;
				int PcxColor = 0;
				bool DrawGhost = false;
				AI.pQueryChessBoardInfo(CptX, CptY, &PcxType, &PcxColor, DrawGhost);

				//If there something here...
				if(PcxColor != 0){
					
					//Translate the pcx to where it is currently on the board
					if(CurrentSel == ((CptY << 3) + CptX))
						TRANSLATE(CurrentSelPos.X, CurrentSelPos.Y, 0);
					else
						TRANSLATE(PreCalc.PiecesPositionTable.BoardPosL[CptX], PreCalc.PiecesPositionTable.BoardPosT[CptY], 0.5f);	

					//Apply an angle rotation to our pcx 
					//glRotatef(Chessboard.ZShortedPcx[Cpt].Angle, 0.0f, 1.0f, 0.0f);
					
					//Draw the pcx
					Shadows3D.CastShadows(&SVM.ChessPieces[PcxType - 1], lp);
				}

			glPopMatrix();
		}
	}
}

int CGraphicsEngine::DoLensFlareTests(CE3DCamera *pCam, bool ShowLine = false)
{
	int retval = SUN_NOT_IN_FRUSTRUM;
	if(!CanRenderLensFlares())
		return retval;

	//Disable the lighting and texture
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	//Update the frustrum matrix
	pCam->LensFlare.UpdateFrustum();
	
	bool UseVMWareCode = false;
	#ifdef FIX_VMWARE_PICKING_BUG
		UseVMWareCode = true;
	#endif

	//Check if the sun position is in the frustrum
	float TmpLensFlareAlpha = 1.0f;
	if(pCam->LensFlare.PointInFrustum(SunPosition.X, SunPosition.Y, SunPosition.Z)){
		//Check if the sun position is occluded
		if(!pCam->LensFlare.IsOccluded(SunPosition.X, SunPosition.Y, SunPosition.Z, &TmpLensFlareAlpha, UseVMWareCode)){
			pCam->LensFlareAlpha = TmpLensFlareAlpha;
			glColor3f(0.0f, 1.0f, 0.0f);  //In Frustrum - Not Occluded
			retval = SUN_IN_FRUSTRUM_NOT_OCCLUDED;
		} else {
			glColor3f(1.0f, 1.0f, 0.0f); //In Frustrum - Occluded
			retval = SUN_IN_FRUSTRUM_BUT_OCCLUDED;
		}
	} else {
		// Called to update the sun proj. position
		pCam->LensFlare.IsOccluded(SunPosition.X, SunPosition.Y, SunPosition.Z, &TmpLensFlareAlpha, UseVMWareCode);
		
		glColor3f(1.0f, 0.0f, 0.0f);     //Not in frustrum
		retval = SUN_NOT_IN_FRUSTRUM;
	}

//SkipTest:
	if(ShowLine){
		//Draw a line from the center of the scene to the sun
		glBegin(GL_LINES);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(SunPosition.X, SunPosition.Y, SunPosition.Z);	
		glEnd();
	}

	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);

	return retval;
}

void CGraphicsEngine::RenderLensFlares(CE3DCamera *pCam, int CurView)
{
	if(!CanRenderLensFlares())
		return;

	//Find the center of the screen
	float cx, cy;
	pCam->CalcLensFlareScreenCenter(&cx, &cy, CurView);

	//find the sun position in 2d coordinate
	float lx, ly;
	pCam->LensFlare.GetSunProjposition(&lx, &ly);

	//Flare is rendered along a line from (lx,ly) to a
	//point opposite it across the center point
	float dx = cx + (cx - lx);
	float dy = cy + (cy - ly);
	
	if(UseDualView){
		glEnable(GL_SCISSOR_TEST);
		int w, h;
		if(pEngine->FBO.IsEnabled()){
			w = pEngine->FBO.GetTexWidth();
			h = pEngine->FBO.GetTexHeight();
		} else {
			w = pEngine->GameWindow.GetWidth();
			h = pEngine->GameWindow.GetHeight();
		}
		switch(DualViewMode)
		{
		case DUAL_VIEW_MODE_H: glScissor(0, CurView != 0 ? 0 : h / 2, w, h / 2); break;
		case DUAL_VIEW_MODE_V: glScissor(CurView == 0 ? 0: w / 2, 0, w / 2, h);  break;
		}
	}

	for(UINT Cpt = 0; Cpt < pCam->LensFlare.Flares.size(); Cpt++){
		CLensFlare::FlareStruct *pFlare = &pCam->LensFlare.Flares[Cpt];

		float px = ((1-pFlare->Dist)*lx)+(pFlare->Dist*dx);
		float py = ((1-pFlare->Dist)*ly)+(pFlare->Dist*dy);

		float AlphaFade = 1.0f;
		if(pCam->LensFlare.FadeInTimeLeft > 0.0f)
			AlphaFade = 1.0f - (pCam->LensFlare.FadeInTimeLeft * (1.0f / FADE_IN_LENGTH));
		else if(pCam->LensFlare.FadeOutTimeLeft > 0.0f)
			AlphaFade = pCam->LensFlare.FadeOutTimeLeft * (1.0f / FADE_OUT_LENGTH);

		float a = 0.8f * ((pFlare->Color.a * AlphaFade) * (pCam->LensFlareAlpha * 1.5f));
		glColor4f(pFlare->Color.r, pFlare->Color.g, pFlare->Color.b, a);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);				
		
		float P[8];
		P[0] = px - pFlare->Size;
		P[1] = py + pFlare->Size;
		P[2] = px + pFlare->Size;
		P[3] = py + pFlare->Size;
		P[4] = px + pFlare->Size;
		P[5] = py - pFlare->Size;
		P[6] = px - pFlare->Size;
		P[7] = py - pFlare->Size;

		if(pFlare->Type == RainBowHalo)
			goto SkipRainBow;

		glBindTexture(GL_TEXTURE_2D, Textures.LensFlares[pFlare->Type]);

		glPushMatrix();
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f); glVertex2f(P[0], P[1]);
				glTexCoord2f(1.0f, 0.0f); glVertex2f(P[2], P[3]);
				glTexCoord2f(1.0f, 1.0f); glVertex2f(P[4], P[5]);
				glTexCoord2f(0.0f, 1.0f); glVertex2f(P[6], P[7]);
			glEnd();
		glPopMatrix();


		SkipRainBow:
		glDisable(GL_BLEND);
	}

	if(UseDualView)
		glDisable(GL_SCISSOR_TEST);

	glColor3f(1.0f, 1.0f, 1.0f);
}

void CGraphicsEngine::LensFlareRenderer(CE3DCamera *pCam, int CurView)
{
	if(!CanRenderLensFlares())
		return;

	switch(pCam->SunVisStatus)
	{
	case SUN_NOT_IN_FRUSTRUM: 
	case SUN_IN_FRUSTRUM_BUT_OCCLUDED: 
		if(pCam->LastSunVisStatus == SUN_IN_FRUSTRUM_NOT_OCCLUDED)
			pCam->LensFlare.FadeOutTimeLeft = FADE_OUT_LENGTH - (pCam->LensFlare.FadeInTimeLeft * (FADE_OUT_LENGTH / FADE_IN_LENGTH));

		if(pCam->LensFlare.FadeOutTimeLeft > 0.0f){
			glEnable(GL_TEXTURE_2D);
			RenderLensFlares(pCam, CurView);
			glDisable(GL_TEXTURE_2D);
		}
		break;
	case SUN_IN_FRUSTRUM_NOT_OCCLUDED: 
		if(pCam->LastSunVisStatus != SUN_IN_FRUSTRUM_NOT_OCCLUDED)
			pCam->LensFlare.FadeInTimeLeft = FADE_IN_LENGTH;

		pCam->LensFlare.FadeOutTimeLeft = 0.0f;
		glEnable(GL_TEXTURE_2D);
		RenderLensFlares(pCam, CurView);
		glDisable(GL_TEXTURE_2D);
		break;
	}

	//Save the last sun visibility status
	pCam->LastSunVisStatus = pCam->SunVisStatus;
}

void CGraphicsEngine::RenderDebugInfo(CE3DCamera *pCam)
{
	bool Stretch = !FBO.IsInitialized();
	glEnable(GL_BLEND); 
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
	CQuadRect Rect(0.0f, 0.0f, 250.0f, 320.0f);
	DrawQuads(&Rect, NULL, Stretch);
	glDisable(GL_BLEND); 
	glColor3f(1.0f, 1.0f, 1.0f);

	int Indx = 0;
	float DevRatio[2] = {GameWindow.GetWidthDevRatio(), GameWindow.GetHeightDevRatio()};

	// Print selection info
	DebugInfoFont.PrintArg(0, 0, Stretch, 15.0f, 733.0f - (Indx++ * 20.0f), DevRatio[0], DevRatio[1], 1.0f, 1.0f, 0.0f, "Selection: %d", CurrentSel);
	DebugInfoFont.PrintArg(0, 0, Stretch, 15.0f, 733.0f - (Indx++ * 20.0f), DevRatio[0], DevRatio[1], 1.0f, 1.0f, 0.0f, "Sel. Pos.: (%2.2f, %2.2f)", CurrentSelPos.X, CurrentSelPos.Y);

	// Print Time Scale
	DebugInfoFont.PrintArg(0, 0, Stretch, 15.0f, 733.0f - (Indx++ * 20.0f), DevRatio[0], DevRatio[1], 1.0f, 1.0f, 0.0f, "Time Scale: %dx", TimeScale);
	// Print Time
	float Hrs = (float)((int)(GameTime / 60.0f));
	float Mins = floor(GameTime - (Hrs * 60.0f));
	float Secs = (GameTime - (int)GameTime) / 0.016f;
	DebugInfoFont.PrintArg(0, 0, Stretch, 15.0f, 733.0f - (Indx++ * 20.0f), DevRatio[0], DevRatio[1], 1.0f, 1.0f, 0.0f, "Game Time: %2.2d:%2.2d:%2.2d (%2.2f)", (int)Hrs, (int)Mins, (int)Secs, GameTime);
	
	// Skydome alpha
	DebugInfoFont.PrintArg(0, 0, Stretch, 15.0f, 733.0f - (Indx++ * 20.0f), DevRatio[0], DevRatio[1], 1.0f, 1.0f, 0.0f, "Skydome Alpha: %2.2f", SkyDomeAlpha);
	// Skydome alpha
	DebugInfoFont.PrintArg(0, 0, Stretch, 15.0f, 733.0f - (Indx++ * 20.0f), DevRatio[0], DevRatio[1], 1.0f, 1.0f, 0.0f, "LensFlares Alpha: %2.2f", pCam->LensFlareAlpha);
	// Water texture offset
	DebugInfoFont.PrintArg(0, 0, Stretch, 15.0f, 733.0f - (Indx++ * 20.0f), DevRatio[0], DevRatio[1], 1.0f, 1.0f, 0.0f, "Water Tex. Offset: %2.2f", WaterTexOffset);
	// Water texture offset
	DebugInfoFont.PrintArg(0, 0, Stretch, 15.0f, 733.0f - (Indx++ * 20.0f), DevRatio[0], DevRatio[1], 1.0f, 1.0f, 0.0f, "SkyPlane Tex. Offset: %2.2f", SkyPlaneTexOffset);

	// Camera Rotation
	DebugInfoFont.PrintArg(0, 0, Stretch, 15.0f, 733.0f - (Indx++ * 20.0f), DevRatio[0], DevRatio[1], 1.0f, 1.0f, 0.0f, "Cam. Rot.: X = %2.2f. Y = %2.2f", pCam->GetXRotation(), pCam->GetYRotation());
	// Camera Distance
	DebugInfoFont.PrintArg(0, 0, Stretch, 15.0f, 733.0f - (Indx++ * 20.0f), DevRatio[0], DevRatio[1], 1.0f, 1.0f, 0.0f, "Cam. Dist.: %2.2f", pCam->GetDistance());
	// Camera Mod. Distance
	DebugInfoFont.PrintArg(0, 0, Stretch, 15.0f, 733.0f - (Indx++ * 20.0f), DevRatio[0], DevRatio[1], 1.0f, 1.0f, 0.0f, "Cam. Mod. Dist.: %2.2f", pCam->ModCamDist);

	// Sun Position
	DebugInfoFont.PrintArg(0, 0, Stretch, 15.0f, 733.0f - (Indx++ * 20.0f), DevRatio[0], DevRatio[1], 1.0f, 1.0f, 0.0f, "Sun Position: X = %2.2f", SunPosition.X);
	DebugInfoFont.PrintArg(0, 0, Stretch, 15.0f, 733.0f - (Indx++ * 20.0f), DevRatio[0], DevRatio[1], 1.0f, 1.0f, 0.0f, "Sun Position: Y = %2.2f", SunPosition.Y);
	DebugInfoFont.PrintArg(0, 0, Stretch, 15.0f, 733.0f - (Indx++ * 20.0f), DevRatio[0], DevRatio[1], 1.0f, 1.0f, 0.0f, "Sun Position: Z = %2.2f", SunPosition.Z);
}

void CGraphicsEngine::RenderPromotionMenu()
{
	bool Stretch = !FBO.IsInitialized();
	if(IsPromotePawnDlgVisible){
		glAlphaFunc(GL_GREATER, 0.0f);
		glEnable(GL_ALPHA_TEST);
		CQuadRect Rect(256.0f, 320.0f, 512.0f, 128.0f);
		DrawTexQuads(Textures.PromotePawn[PromoteColor], &Rect, NULL, Stretch);
		glDisable(GL_ALPHA_TEST);
	}
}

void CGraphicsEngine::RenderGameMsg()
{
	bool Stretch = !FBO.IsInitialized();
	float DevRatio[2] = {GameWindow.GetWidthDevRatio(), GameWindow.GetHeightDevRatio()};
	if(ShowCheckTimeLeft > 0.0f){
		glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		CQuadRect Rect((float)PreCalc.GetWideScreenOffset() + 352.0f, 326.0f, 324.0f, 115.0f);
		DrawQuads(&Rect);
		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glColor3f(1.0f, 1.0f, 1.0f);
		GameMsgFont.PrintArg(0, 0, Stretch, (float)PreCalc.GetWideScreenOffset() + 375.0f, 430.0f, DevRatio[0], DevRatio[1], 1.0f, 1.0f, 0.0f, "%s", "Check!");
	} else if(ShowCheckMateTimeLeft > 0.0f){
		glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		CQuadRect Rect((float)PreCalc.GetWideScreenOffset() + 352.0f, 326.0f, 324.0f, 115.0f);
		DrawQuads(&Rect);
		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glColor3f(1.0f, 1.0f, 1.0f);
		GameMsgFont.PrintArg(0, 0, Stretch, (float)PreCalc.GetWideScreenOffset() + 265.0f, 430.0f, DevRatio[0], DevRatio[1], 1.0f, 1.0f, 0.0f, "%s", "CheckMate!");
	}
}

void CGraphicsEngine::RenderChatMsg()
{
	if(GameType != GAME_TYPE_NETWORKED)
		return;

	// How many chat msg are in the list?
	UINT NumChatMsg = ChatMsgList.GetNodesCount();

	float h = FBO.IsEnabled() ? FBO.GetTexHeightf() - 20.0f : GameWindow.GetHeightf() - 20.0f;
	CQuadRect Coords(20.0f, 708.0f, 200.0f, 20.0f);

	// Draw the chat msg
	if(NumChatMsg > 0){
		// Set the index to the last msg node
		UINT CurrentNode = ChatMsgList.GetCurrentNode();

		// Show them backward until we've shown at least MAX_CHAT_LINES
		UINT Cpt = MAX_CHAT_LINES;
		while(Cpt > 0){
			CChatMsgNode *pNode = ChatMsgList.GetNode(CurrentNode);

			// Get the number of lines this text will take
			UINT NumLines = ChatMsgFont.CalcNumLines(&pNode->pText[0], MaxChatWidth);
			if(NumLines > 0)
				ChatMsgFont.PrintArg(MaxChatWidth, 0, !pEngine->FBO.IsInitialized(), Coords.L, Coords.T - (((float)(Cpt) - (float)(NumLines - 1)) * CHAT_MSG_LINE_OFFSET), GameWindow.GetWidthDevRatio(), GameWindow.GetHeightDevRatio(), pNode->r, pNode->g, pNode->b, "%s", &pNode->pText[0]);	

			Cpt -= NumLines;
			if((int)Cpt <= 0 || CurrentNode == 0)
				break;
			CurrentNode--;
		}
	}

	// Disable stencil buffer
	//glDisable(GL_STENCIL_TEST);

	// If the user is editing some text, draw it too
	if(InEditingChatMsg){
		static char TextBuf[2048];
		ZeroMemory(&TextBuf[0], 2048);
		sprintf(&TextBuf[0], "Says: %s", ChatMsgBuffer.GetBuffer());
		UINT TextStart = ChatMsgFont.CalcFirstVisibleCharsNum(&TextBuf[0], MaxChatWidth);

		CQuadRect Coords(20.0f, 20.0f, 200.0f, 20.0f);
		ChatMsgFont.PrintArg(0, 0, !pEngine->FBO.IsInitialized(), Coords.L, Coords.T, GameWindow.GetWidthDevRatio(), GameWindow.GetHeightDevRatio(), 0.75f, 1.0f, 0.75f, "%s", &TextBuf[TextStart]);

		if(IsChatCursorVisible){
			glDisable(GL_TEXTURE_2D);
			UINT CursorPos = ChatMsgFont.CalcCursorOffset((BYTE*)&TextBuf[TextStart]);
			glColor3f(0.75f, 1.0f, 0.75f);
			CQuadRect CursorCoords(20.0f + (float)CursorPos, 20.0f, 3.0f, 20.0f);
			DrawQuads(&CursorCoords, NULL, !FBO.IsInitialized(), 0, 0);
			glColor3f(1.0f, 1.0f, 1.0f);
			glEnable(GL_TEXTURE_2D);
		}
	}
}

void CGraphicsEngine::RenderMenus()
{
	FBO.EnableFBO(true);
	// Clear OpenGL buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	// Set OpenGL for 2D Drawing
	if(FBO.IsEnabled()){
		int VPCoords[4] = {0, 0, FBO.GetTexWidth(), FBO.GetTexHeight()};
		Set2DMode(&VPCoords[0]);	
	} else {
		Set2DMode();	
	}
	
	// Enable texture mapping
	glEnable(GL_TEXTURE_2D);
	// Disable lightning + depth test
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);  

	bool Stretch = !FBO.IsInitialized();
	CQuadRect MainRect((float)PreCalc.GetWideScreenOffset(), 0.0f, 1024.0f, 768.0f);
	glPushMatrix();
	switch(Menus.CurrentMenu)
	{
	case MAIN_MENU:
		{
			CMainMenuPrecalcData *pc = &PreCalc.MenusPrecalcData.MainMenu;

			// Draw the main texture
			DrawTexQuads(Textures.Menus.MainMenu[1], &MainRect, NULL, Stretch);
			
			// Clear the stencil buffer
			glClearStencil(0xFFFFFFFF);
			glClear(GL_STENCIL_BUFFER_BIT);

			glDisable(GL_TEXTURE_2D);
			glEnable(GL_STENCIL_TEST);
			glColorMask(0, 0, 0, 0);

			glStencilFunc(GL_ALWAYS, 0, 1);
			glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

			// Make the mask
			CQuadRect Rect1(&pc->Mask[Menus.CurrentMenuSel][0]);
			DrawQuads(&Rect1, NULL, Stretch);
			
			glColorMask(1, 1, 1, 1);
			glEnable(GL_TEXTURE_2D);

			glStencilFunc(GL_NOTEQUAL, 0, 1);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

			// Draw the main menu texture
			DrawTexQuads(Textures.Menus.MainMenu[0], &MainRect, NULL, Stretch); 

			// Disable stencil buffer
			glDisable(GL_STENCIL_TEST);
			glClearStencil(0);

			// Make the "Resume game" text darker
			if(!AI.pIsGameStarted()){
				glDisable(GL_TEXTURE_2D);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
				CQuadRect ResumeMask(&pc->Mask[0][0]);
				DrawQuads(&ResumeMask, NULL, Stretch);
				glColor3f(1.0f, 1.0f, 1.0f);
				glDisable(GL_BLEND);
				glEnable(GL_TEXTURE_2D);
			}
			
			// Make the "Load/Save" text darker
			{
				glDisable(GL_TEXTURE_2D);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
				CQuadRect ResumeMask(&pc->Mask[4][0]);
				DrawQuads(&ResumeMask, NULL, Stretch);
				glColor3f(1.0f, 1.0f, 1.0f);
				glDisable(GL_BLEND);
				glEnable(GL_TEXTURE_2D);
			}
			
			// Draw the arrow
			CQuadRect Rect(&pc->Arrow[Menus.CurrentMenuSel][0]);
			DrawTexQuads(Textures.Menus.Arrow, &Rect, NULL, Stretch);
		}
		break;	
	case NETWORK_MENU:
		{
			CNetworkMenuPrecalcData *pc = &PreCalc.MenusPrecalcData.NetworkMenu;

			// Draw the main texture
			DrawTexQuads(Textures.Menus.NetworkMenu[1], &MainRect, NULL, Stretch);

			// Clear the stencil buffer
			glClearStencil(0xFFFFFFFF);
			glClear(GL_STENCIL_BUFFER_BIT);

			glDisable(GL_TEXTURE_2D);
			glEnable(GL_STENCIL_TEST);
			glColorMask(0, 0, 0, 0);

			glStencilFunc(GL_ALWAYS, 0, 1);
			glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

			// Make the mask #1
			CQuadRect Rect1(&pc->Mask1[Menus.CurrentMenuSel][0]);
			DrawQuads(&Rect1, NULL, Stretch);
			
			// Make the mask
			CQuadRect Rect2(&pc->Mask2[Menus.NetworkSettings.NetType][0]);
			DrawQuads(&Rect2, NULL, Stretch);

			glColorMask(1, 1, 1, 1);
			glEnable(GL_TEXTURE_2D);

			glStencilFunc(GL_NOTEQUAL, 0, 1);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

			// Draw the main menu texture
			DrawTexQuads(Textures.Menus.NetworkMenu[0], &MainRect, NULL, Stretch); 

			// Disable stencil buffer
			glDisable(GL_STENCIL_TEST);
			glClearStencil(0);

			// Render the Server/Client choice mask
			glDisable(GL_TEXTURE_2D);
			glColor3f(0.0f, 0.0f, 0.0f);
			CQuadRect Rect3(&pc->NetTypeSelSettingsMask[1 - Menus.NetworkSettingsBackup[TEMP_SETTINGS].NetType][0]);
			DrawQuads(&Rect3, NULL, Stretch); 
			glColor3f(1.0f, 1.0f, 1.0f);
			glEnable(GL_TEXTURE_2D);

			// Render text
			float IPTxtColor[3];
			if(Menus.NetworkSettings.EditingMode == EDITING_IP){
				IPTxtColor[0] = 1.0f;
				IPTxtColor[1] = 0.0f;
				IPTxtColor[2] = 0.0f;
			} else {
				IPTxtColor[0] = 1.0f;
				IPTxtColor[1] = 1.0f;
				IPTxtColor[2] = 0.0f;
			}
			
			float PortTxtColor[3];
			if(Menus.NetworkSettings.EditingMode == EDITING_PORT){
				PortTxtColor[0] = 1.0f;
				PortTxtColor[1] = 0.0f;
				PortTxtColor[2] = 0.0f;
			} else {
				PortTxtColor[0] = 1.0f;
				PortTxtColor[1] = 1.0f;
				PortTxtColor[2] = 0.0f;
			}

			NetworkMenuFont.PrintArg(0, 0, Stretch, 330.0f + (float)PreCalc.GetWideScreenOffset(), GameWindow.GetDevHeightf() - 546.0f, GameWindow.GetWidthDevRatio(), GameWindow.GetHeightDevRatio(), IPTxtColor[0], IPTxtColor[1], IPTxtColor[2], "%s", &Menus.NetworkSettings.ip[0]);
			NetworkMenuFont.PrintArg(0, 0, Stretch, 330.0f + (float)PreCalc.GetWideScreenOffset(), GameWindow.GetDevHeightf() - 634.0f, GameWindow.GetWidthDevRatio(), GameWindow.GetHeightDevRatio(), PortTxtColor[0], PortTxtColor[1], PortTxtColor[2], "%s", &Menus.NetworkSettings.Port[0]);

			if(Menus.NetworkSettings.EditingMode == EDITING_OFF){
				// Draw the arrow
				CQuadRect Rect(&pc->Arrow[Menus.CurrentMenuSel][0]);
				DrawTexQuads(Textures.Menus.Arrow, &Rect, NULL, Stretch);
			} else if(Menus.NetworkSettings.EditingMode == EDITING_CONNTYPE){
				// Draw the arrow
				CQuadRect Rect(&pc->NetTypeSelArrow[Menus.NetworkSettings.NetType][0]);
				DrawTexQuads(Textures.Menus.Arrow, &Rect, NULL, Stretch);
			}
		}
		break;	
	case OPTIONS_MENU:
		{
			COptionsMenuPrecalcData *pc = &PreCalc.MenusPrecalcData.OptionsMenu;

			// Draw the main texture
			DrawTexQuads(Textures.Menus.OptionsMenu[1], &MainRect, NULL, Stretch);

			// Clear the stencil buffer
			glClearStencil(0xFFFFFFFF);
			glClear(GL_STENCIL_BUFFER_BIT);

			glDisable(GL_TEXTURE_2D);
			glEnable(GL_STENCIL_TEST);
			glColorMask(0, 0, 0, 0);

			glStencilFunc(GL_ALWAYS, 0, 1);
			glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

			// Make the mask
			CQuadRect Rect1(&pc->Mask[Menus.CurrentMenuSel][0]);
			DrawQuads(&Rect1, NULL, Stretch);
			
			glColorMask(1, 1, 1, 1);
			glEnable(GL_TEXTURE_2D);

			glStencilFunc(GL_NOTEQUAL, 0, 1);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

			// Draw the main menu texture
			DrawTexQuads(Textures.Menus.OptionsMenu[0], &MainRect, NULL, Stretch); 

			// Disable stencil buffer
			glDisable(GL_STENCIL_TEST);
			glClearStencil(0);

			// Hides dots of unseleceted options
			bool *pOptions = &Menus.Options.ShowSky;
			for(int Cpt = 0; Cpt < pc->NumSel - 5; Cpt++){
				if(!pOptions[Cpt]){
					CQuadRect Rect(&pc->SettingsMask[Cpt][0]);
					glColor3f(0.0f, 0.0f, 0.0f);
					DrawQuads(&Rect, NULL, Stretch);
					glColor3f(1.0f, 1.0f, 1.0f);
				}
			}
			if(!Menus.Options.ShowShadows || (Menus.Options.ShowShadows && Menus.Options.ShadowMode != SHADOW_COMPLEX)){
				CQuadRect Rect(&pc->SettingsMask[pc->NumSel - 5][0]);
				glColor3f(0.0f, 0.0f, 0.0f);
				DrawQuads(&Rect, NULL, Stretch);
				glColor3f(1.0f, 1.0f, 1.0f);
			}
			if(!Menus.Options.ShowShadows || (Menus.Options.ShowShadows && Menus.Options.ShadowMode != SHADOW_SIMPLE)){
				CQuadRect Rect(&pc->SettingsMask[pc->NumSel - 4][0]);
				glColor3f(0.0f, 0.0f, 0.0f);
				DrawQuads(&Rect, NULL, Stretch);
				glColor3f(1.0f, 1.0f, 1.0f);
			}
			if(Menus.Options.ShowShadows){
				CQuadRect Rect(&pc->SettingsMask[pc->NumSel - 3][0]);
				glColor3f(0.0f, 0.0f, 0.0f);
				DrawQuads(&Rect, NULL, Stretch);
				glColor3f(1.0f, 1.0f, 1.0f);
			}

			CQuadRect Rect(&pc->Arrow[Menus.CurrentMenuSel][0]);
			// Draw the arrow
			DrawTexQuads(Textures.Menus.Arrow, &Rect, NULL, Stretch);
		}
		break;	
	case CONNECTING_MENU:
		{
			if(Menus.ConnectingOptions.IsConnected && Menus.ConnectingOptions.IsLogedIn && !Menus.ConnectingOptions.IsOpponentReady){
				DrawTexQuads(Textures.Menus.ConnectMenu[1], 0.25f, 0.41536425f, 0.5f, 0.083333f, NULL, Stretch);
				DrawTexQuads(Textures.Menus.ConnectMenu[2], 0.25f, 0.5f, 0.5f, 0.083333f, NULL, Stretch);
			} else if(!Menus.ConnectingOptions.IsConnected || !Menus.ConnectingOptions.IsLogedIn){
				DrawTexQuads(Textures.Menus.ConnectMenu[0], 0.25f, 0.458333f, 0.5f, 0.083333f, NULL, Stretch);
			}
		}
		break;	
	case AI_SETUP_MENU:
		{
			CAISetupMenuPrecalcData *pc = &PreCalc.MenusPrecalcData.AISetupMenu;
			// Draw the main texture
			DrawTexQuads(Textures.Menus.AISetupMenu[1], &MainRect, NULL, Stretch);

			// Clear the stencil buffer
			glClearStencil(0xFFFFFFFF);
			glClear(GL_STENCIL_BUFFER_BIT);

			glDisable(GL_TEXTURE_2D);
			glEnable(GL_STENCIL_TEST);
			glColorMask(0, 0, 0, 0);

			glStencilFunc(GL_ALWAYS, 0, 1);
			glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

			// Make the mask
			CQuadRect Rect1(&pc->Mask[Menus.CurrentMenuSel][0]);
			DrawQuads(&Rect1, NULL, Stretch);
			
			glColorMask(1, 1, 1, 1);
			glEnable(GL_TEXTURE_2D);

			glStencilFunc(GL_NOTEQUAL, 0, 1);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

			// Draw the main menu texture
			DrawTexQuads(Textures.Menus.AISetupMenu[0], &MainRect, NULL, Stretch); 

			// Disable stencil buffer
			glDisable(GL_STENCIL_TEST);
			glClearStencil(0);


			// Draw the arrow
			CQuadRect Rect(&pc->Arrow[Menus.CurrentMenuSel][0]);
			DrawTexQuads(Textures.Menus.Arrow, &Rect, NULL, Stretch);
		}
		break;	
	}
	glPopMatrix();

	// Draw some debug info
	if(FBO.IsEnabled()){
		int VPCoords[4] = {0, 0, FBO.GetTexWidth(), FBO.GetTexHeight()};
		DrawFPS(!FBO.IsInitialized(), &VPCoords[0]);	
	} else {
		DrawFPS(!FBO.IsInitialized(), NULL);	
	}
	
	if(FBO.IsEnabled()){
		FBO.EnableFBO(false);
		FBO.DrawFBO(GameWindow.GetWidth(), GameWindow.GetHeight());
	}

	// Swap OpenGL buffers
	SwapBuffers(GethDC());
}

void CGraphicsEngine::RenderScene()
{
	// Make it easyer on the gpu when testing multiplayer mode with 2 windows
	#ifdef DEBUG
	if(GethWnd() != GetActiveWindow())
		Sleep(50);
	#endif

	// Render menus... 
	if(Menus.IsMenuVisible()){
		RenderMenus();
		return;
	}

	FBO.EnableFBO(true);
	// Clear OpenGL buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Select camera
	int CurView = 0;
	CE3DCamera *pCam = NULL;
	DrawNextCam:
	!UseDualView ? pCam = &SingleViewCamera : pCam = &DualViewCamera[CurView];

	// Reset the current color
	glColor3f(1.0f, 1.0f, 1.0f);
	// Set OpenGL for 3D Drawing
	pCam->Set3DMode(CurView);

	// Initialize shaders
	if(CanUseShaders()){
		Shader.glUseProgramObjectARB(Shader.m_Program);
		Shader.glUniform3fARB(Shader.hLightPos0, 1.0f, 1000.0f, 0.0f);
		Shader.glUniform4fARB(Shader.hTex0Scale, 1.0f, 1.0f, 0.0f, 0.0f);
		Shader.glUniform4fARB(Shader.hTex1Scale, 1.0f, 1.0f, 0.0f, 0.0f);
		Shader.glUniform4fARB(Shader.hTex0Translate, 0.0f, 0.0f, 0.0f, 0.0f);
		Shader.glUniform4fARB(Shader.hTex1Translate, 0.0f, 0.0f, 0.0f, 0.0f);
		Shader.glUniform1iARB(Shader.hUsePerVertexFog, 0);
		Shader.glUniform1iARB(Shader.hUseWaterWave, 0);
		Shader.glUniform1iARB(Shader.hUseLighting, 0);
		Shader.glUniform1iARB(Shader.hWaveMovement, 0);
		Shader.glUniform1iARB(Shader.hUseAlphaBlendingForReflection, 0);
		Shader.glUseProgramObjectARB(Shader.m_Program);
		Shader.glUseProgramObjectARB(NULL);
	}

	/*********************************************************************************************/

	glPushMatrix();
		// Setup our camera
		pCam->SetCam();

		// Enable texture mapping
		glEnable(GL_TEXTURE_2D);

		// Line hack
		if(ForceLinesRendering)
			SetPolygonMode(GL_LINE); 

		if(!LiteMode){
			// Disable lightning + depth test
			glDisable(GL_LIGHTING);
			glDisable(GL_DEPTH_TEST);

			// Draw the sky
			RenderSkyDome();
			RenderSkyPlane();

			// Enable lightning + depth test
			glEnable(GL_LIGHTING);
			glEnable(GL_DEPTH_TEST); 

			// Draw solid objects
			RenderTerrain(UseLodModels());
			RenderRocks();
			RenderColumn();
			// Render chess board
			RenderChessBoard();
			// Render chess pieces
			RenderReflectedPcx(UseLodModels());
			RenderChessPieces(UseLodModels());
			RenderChessPiecesNextMoves(UseLodModels());
			// Draw transparent objects
			RenderWater(UseLodModels());
		
			// Check the sun visibility status
			pCam->SunVisStatus = DoLensFlareTests(pCam);

			// Generate the shadows effects
			if(Menus.Options.ShowShadows){
				if(Menus.Options.ShadowMode == SHADOW_SIMPLE){
					CastPlanarShadow();
				} else {
					Cast3DShadows(pCam);
				}
			}
		} else {
			// Enable lightning + depth test
			glEnable(GL_LIGHTING);
			glEnable(GL_DEPTH_TEST); 
		
			// Render chess board + chess pieces
			RenderChessBoard();
			RenderChessPieces(UseLodModels());
		}

		if(ForceLinesRendering)
			SetPolygonMode(GL_FILL); 

	glPopMatrix();
	
	/*********************************************************************************************/

	FBO.EnableFBO(true);
	// Set OpenGL for 2D Drawing
	if(FBO.IsEnabled()){
		int VPCoords[4] = {0, 0, FBO.GetTexWidth(), FBO.GetTexHeight()};
		Set2DMode(&VPCoords[0]);	
	} else {
		Set2DMode();	
	}
	
	// Enable texture mapping
	glEnable(GL_TEXTURE_2D);
	// Disable lightning + depth test
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);  

	if(!LiteMode){
		// Render planar shadows
		RenderShadowMap();
		// Draw the lens flares
		LensFlareRenderer(pCam, CurView);
	}

	if(UseDualView && CurView == 0){
		CurView++;
		goto DrawNextCam;
	}

	if(ShowDebugInfo/* && CurView == 0*/){
		// Draw some debug info
		RenderDebugInfo(pCam);
	}

	RenderChatMsg();
	RenderPromotionMenu();
	RenderGameMsg();

	// Render Frame per seconds
	if(FBO.IsEnabled()/* && CurView == 0*/){
		int VPCoords[4] = {0, 0, FBO.GetTexWidth(), FBO.GetTexHeight()};
		DrawFPS(!FBO.IsInitialized(), &VPCoords[0]);	
	} else {
		DrawFPS(!FBO.IsInitialized(), NULL);	
	}
	
	if(FBO.IsEnabled()){
		FBO.EnableFBO(false);
		FBO.DrawFBO(GameWindow.GetWidth(), GameWindow.GetHeight());
	}

	/*********************************************************************************************/

	// Swap OpenGL buffers
	SwapBuffers(GethDC());
}

void CGraphicsEngine::UpdateScene(float ElapsedTime)
{
	// No need to update anything in menus...
	if(Menus.IsMenuVisible())
		return;

	// This is the clock of the game
	TimeStuff(&GameTime, ElapsedTime, TimeScale, 1.0f, 1440.0f);

	// Show the check or checkmate message 
	if(ShowCheckTimeLeft > 0.0f){
		ShowCheckTimeLeft -= ElapsedTime;
		if(ShowCheckTimeLeft <= 0.0f){
			ShowCheckTimeLeft = 0.0f;
		}
	}
	if(ShowCheckMateTimeLeft > 0.0f){
		ShowCheckMateTimeLeft -= ElapsedTime;
		if(ShowCheckMateTimeLeft <= 0.0f){
			ShowCheckMateTimeLeft = 0.0f;
		}
	}

	// Control the cpu move translation
	if(MoveChessPieceTimeLeft > 0.0f){
		MoveChessPieceTimeLeft -= ElapsedTime;
		//Move the CPU pcx
		if(MoveChessPieceTimeLeft > 0.0f){
			CurrentSelPos.X = PreCalc.PiecesPositionTable.BoardPosL[CurrentSel % 8] + ((MoveChessPieceOffset.X * SPACE_BETWEEN_PCX) * (MoveChessPieceTimeLeft / PCX_MOVE_TRANSITION_LENGTH));
			CurrentSelPos.Y = PreCalc.PiecesPositionTable.BoardPosT[CurrentSel / 8] + ((MoveChessPieceOffset.Y * SPACE_BETWEEN_PCX) * (MoveChessPieceTimeLeft / PCX_MOVE_TRANSITION_LENGTH));
		} else {
			CurrentSel = NO_PCX_SELECTED;
			CurrentSelPos.X = 0.0f;
			CurrentSelPos.Y = 0.0f;
			MoveChessPieceTimeLeft = 0.0f;
			MoveChessPieceOffset.X = 0;
			MoveChessPieceOffset.Y = 0;
		}
	}

	// Skip the remaining code if we're in lite mode
	if(LiteMode)
		return;

	// Update the cloud translation, and generate new clouds if needed...
	SkyPlaneTexOffset += ElapsedTime * TimeScale;
	while(SkyPlaneTexOffset >= SKYPLANE_TRANSITION_LENGTH){
		SkyPlaneTexOffset -= SKYPLANE_TRANSITION_LENGTH;

		CloudsNoise.MakeImage();	
		CloudsNoise.BindTexureToOpenGL(&Textures.SkyPlane[0], &Textures.SkyPlane[1]);
	}

	// Control the lens flares fade in/out (on the 3 camera simultanously)
	for(int CptLensFlares = 0; CptLensFlares < 3; CptLensFlares++){
		CE3DCamera *pCam = NULL;
		switch(CptLensFlares)
		{
		case 0: pCam = &SingleViewCamera;  break;
		case 1: pCam = &DualViewCamera[0]; break;
		case 2: pCam = &DualViewCamera[1]; break;
		}
		
		pCam->TimeLensFlaresFadeEffect(ElapsedTime, &pCam->LensFlare.FadeInTimeLeft,  &pCam->LensFlareAlpha);
		pCam->TimeLensFlaresFadeEffect(ElapsedTime, &pCam->LensFlare.FadeOutTimeLeft, &pCam->LensFlareAlpha);
	}

	// Update the water primary texture translation effect
	TimeStuff(&WaterTexOffset,   ElapsedTime, TimeScale, 0.01f, 1.0f);
	// Update the water secondary texture to display
	TimeStuff(&CurrentCaustIndx, ElapsedTime, TimeScale, 16.0f, 32.0f);
	// Update the water wave effect
	if(CanUseShaders())
		pEngine->TimeStuff(&WaveMovement, ElapsedTime, TimeScale, 1.0f, TWO_PI);

	// Set the shadow map alpha value
	SetShadowMapAlpha();
	// Set the skydome alpha value
	SetSkyDomeAlpha();
	// Update the sun position in the sky
	UpdateSunPos();
	// Update fog transparency
	UpdateFog();
}