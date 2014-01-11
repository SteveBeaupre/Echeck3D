#ifndef _ECHECK3D_ENGINE_H_
#define _ECHECK3D_ENGINE_H_
#ifdef __cplusplus

#pragma comment(lib, "Vortez3DEngine.lib")	 
#pragma comment(lib, "VortezNetworkEngine.lib")	 
#pragma comment(lib, "VortezSoundEngine.lib")	 

/////////////////////////////////////////////////////////////////////////////////////

#include "Vortez3DEngine.h"
#include "VortezNetworkEngine.h"
#include "VortezSoundEngine.h"

#include "Menus.h"
#include "CloudsNoise.h"
#include "LensFlare.h"
#include "3DShadowCaster.h"
#include "ChatMsgBuffer.h"
#include "ChatMsgList.h"

#include "SafeKill.h"

//#define FIX_VMWARE_PICKING_BUG 
//#define RUN_WINDOWED
#ifdef DEBUG
	//#define NO_FBO
	//#define LITE_MODE
	//#define SMALL_GAME_WINDOW
#endif

#define WM_AI_SHOWCURSOR         WM_USER + 105
#define WM_AI_SHOWPROMOTEDLG     WM_USER + 106
#define WM_AI_CPU_CHECK          WM_USER + 107
#define WM_DO_CPU_MOVE           WM_USER + 108
#define WM_AI_GAME_MSG           WM_USER + 109

#define GAME_TYPE_LOCAL       0
#define GAME_TYPE_NETWORKED   1

#define NEAR_PLANE   10.0f
#define FAR_PLANE    50000.0f

#define POW2(x) ((x)*(x))
#define TWO_PI  6.2831853f

#define MOVE_PUT_KING_IN_CHESS  -1
#define INVALID_CMD              0
#define VALID_CMD                1
#define SHOW_PROMOTE_PCX_DLG     2

#define SKYPLANE_TRANSITION_LENGTH    200.0f
#define SKYDOME_TEXTURE_WIDTH         1440

#define SUN_RADIUS   20000.0f
#define SUN_OFFSET   2500.0f
#define SUN_OFFSET   2500.0f

#define TRANSLATE(c, r, z)   glTranslatef((c), (z), (r))
#define PCX_MOVE_TRANSITION_LENGTH    0.5f
#define NO_PCX_SELECTED               -1

#define SUN_NOT_IN_FRUSTRUM           0
#define SUN_IN_FRUSTRUM_BUT_OCCLUDED  1
#define SUN_IN_FRUSTRUM_NOT_OCCLUDED  2

#define NET_MOVE_MSG          1
#define NET_UPDATE_MSG        2
#define NET_CHAT_MSG          3

#define NUM_LOD_LEVEL          2
#define NUM_CHESSPIECES        6
#define NUM_CHESSPIECES_COLORS 2
#define NUM_CHESSBOARD_PARTS   3
#define NUM_WATER_REFLECTIONS  32
#define NUM_LENS_FLARES_TYPE   6
#define NUM_ROCKS_MODELS       8
#define NUM_ROCKS_TEXTURES     4

#define DUAL_VIEW_MODE_H       0
#define DUAL_VIEW_MODE_V       1

#define HI_RES  0
#define LO_RES  1

#define WHITE   0
#define BLACK   1

#define MSG_MOVE_PIECE   1

#define PAWN    0
#define ROOK    1
#define KNIGHT  2
#define BISHOP  3
#define QUEEN   4
#define KING    5

#define CHESSBOARD_CASES   0
#define CHESSBOARD_FRAME   1
#define CHESSBOARD_TOP     1
#define CHESSBOARD_BOTTOM  2

#define MAX_CHAT_LINES        10
#define CHAT_MSG_LINE_OFFSET  15

struct glObjectStruct {
	glObject StaticObjs;
	glObject ChessPieces[NUM_CHESSPIECES];	     
};

class CE3DShadowCaster  : public C3DShadowCaster {
public:
	bool OnCreateVBO(glObject *o);
	void OnBindVBO(glObject *o);
	void OnDeleteVBO(glObject *o);
};

class CE3DLog : public CTxtFileIO {
public:
	void LogArgs(const char *fmt, ...);
};

class CEcheck3DShader : public CShader {
public:
	UINT hLightPos0;
	UINT hTex0Scale, hTex1Scale;
	UINT hTex0Translate, hTex1Translate;
	UINT hUsePerVertexFog;
	UINT hUseWaterWave;
	UINT hUseLighting;
	UINT hWaveMovement;
	UINT hUseAlphaBlendingForReflection;
public:
	void InitShaderDataExt();
};

struct GameSoundsFXStruct {
	CSoundFX Drop[2];
	CSoundFX IllegalDrop;
	CSoundFX MenusChange;
};

struct MenusTexturesStruct {
	UINT Arrow;
	UINT MainMenu[2];
	UINT NetworkMenu[2];
	UINT OptionsMenu[2];
	UINT ConnectMenu[4];
	UINT AISetupMenu[2];
};

struct GameTexturesStruct {
	UINT ChessPieces[NUM_CHESSPIECES_COLORS];
	UINT Column;
	UINT ChessBoard[NUM_CHESSBOARD_PARTS-1];
	UINT Terrain;
	UINT Rocks[NUM_ROCKS_TEXTURES];
	UINT Water;
	UINT WaterReclect[NUM_WATER_REFLECTIONS];
	UINT SkyDome;
	UINT SkyPlane[2];
	UINT LensFlares[NUM_LENS_FLARES_TYPE];
	UINT LoadScreen[2];
	UINT PromotePawn[2];
	MenusTexturesStruct Menus;
};

struct GameModelsStruct {
	CModel ChessPieces[NUM_LOD_LEVEL][NUM_CHESSPIECES];
	CModel Column;
	CModel ChessBoard[NUM_CHESSBOARD_PARTS];
	CModel Terrain[NUM_LOD_LEVEL];
	CModel Rocks[NUM_ROCKS_MODELS];
	CModel Water[NUM_LOD_LEVEL];
	CModel SkyDome;
	CModel SkyPlane;
};

class CEcheck3DAI {
public:
	//Pointers to functions of the AI Dll
	int  (*pProcessAICmd)(char *movestr);
	void (*pInit)();
	char* (*pGameStateCheck)();
	void (*pQueryChessBoardInfo)(int x, int y, int *PcxType, int *PcxColor, BOOL Ghost);
	void (*pSetChessBoardInfo)(int x, int y, int PcxType, int PcxColor);
	void (*pGenGhostBoard)(int From);
	void (*pSetHostAppHandlePointers)(HWND lhWnd);
	BOOL (*pIsAIThreadRunning)();
	int  (*pGetCurrentTurn)();
	BOOL (*pLoadGame)(char *pFileName);
	void (*pSaveGame)(char *pFileName);
	void (*pFinishAIThreadJob)();
	BOOL (*pIsGameStarted)();
};

///////////////////////////////////////////////////////
// The Camera class (derived from CCamera)
///////////////////////////////////////////////////////
class CE3DCamera : public CCamera {
private:
	class CViewPort {
	private:
		int Left;
		int Width;
		int Top;
		int Height;
	public:
		int GetLeft(){return Left;}
		int GetWidth(){return Width;}
		int GetTop(){return Top;}
		int GetHeight(){return Height;}
		
		int GetRight(){return Left + Width;}
		int GetBottom(){return Top + Height;}

		float GetRatio(){return (float)GetWidth() / (float)GetHeight();}
		
		void SetVP(int l, int w, int t, int h){Left = l; Width = w; Top = t; Height = h;}
		void SetCam();
	};
public:
	void InitializeE3DCamera(){
		//InitRootZoomNode();
		ModCamDist = 0.0f;
		LensFlareAlpha = 0.0f;
		SunVisStatus = SUN_NOT_IN_FRUSTRUM;
		LastSunVisStatus = SunVisStatus;
	}
public:
	CViewPort vp;
public:
	float ModCamDist;
public:
	void  Set3DMode();
	void  Set3DMode(int Loop);
	void  SetCam();
	float GetCamOffset();
public:
	CLensFlare LensFlare;
	float      LensFlareAlpha;
	void CalcLensFlareScreenCenter(float *cx, float *cy, int CurView);
public:
	int SunVisStatus;
	int LastSunVisStatus;
public:
	void TimeLensFlaresFadeEffect(float ElapsedTime, float *pTimeLeft, float *pLensFlareAlpha);
};

class CE3DMenus : public CMenus {
public:
	void OnMenuDoServerStartListening();
	void OnMenuDoConnectToServer();
	void OnMenuDoDisconect();
	void OnMenuDoAISetup(int Sel);
	bool OnMenuIsGameStarted();
	void OnMenuDoExit();
	void PlayMenusChangeSoundFX();
	void OnLoadGameSettings();
	void OnSaveGameSettings();
};

class CNetServer : public CVortezNetworkServer {
public:
	void OnConnectionAccepted(CSocketNode *pNode);
	void OnConnectionRefused(UINT Reason);
	void OnConnectionClosed(CSocketNode *pNode);

	void OnClientLogedIn(CSocketNode *pNode);
	void OnClientLoginFailed(CSocketNode *pNode, UINT Err);

	void OnEveryoneLoged();

	void OnChatMsg(UINT PlayerID, char *pPlayerName, char *pTextData);
	void OnCustomNetworkMsgRecv(CSocketNode *pNode, CNetworkMsg *pMsg);

	void OnServerDisconnected(SOCKET s);	
	void OnClientDisconnected(CSocketNode *pNode);
};

class CNetClient : public CVortezNetworkClient {
public:
	void OnConnectionEstablished(SOCKET s);
	void OnConnectionLoss(SOCKET s, UINT ErrCode);
	
	void OnLoginSuccess(SOCKET s, UINT PlayerID);
	void OnLoginFailed(SOCKET s, UINT Err);

	void OnEveryoneReady();

	void OnChatMsg(UINT PlayerID, char *pPlayerName, char *pTextData);
	void OnCustomNetworkMsgRecv(SOCKET s, CNetworkMsg *pMsg);
	
	void SendMoveMsg(char *Move);

	void OnDisconnect(SOCKET s);	
};


//
class CEcheck3DEngine : public CVortez3DEngine {
public:
	CEcheck3DEngine();
	~CEcheck3DEngine();
public:
	bool IsGameEngineInitialized;
public:
	CFBO FBO;
	CFBO LoadingFBO;
public:
	CE3DLog *pLog;
	bool ShowDebugInfo;
public:
	CEcheck3DShader Shader;
public:
	UINT GameType;
	DWORD ClientID;
	CNetClient NetClient;
	CNetServer NetServer;
public:
	CPreCalc PreCalc;
public:
	float MoveChessPieceTimeLeft;
	int2  MoveChessPieceOffset;
public:
	bool LiteMode;
	bool ForceLinesRendering;
	UINT GameState;
	CE3DMenus Menus;
public: // Hide/Show Debug info
	CE3DCamera SingleViewCamera;
	CE3DCamera DualViewCamera[2];
	CE3DCamera *pCurrentCam;
	bool UseDualView;
	int  DualViewMode;
	void InitCameras();
	void SetDualViewMode(int Mode);
	void ResetCameraPosition(CE3DCamera *pCam);
	void SelectCamera(int x, int y);
public:
	int   TimeScale;
	float GameTime;
	float SkyDomeAlpha;
	float ShadowMapAlpha;
	float WaterTexOffset;
	float WaveMovement;
	float CurrentCaustIndx;
	float SkyPlaneTexOffset;
	BYTE  SkyDomeColor[2][SKYDOME_TEXTURE_WIDTH * 3];
	bool  LoadSkyDomePrimaryTexture(char *fname);
public:
	float3 SunPosition;
public:
	CFreeTypeFonts DebugInfoFont;
	CFreeTypeFonts NetworkMenuFont;
	CFreeTypeFonts GameMsgFont;
	CFreeTypeFonts ChatMsgFont;
public:
	UINT MaxChatWidth;
	//UINT CurrentTrackBarSelPart;
	bool InEditingChatMsg;
	bool IsChatCursorVisible;
	CClientChatMsgNode* pChatMsgMemNode;
	CChatMsgList   ChatMsgList;
	CChatMsgBuffer ChatMsgBuffer;
public:
	bool IsPromotePawnDlgVisible;
	UINT PromoteColor;
	BYTE PromotionMove[8];
	float ShowCheckTimeLeft;
	float ShowCheckMateTimeLeft;
public:
	CEcheck3DAI AI;
	bool LoadAIDLL();
	void FreeAIDLL();
public:
	void ResetGame();
public:
	GameModelsStruct Models;
	GameTexturesStruct Textures;
	GameSoundsFXStruct SoundsFX;
public:
	CCloudsNoise CloudsNoise;
public:
	bool CanUseMultiTexture(){return IsMultitextureExtSupported();}
public:
	bool LoadSoundsFX();
	void FreeSoundsFX();
public:
	bool LoadGameTextures();
	void SetGameTextures();
	void FreeGameTextures();
	bool LoadGameModels();
	void FreeGameModels();
public:
	void SetGameTime(int h, int m);
	void SetupFog();
	void SetSkyDomeAlpha();
	void SetShadowMapAlpha();
	void UpdateSunPos();
	void UpdateFog();
	bool CenterPcxOnBoard(int *pPosX, int *pPosY);
	void CalcCoordsForPicking(int x, int y, int *px, int *py, int *pw, int *ph);
	bool GetMousePositionOnPlane(int x, int y, double *ObjX, double *ObjY, double *ObjZ);
public:
	void GetScreenSize(int *w, int *h, int CurView);
	void GetFrameBufferSize(int *w, int *h, int CurView);
	void GetPickingInfo(int InX, int InY, int *OutX, int *OutY, int CurView);
public:
	float ShadowMatrix[16];
	void  GenerateShadowMatrix(float *pShadowMatrix);
public:
	CE3DShadowCaster Shadows3D;
	glObjectStruct   SVM;
public:
	float ClampColor(float color);
public:
	void InitEngineExt();
	void ShutdownEngineExt();
public:
	void OnWinsockMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnNetworkMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnCustomNetworkMsgRecv(SOCKET s, CNetworkMsg *pMsg);
public:
	void OnCustomMessageExt(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void OnChar(UINT KeyPressed);
	void OnKeyDown(UINT KeyPressed);

	int  ProcessMouseMoveMsgForPromotionMenu(float x, float y);
	void OnMouseDown(int ButtonPressed, int x, int y);
	void OnMouseUp(int ButtonPressed, int x, int y);
	void OnMouseMoveExt(int x, int y, int XDisp, int YDisp);
	void OnMouseRollExt(int RollCount);

	void OnInitializeOpenGLForLoadingExt();
	void OnCleanUpOpenGLForLoadingExt();
	void RenderLoadingExt(int PercentDone);
public:
	/////////AI Stuff//////////
	HINSTANCE hAIDll;	
	int    CurrentSel;
	float2 CurrentSelPos;
public:
	virtual void RenderLoadScreen(int PercentDone){}
	virtual void RenderTerrainForCamCollision(BOOL LoRes){}
	virtual int  RenderChessPiecesForPicking(int x, int y, BOOL LoRes){return -1;}
	virtual void RenderPlaneForPcxCoordinate(){}
};

#endif
#endif //_ECHECK3D_ENGINE_H_/*