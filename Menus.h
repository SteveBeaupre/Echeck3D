#ifndef _CMENUS_H_
#define _CMENUS_H_
#ifdef __cplusplus

#include <Windows.h>
#include "Stdio.h"
#include "SafeKill.h"

#include <gl\gl.h>			
#include <gl\glu.h>	

#include "PreCalc.h"

/*#include <fmod.h>

#include "RegLib.h"*/

#define MAIN_MENU            0
#define OPTIONS_MENU         1
#define NETWORK_MENU         2
#define CONNECTING_MENU      3
#define AI_SETUP_MENU        4
//#define GAME_SETUP_MENU      1

#define GAME_STATE_IN_MENU   0
#define GAME_STATE_PLAYING   1
#define GAME_STATE_OVER_W    2
#define GAME_STATE_OVER_L    3

#define EDITING_OFF          0
#define EDITING_CONNTYPE     1
#define EDITING_IP           2
#define EDITING_PORT         3
//#define EDITING_PASS         4

#define NET_SERVER   0
#define NET_CLIENT   1

#define CONN_STATE_CONNECTING   0
#define CONN_STATE_CONNECTED    1
#define CONN_STATE_CONNFAILED   2
#define LOGIN_STATE_LOGININ     0
#define LOGIN_STATE_LOGEDIN     1
#define LOGIN_STATE_LOGFAILED   2

#define BACKUP_SETTINGS    0
#define TEMP_SETTINGS      1

#define SHADOW_COMPLEX   0
#define SHADOW_SIMPLE    1

//#define GAME_REGISTRY_KEY             "SOFTWARE\\Skip-Bo"
//#define GAME_SETTINGS_REGISTRY_KEY    "SOFTWARE\\Skip-Bo\\Client\\GameSettings"
//#define NETWORK_SETTINGS_REGISTRY_KEY "SOFTWARE\\Skip-Bo\\Client\\NetWorkSettings"

struct CNetWorkSettings {
	UINT EditingMode;
	UINT NetType;
	char Name[16];
	char ip[16];
	char Port[8];
	char Pass[16];
};

struct COptionsSettings {
	bool ShowSky;
	bool ShowClouds;
	bool ShowTerrain;
	bool ShowRocks;
	bool ShowWater;
	bool ShowChessboard;
	bool ShowColumn;
	bool ShowChessPieces;
	//////////////////////
	bool ShowFog;
	bool UseShader;
	bool ShowReflections;
	bool ShowLensFlares;
	bool UseLODModels;
	bool ShowNextMoves;
	bool ShowShadows;
	UINT ShadowMode;
};


struct CConnectingOptions {
	bool IsConnected;
	bool IsLogedIn;
	bool IsOpponentReady;
};

bool IsInSquare(float x, float y, float l, float r, float t, float b, float DevRatio, float ScrLeft);
void InvBool(bool *b1);

class CMenus {
public:
	CMenus();
private:
	UINT *pGameState;
	CPreCalc *pPreCalc;
public:
	int  CurrentMenu;
	int  CurrentMenuSel;
public:
	bool  CursorVisible;
	float CursorAnimTimer;
public:
	CNetWorkSettings NetworkSettings;
	CNetWorkSettings NetworkSettingsBackup[2];
	COptionsSettings Options;
	COptionsSettings OptionsBackup;
	CConnectingOptions ConnectingOptions;
public:
	void SaveNetworkSettings(int Dest);
	void RestoreNetworkSettings(int Dest);
	void SaveOptionsSettings();
	void RestoreOptionsSettings();
public:
	void SetGameStatePtr(UINT *lpGameState){pGameState = lpGameState;}
	void SetPreCalcPtr(CPreCalc *pPreCalcClass){pPreCalc = pPreCalcClass;}
public:
	bool IsMenuVisible(){return *pGameState == GAME_STATE_IN_MENU;}
	void HideMenus();
	void ShowMainMenu();
	void ShowNetworkMenu();
	void ShowOptionsMenu();
	void ShowConnectingMenu();
	void ShowAISetupMenu();
public:
	void LoadGameSettings();
	void SaveGameSettings();
public:
	void ProcessKeyDownMsg(UINT KeyPressed);
	void ProcessCharMsg(UINT KeyPressed);
	void ProcessMouseMoveMsg(int x, int y, float WidthRatio, float HeightRatio, bool IsWideScreen);
	void ProcessMouseClickMsg(int x, int y, float WidthRatio, float HeightRatio, bool IsWideScreen);
public:
	virtual void OnMenuDoServerStartListening(){}
	virtual void OnMenuDoConnectToServer(){}
	virtual void OnMenuDoDisconect(){}

	virtual void OnMenuDoAISetup(int Sel){}

	virtual bool OnMenuIsGameStarted(){return true;}

	virtual void OnMenuDoExit(){}

	virtual void PlayMenusChangeSoundFX(){}

	virtual void OnLoadGameSettings(){}
	virtual void OnSaveGameSettings(){}
};

#endif
#endif //_CMENUS_H_
