#include "Menus.h"

CMenus::CMenus()
{
	pGameState = NULL;
	pPreCalc = NULL;

	ZeroMemory(&NetworkSettings, sizeof(CNetWorkSettings));
	sprintf(&NetworkSettings.Name[0], "%s", "ChessPlayer");
	sprintf(&NetworkSettings.Port[0], "%s", "9966");
	sprintf(&NetworkSettings.Pass[0], "%s", "MyPass");
	sprintf(&NetworkSettings.ip[0],   "%s", "127.0.0.1");
	NetworkSettings.EditingMode = EDITING_OFF;
	SaveNetworkSettings(BACKUP_SETTINGS);
	SaveNetworkSettings(TEMP_SETTINGS);
	
	memset(&Options, true, sizeof(COptionsSettings));
	Options.UseLODModels = false;
	Options.ShowNextMoves = false;
	Options.ShadowMode = SHADOW_COMPLEX;

	ZeroMemory(&ConnectingOptions, sizeof(CConnectingOptions));
	
	CursorVisible = 0;
	CursorAnimTimer = 0.0f;

	CurrentMenu = MAIN_MENU;
	CurrentMenuSel = 1;
}

void InvBool(bool *b1){
	bool b2 = *b1; 
	b2 = !b2; 
	*b1 = b2;
}

void CMenus::SaveNetworkSettings(int Dest)
{
	memcpy(&NetworkSettingsBackup[Dest], &NetworkSettings, sizeof(CNetWorkSettings));
}

void CMenus::RestoreNetworkSettings(int Dest)
{
	memcpy(&NetworkSettings, &NetworkSettingsBackup[Dest], sizeof(CNetWorkSettings));
}

void CMenus::SaveOptionsSettings()
{
	memcpy(&OptionsBackup, &Options, sizeof(COptionsSettings));
}

void CMenus::RestoreOptionsSettings()
{
	memcpy(&Options, &OptionsBackup, sizeof(COptionsSettings));
}

void CMenus::LoadGameSettings()
{
	OnLoadGameSettings();
	//SaveNetworkSettings(BACKUP_SETTINGS);
	//SaveNetworkSettings(TEMP_SETTINGS);
}

void CMenus::SaveGameSettings()
{
	OnSaveGameSettings();
}

void CMenus::HideMenus()
{
	NetworkSettings.EditingMode = EDITING_OFF;
	*pGameState = GAME_STATE_PLAYING;
	CurrentMenu = MAIN_MENU;
	CurrentMenuSel = 0;
}

void CMenus::ShowMainMenu()
{
	NetworkSettings.EditingMode = EDITING_OFF;
	if(pGameState)
		*pGameState = GAME_STATE_IN_MENU;
	CurrentMenu = MAIN_MENU;
	CurrentMenuSel = OnMenuIsGameStarted() ? 0 : 1;
}

void CMenus::ShowNetworkMenu()
{
	NetworkSettings.EditingMode = EDITING_OFF;
	SaveNetworkSettings(BACKUP_SETTINGS);
	SaveNetworkSettings(TEMP_SETTINGS);
	*pGameState = GAME_STATE_IN_MENU;
	CurrentMenu = NETWORK_MENU;
	CurrentMenuSel = 0;
}

void CMenus::ShowOptionsMenu()
{
	SaveOptionsSettings();
	*pGameState = GAME_STATE_IN_MENU;
	CurrentMenu = OPTIONS_MENU;
	CurrentMenuSel = 0;
}

void CMenus::ShowConnectingMenu()
{
	ZeroMemory(&ConnectingOptions, sizeof(CConnectingOptions));
	*pGameState = GAME_STATE_IN_MENU;
	CurrentMenu = CONNECTING_MENU;
	CurrentMenuSel = 0;
}

void CMenus::ShowAISetupMenu()
{
	*pGameState = GAME_STATE_IN_MENU;
	CurrentMenu = AI_SETUP_MENU;
	CurrentMenuSel = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CMenus::ProcessKeyDownMsg(UINT KeyPressed)
{
	switch(CurrentMenu)
	{
	case MAIN_MENU:
		{
			CMainMenuPrecalcData *pc = &pPreCalc->MenusPrecalcData.MainMenu;
			switch(KeyPressed)
			{
			case VK_UP:     
				if(CurrentMenuSel == 0 || CurrentMenuSel == 4){
					break;
				} else {
					if(CurrentMenuSel > 1){
						if(CurrentMenuSel == 5)
							CurrentMenuSel--;
						CurrentMenuSel--;
						PlayMenusChangeSoundFX();
					}
				} 
				break;
			case VK_DOWN:   
				if(CurrentMenuSel == 0 || CurrentMenuSel == 4){
					break;
				} else {
					if(CurrentMenuSel < pc->NumSel - 1){
						if(CurrentMenuSel == 3)
							CurrentMenuSel++;
						CurrentMenuSel++;
						PlayMenusChangeSoundFX();
					}
				} 
				break;
			case VK_RETURN: 
				switch(CurrentMenuSel)
				{
				case 0: HideMenus();       break;
				case 1: ShowAISetupMenu(); break;
				case 2: ShowNetworkMenu(); break;
				case 3: ShowOptionsMenu(); break;
				case 4: break;
				case 5: OnMenuDoExit();    break;
				}
				break;
			case VK_ESCAPE: 
				OnMenuDoExit();
				break;
			}
		}
		break;
	case NETWORK_MENU:
		{
			CNetworkMenuPrecalcData *pc = &pPreCalc->MenusPrecalcData.NetworkMenu;
			if(NetworkSettings.EditingMode == EDITING_OFF){
				switch(KeyPressed)
				{
				case VK_UP: 
					if(CurrentMenuSel > 0){
						CurrentMenuSel--; 
						PlayMenusChangeSoundFX();
					}
					break;
				case VK_DOWN: 
					if(CurrentMenuSel < pc->NumSel - 1){
						CurrentMenuSel++; 
						PlayMenusChangeSoundFX();
					}
					break;
				case VK_RETURN: 
					/*if(CurrentMenuSel < 4){
						CursorAnimTimer = 0.0f;
						CursorVisible = true;
					}*/

					switch(CurrentMenuSel)
					{
					case 0: 
						NetworkSettings.EditingMode = EDITING_CONNTYPE; 
						SaveNetworkSettings(TEMP_SETTINGS);
						break;
					case 1: 
						NetworkSettings.EditingMode = EDITING_IP;       
						SaveNetworkSettings(TEMP_SETTINGS);
						break;
					case 2: 
						NetworkSettings.EditingMode = EDITING_PORT;     
						SaveNetworkSettings(TEMP_SETTINGS);
						break;
					case 3: 
						ShowConnectingMenu();

						if(NetworkSettings.NetType == NET_SERVER){
							OnMenuDoServerStartListening();
							Sleep(500);
							OnMenuDoConnectToServer();
						} else {
							OnMenuDoConnectToServer();
						}
						break;
					case 4: 
						RestoreNetworkSettings(BACKUP_SETTINGS);
						ShowMainMenu(); 
						break;
					}
					break;
				case VK_ESCAPE: 
					RestoreNetworkSettings(BACKUP_SETTINGS);
					ShowMainMenu(); 
					break;
				}
				break;
			} else if(NetworkSettings.EditingMode != EDITING_OFF){
				switch(KeyPressed)
				{
				case VK_RETURN:
					if(NetworkSettings.EditingMode == EDITING_CONNTYPE)
						SaveNetworkSettings(TEMP_SETTINGS);
					NetworkSettings.EditingMode = EDITING_OFF;
					break;
				case VK_ESCAPE:
					RestoreNetworkSettings(TEMP_SETTINGS);
					NetworkSettings.EditingMode = EDITING_OFF;
					break;
				case VK_UP:
					if(NetworkSettings.EditingMode == EDITING_CONNTYPE)
						NetworkSettings.NetType = NET_SERVER;
					break;
				case VK_DOWN:
					if(NetworkSettings.EditingMode == EDITING_CONNTYPE)
						NetworkSettings.NetType = NET_CLIENT;
					break;
				}
			}
		}
		break;
	case CONNECTING_MENU:
		{
			switch(KeyPressed)
			{
			case VK_ESCAPE: 
				OnMenuDoDisconect();
				ShowNetworkMenu();
				break;
			}
		}
		break;
	case OPTIONS_MENU:
		{
			COptionsMenuPrecalcData *pc = &pPreCalc->MenusPrecalcData.OptionsMenu;
			switch(KeyPressed)
			{
			case VK_UP:     
				if(CurrentMenuSel > 0){
					CurrentMenuSel--; 
					PlayMenusChangeSoundFX();
				}
				break;
			case VK_DOWN:   
				if(CurrentMenuSel < pc->NumSel - 1){
					CurrentMenuSel++;
					PlayMenusChangeSoundFX();
				} 
				break;
			case VK_RETURN:   
				if(CurrentMenuSel >= pc->NumSel - 2){
					if(CurrentMenuSel == pc->NumSel - 1)
						RestoreOptionsSettings();
					ShowMainMenu();
				} else {
					//bool *pOptions = &Options.ShowSky;
					//pOptions[CurrentMenuSel] = !pOptions[CurrentMenuSel];		
					SetOptionsJmp:
					switch(CurrentMenuSel)
					{
					case 0:  Options.ShowSky         = !Options.ShowSky;         break;
					case 1:  Options.ShowClouds      = !Options.ShowClouds;      break;
					case 2:  Options.ShowTerrain     = !Options.ShowTerrain;     break;
					case 3:  Options.ShowRocks       = !Options.ShowRocks;       break;
					case 4:  Options.ShowWater       = !Options.ShowWater;       break;
					case 5:  Options.ShowChessboard  = !Options.ShowChessboard;  break;
					case 6:  Options.ShowColumn      = !Options.ShowColumn;      break;
					case 7:  Options.ShowChessPieces = !Options.ShowChessPieces; break;

					case 8:  Options.ShowFog         = !Options.ShowFog;         break;
					case 9:  Options.UseShader       = !Options.UseShader;       break;
					case 10: Options.ShowReflections = !Options.ShowReflections; break;
					case 11: Options.ShowLensFlares  = !Options.ShowLensFlares;  break;
					case 12: Options.UseLODModels    = !Options.UseLODModels;    break;
					case 13: Options.ShowNextMoves   = !Options.ShowNextMoves;   break;
					case 14: 
						Options.ShowShadows = true; 
						Options.ShadowMode = SHADOW_COMPLEX; 
						break;
					case 15: 
						Options.ShowShadows = true; 
						Options.ShadowMode = SHADOW_SIMPLE; 
						break;
					case 16:Options.ShowShadows = false; break;
					}
				}
				break;
			case VK_SPACE:   
				if(CurrentMenuSel < pc->NumSel - 2)
					goto SetOptionsJmp;			
				break;
			case VK_ESCAPE:   
				RestoreOptionsSettings();
				ShowMainMenu();
				break;
			}
		}
		break;
	case AI_SETUP_MENU:
		{
			CAISetupMenuPrecalcData *pc = &pPreCalc->MenusPrecalcData.AISetupMenu;
			switch(KeyPressed)
			{
			case VK_UP:     
				if(CurrentMenuSel > 0){
					CurrentMenuSel--; 
					PlayMenusChangeSoundFX();
				}
				break;
			case VK_DOWN:   
				if(CurrentMenuSel < pc->NumSel - 1){
					CurrentMenuSel++;
					PlayMenusChangeSoundFX();
				} 
				break;
			case VK_RETURN:   
				if(CurrentMenuSel <= 2){
					OnMenuDoAISetup(CurrentMenuSel);
				} else {
					ShowMainMenu();
				}
				break;
			case VK_ESCAPE:   
				ShowMainMenu();
				break;
			}
		}
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CMenus::ProcessCharMsg(UINT KeyPressed)
{
	switch(CurrentMenu)
	{
	case NETWORK_MENU:
		{
			if(NetworkSettings.EditingMode == EDITING_IP || NetworkSettings.EditingMode == EDITING_PORT){
				char *pBuf = NULL;
				UINT MaxChars = 0;
				UINT CharLimitLo = 0;
				UINT CharLimitHi = 0;
				switch(NetworkSettings.EditingMode)
				{
				case EDITING_IP:   pBuf = &NetworkSettings.ip[0];   MaxChars = 15; CharLimitLo = 32; CharLimitHi = 255; break;
				case EDITING_PORT: pBuf = &NetworkSettings.Port[0]; MaxChars = 5;  CharLimitLo = 48; CharLimitHi = 57;  break;
				}
				
				if(KeyPressed >= CharLimitLo && KeyPressed <= CharLimitHi){
					UINT NameLen = strlen(pBuf);
					if(NameLen < MaxChars){
						pBuf[NameLen]   = (BYTE)KeyPressed;
						pBuf[NameLen+1] = 0;
						//CursorVisible   = true;
						//CursorAnimTimer = 0.0f;
					}
				}
			}
		}
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool IsInSquare(float x, float y, float l, float r, float t, float b, float WidthRatio, float HeightRatio)
{
	l *= WidthRatio;
	r *= WidthRatio;
	t *= HeightRatio;
	b *= HeightRatio;
	
	return ((x >= l && x < r) && (y >= t && y < b));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CMenus::ProcessMouseMoveMsg(int x, int y, float WidthRatio, float HeightRatio, bool IsWideScreen)
{
	switch(CurrentMenu)
	{
	case MAIN_MENU:
		{
			CMainMenuPrecalcData *pc = &pPreCalc->MenusPrecalcData.MainMenu;
			float *pCoords = &pc->MouseInputCoords[0][0];
			for(int Cpt = 0; Cpt < pc->NumSel; Cpt++){
				if(Cpt == 4 || (Cpt == 0 && !OnMenuIsGameStarted())){
					pCoords += 4;
					continue;
				}
				if(IsInSquare((float)x, (float)y, pCoords[0], pCoords[1], pCoords[2], pCoords[3], WidthRatio, HeightRatio)){
					if(CurrentMenuSel != Cpt){
						CurrentMenuSel = Cpt;
						PlayMenusChangeSoundFX();
						break;
					}
				}
				pCoords += 4;
			}
		}
		break;
	case NETWORK_MENU:
		{
			CNetworkMenuPrecalcData *pc = &pPreCalc->MenusPrecalcData.NetworkMenu;
			switch(NetworkSettings.EditingMode)
			{
			case EDITING_OFF:
				{
					float *pCoords = &pc->MouseInputCoords[0][0];
					for(int Cpt = 0; Cpt < pc->NumSel; Cpt++){
						if(IsInSquare((float)x, (float)y, pCoords[0], pCoords[1], pCoords[2], pCoords[3], WidthRatio, HeightRatio)){
							if(CurrentMenuSel != Cpt){
								CurrentMenuSel = Cpt;
								PlayMenusChangeSoundFX();
								break;
							}
						}
						pCoords += 4;
					}
				}
				break;
			case EDITING_CONNTYPE:
				{
					float *pCoords = &pc->NetTypeSelMouseInputCoords[0][0];
					for(int Cpt = 0; Cpt < 2; Cpt++){
						if(IsInSquare((float)x, (float)y, pCoords[0], pCoords[1], pCoords[2], pCoords[3], WidthRatio, HeightRatio)){
							if(NetworkSettings.NetType != Cpt){
								NetworkSettings.NetType = Cpt;
								PlayMenusChangeSoundFX();
							}
							break;
						}
						pCoords += 4;
					}
				}
				break;
			}
		}
		break;
	case OPTIONS_MENU:
		{
			COptionsMenuPrecalcData *pc = &pPreCalc->MenusPrecalcData.OptionsMenu;
			float *pCoords = &pc->MouseInputCoords[0][0];
			for(int Cpt = 0; Cpt < pc->NumSel; Cpt++){
				if(IsInSquare((float)x, (float)y, pCoords[0], pCoords[1], pCoords[2], pCoords[3], WidthRatio, HeightRatio)){
					if(CurrentMenuSel != Cpt){
						CurrentMenuSel = Cpt;
						PlayMenusChangeSoundFX();
						break;
					}
				}
				pCoords += 4;
			}
		}
		break;
	case AI_SETUP_MENU:
		{
			CAISetupMenuPrecalcData *pc = &pPreCalc->MenusPrecalcData.AISetupMenu;
			float *pCoords = &pc->MouseInputCoords[0][0];
			for(int Cpt = 0; Cpt < pc->NumSel; Cpt++){
				if(IsInSquare((float)x, (float)y, pCoords[0], pCoords[1], pCoords[2], pCoords[3], WidthRatio, HeightRatio)){
					if(CurrentMenuSel != Cpt){
						CurrentMenuSel = Cpt;
						PlayMenusChangeSoundFX();
						break;
					}
				}
				pCoords += 4;
			}
		}
		break;
	}
}

void CMenus::ProcessMouseClickMsg(int x, int y, float WidthRatio, float HeightRatio, bool IsWideScreen)
{
	switch(CurrentMenu)
	{
	case MAIN_MENU:
		{
			CMainMenuPrecalcData *pc = &pPreCalc->MenusPrecalcData.MainMenu;
			float *pCoords = &pc->MouseInputCoords[0][0];
			for(int Cpt = 0; Cpt < pc->NumSel; Cpt++){
				if(Cpt == 4 || (Cpt == 0 && !OnMenuIsGameStarted())){
					pCoords += 4;
					continue;
				}
				if(IsInSquare((float)x, (float)y, pCoords[0], pCoords[1], pCoords[2], pCoords[3], WidthRatio, HeightRatio)){
					ProcessKeyDownMsg(VK_RETURN);
					break;
				}			
				pCoords += 4;
			}
		}
		break;
	case NETWORK_MENU:
		{
			CNetworkMenuPrecalcData *pc = &pPreCalc->MenusPrecalcData.NetworkMenu;
			switch(NetworkSettings.EditingMode)
			{
			case EDITING_OFF:
				{
					float *pCoords = &pc->MouseInputCoords[0][0];
					for(int Cpt = 0; Cpt < pc->NumSel; Cpt++){
						if(IsInSquare((float)x, (float)y, pCoords[0], pCoords[1], pCoords[2], pCoords[3], WidthRatio, HeightRatio)){
							ProcessKeyDownMsg(VK_RETURN);
							PlayMenusChangeSoundFX();
							break;
						}
						pCoords += 4;
					}
					pCoords = &pc->NetTypeSelMouseInputCoords[0][0];
					for(int Cpt = 0; Cpt < 2; Cpt++){
						if(IsInSquare((float)x, (float)y, pCoords[0], pCoords[1], pCoords[2], pCoords[3], WidthRatio, HeightRatio)){
							CurrentMenuSel = 0;
							NetworkSettings.EditingMode = EDITING_CONNTYPE;
							PlayMenusChangeSoundFX();
							break;
						}
						pCoords += 4;
					}
					pCoords = &pc->IPBox[0];
					if(IsInSquare((float)x, (float)y, pCoords[0], pCoords[1], pCoords[2], pCoords[3], WidthRatio, HeightRatio)){
						CurrentMenuSel = 1;
						ProcessKeyDownMsg(VK_RETURN);
						NetworkSettings.EditingMode = EDITING_IP;
						PlayMenusChangeSoundFX();
						break;
					}
					pCoords = &pc->PortBox[0];
					if(IsInSquare((float)x, (float)y, pCoords[0], pCoords[1], pCoords[2], pCoords[3], WidthRatio, HeightRatio)){
						CurrentMenuSel = 2;
						NetworkSettings.EditingMode = EDITING_PORT;
						PlayMenusChangeSoundFX();
						break;
					}
				}
				break;
			case EDITING_CONNTYPE:
				{
					bool found = false;
					float *pCoords = &pc->NetTypeSelMouseInputCoords[0][0];
					for(int Cpt = 0; Cpt < 2; Cpt++){
						if(IsInSquare((float)x, (float)y, pCoords[0], pCoords[1], pCoords[2], pCoords[3], WidthRatio, HeightRatio)){
							ProcessKeyDownMsg(VK_RETURN);
							PlayMenusChangeSoundFX();
							found = true;
							break;
						}
						pCoords += 4;
					}
					if(!found)
						ProcessKeyDownMsg(VK_ESCAPE);
				}
				break;
			case EDITING_IP:
				{
					float *pCoords = &pc->IPBox[0];
					if(!IsInSquare((float)x, (float)y, pCoords[0], pCoords[1], pCoords[2], pCoords[3], WidthRatio, HeightRatio)){
						ProcessKeyDownMsg(VK_RETURN);
						PlayMenusChangeSoundFX();
						break;
					}
				}
				break;
			case EDITING_PORT:
				{
					float *pCoords = &pc->PortBox[0];
					if(!IsInSquare((float)x, (float)y, pCoords[0], pCoords[1], pCoords[2], pCoords[3], WidthRatio, HeightRatio)){
						ProcessKeyDownMsg(VK_RETURN);
						PlayMenusChangeSoundFX();
						break;
					}
				}
				break;
			}
		}
		break;
	case OPTIONS_MENU:
		{
			COptionsMenuPrecalcData *pc = &pPreCalc->MenusPrecalcData.OptionsMenu;
			float *pCoords = &pc->MouseInputCoords[0][0];
			for(int Cpt = 0; Cpt < pc->NumSel; Cpt++){
				if(IsInSquare((float)x, (float)y, pCoords[0], pCoords[1], pCoords[2], pCoords[3], WidthRatio, HeightRatio)){
					ProcessKeyDownMsg(VK_RETURN);
					PlayMenusChangeSoundFX();
					break;
				}
				pCoords += 4;
			}
		}
		break;
	case AI_SETUP_MENU:
		{
			CAISetupMenuPrecalcData *pc = &pPreCalc->MenusPrecalcData.AISetupMenu;
			float *pCoords = &pc->MouseInputCoords[0][0];
			for(int Cpt = 0; Cpt < pc->NumSel; Cpt++){
				if(IsInSquare((float)x, (float)y, pCoords[0], pCoords[1], pCoords[2], pCoords[3], WidthRatio, HeightRatio)){
					ProcessKeyDownMsg(VK_RETURN);
					PlayMenusChangeSoundFX();
					break;
				}
				pCoords += 4;
			}
		}
		break;
	}
}

