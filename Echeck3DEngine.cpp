#include "Echeck3DEngine.h"

CEcheck3DEngine *pEngine = NULL;

////////////////////////////////////////////////////////////////////////////////////////////

void CE3DLog::LogArgs(const char *fmt, ...)
{
	va_list ap;
	if(fmt != NULL){
		CRawBuffer Buf(2048);
		va_start(ap, fmt);
		vsprintf(Buf.GetStrBuffer(), fmt, ap);
		va_end(ap);
		strcat(Buf.GetStrBuffer(), "\n");
		WriteLine(Buf.GetStrBuffer());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

CEcheck3DEngine::CEcheck3DEngine()
{
	pEngine = this;

	IsGameEngineInitialized = false;

	GameType = GAME_TYPE_LOCAL;
	GameState = GAME_STATE_IN_MENU;

	pCurrentCam = NULL;
	UseDualView = false;	
	DualViewMode = DUAL_VIEW_MODE_H;
	InitCameras();

	TimeScale = 1;
	SetGameTime(7, 30);
	
	MoveChessPieceTimeLeft = 0.0f;
	MoveChessPieceOffset.X = 0;
	MoveChessPieceOffset.Y = 0;

	ShowDebugInfo = false;
	ShowCheckTimeLeft = 0.0f;
	ShowCheckMateTimeLeft = 0.0f;

	SkyDomeAlpha = 1.0f;
	ShadowMapAlpha = 0.0f;
	WaterTexOffset = 0.0f;
	WaveMovement = 0.0f;
	CurrentCaustIndx = 0.0f;
	SkyPlaneTexOffset = SKYPLANE_TRANSITION_LENGTH / 2.0f;

	UpdateSunPos();

	CurrentSel = -1;
	CurrentSelPos.X = 0;
	CurrentSelPos.Y = 0;

	IsPromotePawnDlgVisible = false;
	PromoteColor = WHITE;

	pChatMsgMemNode = NULL;
	MaxChatWidth = 200;
	InEditingChatMsg = false;
	IsChatCursorVisible = false;
	ChatMsgList.Clear();
	ChatMsgBuffer.Reset();

#ifndef LITE_MODE
	LiteMode = false;
#else
	LiteMode = true;
#endif
	ForceLinesRendering = false;
}

////////////////////////////////////////////////////////////////////////////////////////////

CEcheck3DEngine::~CEcheck3DEngine()
{
	NetClient.Disconnect();
	NetServer.Disconnect();
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DShader::InitShaderDataExt()
{
	hLightPos0       = glGetUniformLocationARB(m_Program, "LightPos0");
	hTex0Scale       = glGetUniformLocationARB(m_Program, "Tex0Scale");
	hTex1Scale       = glGetUniformLocationARB(m_Program, "Tex1Scale");
	hTex0Translate   = glGetUniformLocationARB(m_Program, "Tex0Translate");
	hTex1Translate   = glGetUniformLocationARB(m_Program, "Tex1Translate");
	hUsePerVertexFog = glGetUniformLocationARB(m_Program, "UsePerVertexFog");
	hUseLighting     = glGetUniformLocationARB(m_Program, "UseLighting");
	hUseWaterWave    = glGetUniformLocationARB(m_Program, "UseWaterWave");
	hWaveMovement    = glGetUniformLocationARB(m_Program, "WaveMovement");
	hUseAlphaBlendingForReflection = glGetUniformLocationARB(m_Program, "UseAlphaBlendingForReflection");
}

////////////////////////////////////////////////////////////////////////////////////////////

bool CEcheck3DEngine::LoadAIDLL()
{
	//Load the AI dll and it's functions pointers
	hAIDll = LoadLibrary("AI.dll");
	if(!hAIDll)
		return false;

	AI.pProcessAICmd             = (int  (__cdecl*)(char *s))GetProcAddress(hAIDll, "ProcessAICmd");
	AI.pInit                     = (void (__cdecl*)())GetProcAddress(hAIDll, "Init");
	AI.pGameStateCheck           = (char* (__cdecl*)())GetProcAddress(hAIDll, "GameStateCheck");
	AI.pQueryChessBoardInfo      = (void (__cdecl*)(int x, int y, int *PcxType, int *PcxColor, BOOL Ghost))GetProcAddress(hAIDll, "QueryChessBoardInfo");
	AI.pSetChessBoardInfo        = (void (__cdecl*)(int x, int y, int PcxType, int PcxColor))GetProcAddress(hAIDll, "SetChessBoardInfo");
	AI.pGenGhostBoard            = (void (__cdecl*)(int From))GetProcAddress(hAIDll, "GenGhostBoard");
	AI.pSetHostAppHandlePointers = (void (__cdecl*)(HWND lhWnd))GetProcAddress(hAIDll, "SetHostAppHandlePointers");
	AI.pIsAIThreadRunning        = (BOOL (__cdecl*)())GetProcAddress(hAIDll, "IsAIThreadRunning");
	AI.pGetCurrentTurn           = (int  (__cdecl*)())GetProcAddress(hAIDll, "GetCurrentTurn");
	AI.pLoadGame                 = (BOOL (__cdecl*)(char *pFileName))GetProcAddress(hAIDll, "LoadGame");
	AI.pSaveGame                 = (void (__cdecl*)(char *pFileName))GetProcAddress(hAIDll, "SaveGame");
	AI.pFinishAIThreadJob        = (void (__cdecl*)())GetProcAddress(hAIDll, "FinishAIThreadJob");
	AI.pIsGameStarted            = (BOOL (__cdecl*)())GetProcAddress(hAIDll, "IsGameStarted");

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::FreeAIDLL()
{
	if(hAIDll)
		FreeLibrary(hAIDll);
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

bool CE3DShadowCaster::OnCreateVBO(glObject *o)
{
	if(o->VertsArrayID != 0)
		return true;

	CVBO *pVBO = pEngine->GetVBOPtr();
	pVBO->glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	pVBO->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	if(o == NULL)
		return false;

	pVBO->glGenBuffersARB(1, &o->VertsArrayID);
	pVBO->glBindBufferARB(GL_ARRAY_BUFFER_ARB, o->VertsArrayID);
	pVBO->glBufferDataARB(GL_ARRAY_BUFFER_ARB, o->NumVertsVisible * 3 * sizeof(float), o->pVertsArray, GL_STREAM_DRAW_ARB);

	return true;
}

void CE3DShadowCaster::OnBindVBO(glObject *o)
{
	if(o->VertsArrayID != 0){
		CVBO *pVBO = pEngine->GetVBOPtr();
		pVBO->glBindBufferARB(GL_ARRAY_BUFFER_ARB, o->VertsArrayID);
	}
}

void CE3DShadowCaster::OnDeleteVBO(glObject *o)
{
	if(o->VertsArrayID != 0){
		CVBO *pVBO = pEngine->GetVBOPtr();
		pVBO->glDeleteBuffersARB(1, &o->VertsArrayID);
		o->VertsArrayID = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::OnWinsockMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(NetClient.GetSocket() == (SOCKET)wParam){
		NetClient.ProcessWinsockMsg(GethWnd(), wParam, lParam);
	} else {
		NetServer.ProcessWinsockMsg(GethWnd(), wParam, lParam);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::OnNetworkMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(NetClient.GetSocket() == (SOCKET)wParam){
		NetClient.ProcessNetworkMsg(GethWnd(), wParam, lParam);
	} else {
		NetServer.ProcessNetworkMsg(GethWnd(), wParam, lParam);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CNetServer::OnConnectionAccepted(CSocketNode *pNode)
{

}
void CNetServer::OnConnectionRefused(UINT Reason)
{
}
void CNetServer::OnConnectionClosed(CSocketNode *pNode)
{
}
void CNetServer::OnClientLogedIn(CSocketNode *pNode)
{

}
void CNetServer::OnClientLoginFailed(CSocketNode *pNode, UINT Err)
{
}

void CNetServer::OnEveryoneLoged()
{
	pEngine->Menus.HideMenus();
}

void CNetServer::OnCustomNetworkMsgRecv(CSocketNode *pNode, CNetworkMsg *pMsg)
{
	// Echo stuff back to the other client
	switch(pMsg->MsgID)
	{
	case NET_MOVE_MSG:
		if(pMsg->MsgSize == 4 && SocketList.GetNodeCount() == 2){
			CSocketNode *pNode1 = SocketList.GetNode(0);
			CSocketNode *pNode2 = SocketList.GetNode(1);
			if(pNode1 && pNode2){
				if(pNode == pNode1)
					SendMsg(pNode2->Socket, pMsg);
				else if(pNode == pNode2)
					SendMsg(pNode1->Socket, pMsg);
			}
		}
		break;
	}
}

void CNetServer::OnChatMsg(UINT PlayerID, char *pPlayerName, char *pTextData)
{
	// Bouce the message back to others clients
	UINT NumPlayers = SocketList.GetNodeCount();
	for(UINT Cpt = 0; Cpt < NumPlayers; Cpt++){
		CSocketNode *pTmpNode = SocketList.GetNode(Cpt);
		if(pTmpNode->IsLogedIn)
			SendChatMsg(pTmpNode->Socket, PlayerID, pPlayerName, pTextData);
	}
}

void CNetServer::OnServerDisconnected(SOCKET s)
{
	pEngine->pLog->LogArgs("Closing Server...");
}

void CNetServer::OnClientDisconnected(CSocketNode *pNode)
{
	pEngine->pLog->LogArgs("Disconnecting Client (ID: #%d)...", pNode->PlayerID);
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::OnCustomMessageExt(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_AI_SHOWCURSOR: SetCursor(LoadCursor(NULL, lParam == 0 ? IDC_ARROW : IDC_WAIT)); break;
	case WM_AI_CPU_CHECK : AI.pProcessAICmd("CPU_Check"); break;
	case WM_DO_CPU_MOVE  :
		{
			WORD w1, w2;
			//Reassemble the 2 compacted WORD
			_asm {
				mov eax, lParam; //Put lParam in eax
				mov w2,  ax;      //Put ax in w2
				shr eax, 16;     //Shift eax 2 bytes right  
				mov w1,  ax;      //Put ax in w1 
			}

			//Convert the WORD in two SIGNED int value
			int x = (short)w1; 
			int y = (short)w2;

			//Tell us which pcx is selected
			CurrentSel = (DWORD)wParam;
			//Reset CurrentSelPos 
			CurrentSelPos.X = 0.0f;
			CurrentSelPos.Y = 0.0f;
			//Reset CpuMoveOffsetX and CpuMoveOffsetY
			MoveChessPieceOffset.X = x;
			MoveChessPieceOffset.Y = y;
			//Set the time to move the pcx left to default
			MoveChessPieceTimeLeft = PCX_MOVE_TRANSITION_LENGTH;
			
			//Finish the AI Job!
			AI.pFinishAIThreadJob();
		}
		break;

	case WM_AI_GAME_MSG:
		if(strcmp((char*)wParam, "- Black Are In Check -") == 0)
			ShowCheckTimeLeft = 2.0f;
		else if(strcmp((char*)wParam, "- White Are In Check -") == 0)
			ShowCheckTimeLeft = 2.0f;
		if(strcmp((char*)wParam, "- Black Are Checkmate -") == 0)
			ShowCheckMateTimeLeft = 2.0f;
		else if(strcmp((char*)wParam, "- White Are Checkmate -") == 0)
			ShowCheckMateTimeLeft = 2.0f;
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CNetClient::OnConnectionEstablished(SOCKET s)
{
	pEngine->Menus.ConnectingOptions.IsConnected = true;

	/*DWORD Start = GetTickCount();
	DWORD Stop = Start;
	while(Stop < Start + 1500){
		pEngine->RenderScene();
		Sleep(16);
		Stop = GetTickCount();
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////

void CNetClient::OnConnectionLoss(SOCKET s, UINT ErrCode)
{
	pEngine->Menus.ConnectingOptions.IsConnected = false;
	pEngine->Menus.ConnectingOptions.IsLogedIn = false;
	pEngine->Menus.ConnectingOptions.IsOpponentReady = false;

	Disconnect();
	pEngine->Menus.ShowMainMenu();
}

void CNetClient::OnLoginSuccess(SOCKET s, UINT PlayerID)
{
	pEngine->Menus.ConnectingOptions.IsLogedIn = true;

	pEngine->ClientID = PlayerID;

	/*DWORD Start = GetTickCount();
	DWORD Stop = Start;
	while(Stop < Start + 1500){
		pEngine->RenderScene();
		Sleep(16);
		Stop = GetTickCount();
	}*/

	//pEngine->Menus.HideMenus();
}

////////////////////////////////////////////////////////////////////////////////////////////

void CNetClient::OnLoginFailed(SOCKET s, UINT Err)
{
	pEngine->Menus.ConnectingOptions.IsConnected = false;

	/*switch(Err)
	{
	case ERR_LOGIN_PASS_INVALID: pEngine->ChatMsgList.Push(0, "SERVER", "Login failed... (Invalid login pass)");  break;
	case ERR_PLAYER_NAME_IN_USE: pEngine->ChatMsgList.Push(0, "SERVER", "Login failed... (Name already in use)"); break;
	case ERR_VERSION_DONT_MATCH: pEngine->ChatMsgList.Push(0, "SERVER", "Login failed... (Version dont match)");  break;
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////

void CNetClient::OnEveryoneReady()
{
	pEngine->Menus.ConnectingOptions.IsOpponentReady = true;

	// Initialise game variables
	/*DWORD Start = GetTickCount();
	DWORD Stop = Start;
	while(Stop < Start + 1500){
		pEngine->RenderScene();
		Sleep(16);
		Stop = GetTickCount();
	}*/

	pEngine->Menus.HideMenus();
}

////////////////////////////////////////////////////////////////////////////////////////////

void CNetClient::OnChatMsg(UINT PlayerID, char *pPlayerName, char *pTextData)
{
	pEngine->ChatMsgList.Push(PlayerID, pPlayerName, pTextData);
}

void CNetClient::OnCustomNetworkMsgRecv(SOCKET s, CNetworkMsg *pMsg)
{
	switch(pMsg->MsgID)
	{
	case NET_MOVE_MSG:

		if(pMsg->MsgSize == 4){
			char AICmd[8];         //The command buffer...
			ZeroMemory(AICmd, 8);  //Init the buffer...

			//Write in the first 4 bytes the XY coordiate for both the origin and the end of the move
			memcpy(&AICmd[0], &pMsg->pData[0], 4);
			
			int Turn = pEngine->AI.pGetCurrentTurn();

			//Process our move command...
			switch(pEngine->AI.pProcessAICmd(&AICmd[0]))
			{
			case MOVE_PUT_KING_IN_CHESS:
				break;

			case INVALID_CMD:
				break;

			case VALID_CMD:
				pEngine->SoundsFX.Drop[Turn == WHITE ? BLACK : WHITE].Play();

				int   FromX, FromY, ToX, ToY;
				FromX = pMsg->pData[0] - 'a';
				FromY = pMsg->pData[1] - '1';
				ToX   = pMsg->pData[2] - 'a';
				ToY   = pMsg->pData[3] - '1';

				//Tell us which pcx is selected
				pEngine->CurrentSel = ((ToY << 3) + ToX);
				//Reset CurrentSelPos 
				pEngine->CurrentSelPos.X = 0.0f;
				pEngine->CurrentSelPos.Y = 0.0f;
				//Reset CpuMoveOffsetX and CpuMoveOffsetY
				pEngine->MoveChessPieceOffset.X = FromX - ToX;
				pEngine->MoveChessPieceOffset.Y = -(FromY - ToY);
				//Set the time to move the pcx left to default
				pEngine->MoveChessPieceTimeLeft = PCX_MOVE_TRANSITION_LENGTH;
				break;
			}
		}
		break;
	}
}

void CNetClient::SendMoveMsg(char *Move)
{
	CNetworkMsg Msg;
	Msg.MsgID = NET_MOVE_MSG;
	Msg.MsgSize = 4;
	Msg.pData = (BYTE*)Move;

	SendMsg(&Msg);
}

void CNetClient::OnDisconnect(SOCKET s)
{
	pEngine->pLog->LogArgs("Disconnecting Client (Socket: #%d)...", s);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void CE3DMenus::OnLoadGameSettings()
{
	CFileIO f;
	if(f.OpenForReading("Settings.set", false)){
		f.BlockRead(&pEngine->Menus.Options, sizeof(COptionsSettings));
		f.BlockRead(&pEngine->Menus.NetworkSettings, sizeof(CNetWorkSettings));
		f.Close();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

void CE3DMenus::OnSaveGameSettings()
{
	CFileIO f;
	if(f.OpenForWriting("Settings.set", false)){
		f.BlockWrite(&pEngine->Menus.Options, sizeof(COptionsSettings));
		f.BlockWrite(&pEngine->Menus.NetworkSettings, sizeof(CNetWorkSettings));
		f.Close();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

void CE3DMenus::OnMenuDoServerStartListening()
{
	pEngine->GameType = GAME_TYPE_NETWORKED;
	pEngine->NetServer.Disconnect();
	pEngine->NetServer.StartServer(pEngine->GethWnd(), atoi(NetworkSettings.Port), &NetworkSettings.Pass[0], 2, "3.0");
}

////////////////////////////////////////////////////////////////////////////////////////////

void CE3DMenus::OnMenuDoConnectToServer()
{
	pEngine->GameType = GAME_TYPE_NETWORKED;
	pEngine->NetClient.Disconnect();
	pEngine->NetClient.ConnectToServer(pEngine->GethWnd(), &NetworkSettings.ip[0], atoi(NetworkSettings.Port), &NetworkSettings.Name[0], &NetworkSettings.Pass[0], "3.0");
}

////////////////////////////////////////////////////////////////////////////////////////////

void CE3DMenus::OnMenuDoDisconect()
{
	pEngine->NetServer.Disconnect();
	pEngine->NetClient.Disconnect();
}

////////////////////////////////////////////////////////////////////////////////////////////

void CE3DMenus::OnMenuDoAISetup(int Sel)
{
	pEngine->ResetGame();
	switch(Sel)
	{
	case 0: 
		pEngine->AI.pProcessAICmd("Set_CPU_To_NO_COLOR"); 
		break;
	case 1: 
		pEngine->AI.pProcessAICmd("Set_CPU_To_BLACK"); 
		break;
	case 2: 
		pEngine->AI.pProcessAICmd("Set_CPU_To_WHITE"); 
		pEngine->AI.pProcessAICmd("CPU_Check");
		break;
	}
	HideMenus();
}

////////////////////////////////////////////////////////////////////////////////////////////

bool CE3DMenus::OnMenuIsGameStarted()
{
	return pEngine->AI.pIsGameStarted() == FALSE ? false : true;
}

////////////////////////////////////////////////////////////////////////////////////////////

void CE3DMenus::OnMenuDoExit()
{
	PostQuitMessage(0);
}

////////////////////////////////////////////////////////////////////////////////////////////

void CE3DMenus::PlayMenusChangeSoundFX()
{
	pEngine->SoundsFX.MenusChange.Play();
}

////////////////////////////////////////////////////////////////////////////////////////////

bool CEcheck3DEngine::LoadSkyDomePrimaryTexture(char *fname)
{
	CFileIO f;
	if(f.OpenForReading(fname)){
	
		f.BlockRead(&SkyDomeColor[0][0], SKYDOME_TEXTURE_WIDTH * 3);
		f.BlockRead(&SkyDomeColor[1][0], SKYDOME_TEXTURE_WIDTH * 3);

		f.Close();
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::OnInitializeOpenGLForLoadingExt()
{
	CTexture TexObj;
	TexObj.LoadTexture("Textures\\LoadScreen\\LoadScreen.bmp", &Textures.LoadScreen[0], LINEAR_FILTER, NO_ALPHA_CHANNEL);
	TexObj.LoadTexture("Textures\\LoadScreen\\Bar.bmp",        &Textures.LoadScreen[1], LINEAR_FILTER, NO_ALPHA_CHANNEL);
	
	#ifndef NO_FBO
	if(!GameWindow.IsFullScreen())
		LoadingFBO.InitFBO(GameWindow.GetDevWidth(), GameWindow.GetDevHeight());
	#endif
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::OnCleanUpOpenGLForLoadingExt()
{
	CTexture TexObj;
	for(int Cpt = 1; Cpt >= 0; Cpt--)
		TexObj.FreeTexture(&Textures.LoadScreen[Cpt]);
	
	if(LoadingFBO.IsInitialized()){
		wglMakeCurrent(GetLoadinghDC(), GetLoadinghGLRC());
		LoadingFBO.ShutdownFBO();
		wglMakeCurrent(GethDC(), GethGLRC());
	}

}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::RenderLoadingExt(int PercentDone)
{
	RenderLoadScreen(PercentDone);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::InitEngineExt()
{
	if(IsShadersSupported())
		Shader.LoadShaders("Shader\\Shader.vs", NULL);

	DebugInfoFont.Load("Arial", 12, BLEND_FONT_NORMAL, FONT_FILTER_LINEAR);
	NetworkMenuFont.Load("Arial", 16, BLEND_FONT_NORMAL, FONT_FILTER_LINEAR);
	GameMsgFont.Load("scythe", 72, BLEND_FONT_NORMAL, FONT_FILTER_LINEAR);
	ChatMsgFont.Load("Arial", 10, BLEND_FONT_SPECIAL, FONT_FILTER_NEAREST);

	InitializeOpenGLForLoading();
	wglMakeCurrent(GethDC(), GethGLRC()); // <- to add in InitializeOpenGLForLoading() later

	pLog->LogArgs("Loading SoundFX..... %s",   LoadSoundsFX() ? "Sucess!" : "Error: LoadSoundsFX() Failed...");
	pLog->LogArgs("Loading Textures.... %s",   LoadGameTextures() ? "Sucess!" : "Error: LoadGameTextures() Failed...");
	pLog->LogArgs("Loading 3D Models... %s\n", LoadGameModels() ? "Sucess!" : "Error: LoadGameModels() Failed...");
	RenderLoading(100);

	Sleep(100);
	CleanUpOpenGLForLoading();

	AI.pSetHostAppHandlePointers(GethWnd());
	AI.pProcessAICmd("Set_CPU_To_NO_COLOR");	
	ResetGame();

	SetVSync(true);
	SetupFog();

	#ifndef NO_FBO
	if(!GameWindow.IsFullScreen())
		FBO.InitFBO(GameWindow.GetDevWidth(), GameWindow.GetDevHeight());
	#endif

	pLog->LogArgs("VBO Support............. %s", IsVBOSupported() ? "Yes" : "No");
	pLog->LogArgs("FBO Support............. %s", FBO.IsInitialized() ? "Yes" : "No");
	pLog->LogArgs("Shaders Support......... %s", IsShadersSupported() ? "Yes" : "No");
	pLog->LogArgs("Fog Extention Support... %s", IsFogExtSupported() ? "Yes" : "No");
	pLog->LogArgs("Multitexture Support.... %s", IsMultitextureExtSupported() ? "Yes" : "No");
	pLog->LogArgs("Max Multitexture Maps... %d\n", GetTexelUnitNumber());

	Menus.ShowMainMenu();
	IsGameEngineInitialized = true;
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::ShutdownEngineExt()
{
	if(IsShadersSupported())
		Shader.FreeShader();

	CloudsNoise.CancelBackgroundProcess();

	if(FBO.IsInitialized())
		FBO.ShutdownFBO();

	FreeGameModels();
	FreeGameTextures();

	DebugInfoFont.Free();
	NetworkMenuFont.Free();
	GameMsgFont.Free();
	ChatMsgFont.Free();
	FreeSoundsFX();
}

bool CEcheck3DEngine::LoadSoundsFX()
{
	bool Res = true;

	if(!SoundsFX.Drop[0].LoadFromFile("SoundsFX\\WhiteMove.wav", false))
		Res = false;
	if(!SoundsFX.Drop[1].LoadFromFile("SoundsFX\\BlackMove.wav", false))
		Res = false;
	if(!SoundsFX.IllegalDrop.LoadFromFile("SoundsFX\\CannotDrop.wav", false))
		Res = false;
	if(!SoundsFX.MenusChange.LoadFromFile("SoundsFX\\MenusChange.wav", false))
		Res = false;

	return Res;
}

void CEcheck3DEngine::FreeSoundsFX()
{
	for(int Cpt = 1; Cpt >= 0; Cpt--)
		SoundsFX.Drop[Cpt].FreeSound();
	SoundsFX.IllegalDrop.FreeSound();
	SoundsFX.MenusChange.FreeSound();
}

////////////////////////////////////////////////////////////////////////////////////////////

bool CEcheck3DEngine::LoadGameTextures()
{
	CTexture TexObj;
	RenderLoading(0);

	TexObj.LoadTexture("Textures\\Menus\\Arrow.bmp", &Textures.Menus.Arrow, LINEAR_FILTER, NO_ALPHA_CHANNEL);
	RenderLoading(3);
	TexObj.LoadTexture("Textures\\Menus\\MainMenuA.bmp", &Textures.Menus.MainMenu[0], LINEAR_FILTER, NO_ALPHA_CHANNEL);
	TexObj.LoadTexture("Textures\\Menus\\MainMenuB.bmp", &Textures.Menus.MainMenu[1], LINEAR_FILTER, NO_ALPHA_CHANNEL);
	RenderLoading(7);
	TexObj.LoadTexture("Textures\\Menus\\NetworkMenuA.bmp", &Textures.Menus.NetworkMenu[0], LINEAR_FILTER, NO_ALPHA_CHANNEL);
	TexObj.LoadTexture("Textures\\Menus\\NetworkMenuB.bmp", &Textures.Menus.NetworkMenu[1], LINEAR_FILTER, NO_ALPHA_CHANNEL);
	RenderLoading(13);
	TexObj.LoadTexture("Textures\\Menus\\OptionsMenuA.bmp", &Textures.Menus.OptionsMenu[0], LINEAR_FILTER, NO_ALPHA_CHANNEL);
	TexObj.LoadTexture("Textures\\Menus\\OptionsMenuB.bmp", &Textures.Menus.OptionsMenu[1], LINEAR_FILTER, NO_ALPHA_CHANNEL);
	RenderLoading(16);
	TexObj.LoadTexture("Textures\\Menus\\AISetupMenuA.bmp", &Textures.Menus.AISetupMenu[0], LINEAR_FILTER, NO_ALPHA_CHANNEL);
	TexObj.LoadTexture("Textures\\Menus\\AISetupMenuB.bmp", &Textures.Menus.AISetupMenu[1], LINEAR_FILTER, NO_ALPHA_CHANNEL);
	RenderLoading(20);

	TexObj.LoadTexture("Textures\\Menus\\Connecting\\Connecting.bmp", &Textures.Menus.ConnectMenu[0], LINEAR_FILTER, NO_ALPHA_CHANNEL);
	TexObj.LoadTexture("Textures\\Menus\\Connecting\\Connected.bmp", &Textures.Menus.ConnectMenu[1], LINEAR_FILTER, NO_ALPHA_CHANNEL);
	TexObj.LoadTexture("Textures\\Menus\\Connecting\\WaitingForOpponent.bmp", &Textures.Menus.ConnectMenu[2], LINEAR_FILTER, NO_ALPHA_CHANNEL);
	TexObj.LoadTexture("Textures\\Menus\\Connecting\\ConnectionFailed.bmp", &Textures.Menus.ConnectMenu[3], LINEAR_FILTER, NO_ALPHA_CHANNEL);

	TexObj.LoadTexture("Textures\\Promote\\Promote White Pawn.bmp", &Textures.PromotePawn[0], LINEAR_FILTER, ADD_ALPHA_MASKING);
	TexObj.LoadTexture("Textures\\Promote\\Promote Black Pawn.bmp", &Textures.PromotePawn[1], LINEAR_FILTER, ADD_ALPHA_MASKING);
	RenderLoading(28);

	TexObj.LoadTexture("Textures\\Pcx\\White.bmp", &Textures.ChessPieces[WHITE], LINEAR_FILTER, NO_ALPHA_CHANNEL);
	TexObj.LoadTexture("Textures\\Pcx\\Black.bmp", &Textures.ChessPieces[BLACK], LINEAR_FILTER, NO_ALPHA_CHANNEL);
	RenderLoading(30);

	TexObj.LoadTexture("Textures\\Chessboard\\Marble.bmp", &Textures.ChessBoard[CHESSBOARD_CASES], LINEAR_FILTER, NO_ALPHA_CHANNEL);
	TexObj.LoadTexture("Textures\\Chessboard\\Chessboard Wood.bmp", &Textures.ChessBoard[CHESSBOARD_FRAME], LINEAR_FILTER, NO_ALPHA_CHANNEL);
	RenderLoading(35);

	if(!LiteMode){
		TexObj.LoadTexture("Textures\\Column\\Column.bmp", &Textures.Column, LINEAR_FILTER, NO_ALPHA_CHANNEL);
		RenderLoading(37);

		TexObj.LoadTexture("Textures\\Terrain\\Grass.bmp", &Textures.Terrain, LINEAR_FILTER, NO_ALPHA_CHANNEL);
		RenderLoading(40);

		for(int Cpt = 0; Cpt < NUM_ROCKS_TEXTURES; Cpt++){
			char c[256];
			sprintf(c, "Textures\\Rocks\\Rock%d.bmp", Cpt + 1);
			TexObj.LoadTexture(c, &Textures.Rocks[Cpt], LINEAR_FILTER, NO_ALPHA_CHANNEL);
			RenderLoading(40 + Cpt);
		}

		TexObj.LoadTexture("Textures\\Water\\Water.bmp", &Textures.Water, LINEAR_FILTER, NO_ALPHA_CHANNEL);
		RenderLoading(46);

		for(int Cpt = 0; Cpt < NUM_WATER_REFLECTIONS; Cpt++){
			char c[256];
			sprintf(c, "Textures\\Water\\Reflections\\Caust%d.bmp", Cpt);
			TexObj.LoadTexture(c, &Textures.WaterReclect[Cpt], LINEAR_FILTER, NO_ALPHA_CHANNEL);
		}
		RenderLoading(48);

		LoadSkyDomePrimaryTexture("Textures\\Sky\\Sky.Raw");
		TexObj.LoadTexture("Textures\\Sky\\Stars.bmp", &Textures.SkyDome, LINEAR_FILTER, NO_ALPHA_CHANNEL);
		RenderLoading(57);
	
		CloudsNoise.LoadPreProcessingData2("Textures\\Sky\\Noise.RAW");
		CloudsNoise.BindTexureToOpenGL(&Textures.SkyPlane[0], &Textures.SkyPlane[1]);
		CloudsNoise.StartBackgroundProcess();
		RenderLoading(60);

		TexObj.LoadTexture("Textures\\LensFlares\\SunGlow.bmp",     &Textures.LensFlares[0], LINEAR_FILTER, NO_ALPHA_CHANNEL);
		RenderLoading(61);
		TexObj.LoadTexture("Textures\\LensFlares\\BigGlow.bmp",     &Textures.LensFlares[1], LINEAR_FILTER, NO_ALPHA_CHANNEL);
		RenderLoading(62);
		TexObj.LoadTexture("Textures\\LensFlares\\Halo.bmp",        &Textures.LensFlares[2], LINEAR_FILTER, NO_ALPHA_CHANNEL);
		RenderLoading(63);
		TexObj.LoadTexture("Textures\\LensFlares\\Hexa.bmp",        &Textures.LensFlares[3], LINEAR_FILTER, NO_ALPHA_CHANNEL);
		RenderLoading(64);
		TexObj.LoadTexture("Textures\\LensFlares\\Streak.bmp",      &Textures.LensFlares[4], LINEAR_FILTER, NO_ALPHA_CHANNEL);
		RenderLoading(65);
		TexObj.LoadTexture("Textures\\LensFlares\\RainBowHalo.bmp", &Textures.LensFlares[5], LINEAR_FILTER, NO_ALPHA_CHANNEL);
	}
	RenderLoading(66);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::SetGameTextures()
{
	for(int CptRes = 0; CptRes < NUM_LOD_LEVEL; CptRes++){
		for(int Cpt = 0; Cpt < NUM_CHESSPIECES; Cpt++){
			Models.ChessPieces[CptRes][Cpt].SetPrimaryTexture(Textures.ChessPieces[WHITE]);
			Models.ChessPieces[CptRes][Cpt].SetAuxiliaryTexture(Textures.ChessPieces[BLACK]);
		}
	}

	Models.ChessBoard[CHESSBOARD_CASES].SetPrimaryTexture(Textures.ChessBoard[CHESSBOARD_CASES]);
	Models.ChessBoard[CHESSBOARD_TOP].SetPrimaryTexture(Textures.ChessBoard[CHESSBOARD_FRAME]);
	Models.ChessBoard[CHESSBOARD_BOTTOM].SetPrimaryTexture(Textures.ChessBoard[CHESSBOARD_FRAME]);
	
	if(!LiteMode){
		Models.Column.SetPrimaryTexture(Textures.Column);

		Models.Terrain[LO_RES].SetPrimaryTexture(Textures.Terrain);
		Models.Terrain[HI_RES].SetPrimaryTexture(Textures.Terrain);
		Models.Terrain[LO_RES].SetAuxiliaryTexture(Textures.SkyPlane[1]);
		Models.Terrain[HI_RES].SetAuxiliaryTexture(Textures.SkyPlane[1]);
	
		for(int Cpt = 0; Cpt < NUM_ROCKS_MODELS; Cpt++){
			static int RockTexIndx[NUM_ROCKS_MODELS] = {2, 1, 2, 2, 1, 3, 0, 0};
			Models.Rocks[Cpt].SetPrimaryTexture(Textures.Rocks[RockTexIndx[Cpt]]);
		}

		Models.Water[HI_RES].SetPrimaryTexture(Textures.Water);
		Models.Water[LO_RES].SetPrimaryTexture(Textures.Water);

		Models.SkyDome.SetAuxiliaryTexture(Textures.SkyDome);
	
		Models.SkyPlane.SetPrimaryTexture(Textures.SkyPlane[0]);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::FreeGameTextures()
{
	CTexture TexObj;

	TexObj.FreeTexture(&Textures.Menus.Arrow);
	for(int Cpt = 1; Cpt >= 0; Cpt--){
		TexObj.FreeTexture(&Textures.Menus.MainMenu[Cpt]);
		TexObj.FreeTexture(&Textures.Menus.NetworkMenu[Cpt]);
		TexObj.FreeTexture(&Textures.Menus.OptionsMenu[Cpt]);
		TexObj.FreeTexture(&Textures.Menus.AISetupMenu[Cpt]);
	}
	for(int Cpt = 3; Cpt >= 0; Cpt--)
		TexObj.FreeTexture(&Textures.Menus.ConnectMenu[Cpt]);

	for(int Cpt = 1; Cpt >= 0; Cpt--)
		TexObj.FreeTexture(&Textures.PromotePawn[Cpt]);

	TexObj.FreeTexture(&Textures.ChessPieces[WHITE]);
	TexObj.FreeTexture(&Textures.ChessPieces[BLACK]);
	TexObj.FreeTexture(&Textures.ChessBoard[CHESSBOARD_CASES]);
	TexObj.FreeTexture(&Textures.ChessBoard[CHESSBOARD_FRAME]);
	if(!LiteMode){
		TexObj.FreeTexture(&Textures.Column);
		TexObj.FreeTexture(&Textures.Terrain);
		for(int Cpt = NUM_ROCKS_TEXTURES - 1; Cpt >= 0; Cpt--)
			TexObj.FreeTexture(&Textures.Rocks[Cpt]);
		TexObj.FreeTexture(&Textures.Water);
		for(int Cpt = NUM_WATER_REFLECTIONS - 1; Cpt >= 0; Cpt--)
			TexObj.FreeTexture(&Textures.WaterReclect[Cpt]);
		TexObj.FreeTexture(&Textures.SkyDome);
		for(int Cpt = NUM_LENS_FLARES_TYPE - 1; Cpt >= 0; Cpt--)
			TexObj.FreeTexture(&Textures.LensFlares[Cpt]);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

bool CEcheck3DEngine::LoadGameModels()
{
	for(int CptRes = 0; CptRes < NUM_LOD_LEVEL; CptRes++){
		CModel *pChessPiecesModels[NUM_CHESSPIECES];
		for(int Cpt = 0; Cpt < NUM_CHESSPIECES; Cpt++)
			pChessPiecesModels[Cpt] = &Models.ChessPieces[CptRes][Cpt];

		CModelLoader ModelLoader;
		switch(CptRes)
		{
		case HI_RES: ModelLoader.LoadModelsSet("Models\\ChessPieces\\ChessPiecesHiRes.M3D", pChessPiecesModels[0], NUM_CHESSPIECES, true); break;
		case LO_RES: ModelLoader.LoadModelsSet("Models\\ChessPieces\\ChessPiecesLoRes.M3D", pChessPiecesModels[0], NUM_CHESSPIECES, true); break;
		}
	}
	RenderLoading(70);

	Models.ChessBoard[CHESSBOARD_CASES].Load("Models\\Chessboard\\Cases.M3D", false);
	Models.ChessBoard[CHESSBOARD_TOP].Load("Models\\Chessboard\\Chessboard_Top.M3D", true);
	Models.ChessBoard[CHESSBOARD_BOTTOM].Load("Models\\Chessboard\\Chessboard_Bottom.M3D", true);
	RenderLoading(73);
	
	if(!LiteMode){
		Models.Column.Load("Models\\Column\\Column.M3D", true);
		RenderLoading(75);

		//Models.Terrain[HI_RES].LoadModel("Models\\Terrain\\TerrainHiRes.M3D", true);
		Models.Terrain[HI_RES].Load("Models\\Terrain\\TerrainMidRes.M3D", true);
		RenderLoading(82);
		Models.Terrain[LO_RES].Load("Models\\Terrain\\TerrainLoRes.M3D", true);
		RenderLoading(83);

		{
			CModel *pRocksModels[NUM_ROCKS_MODELS];
			for(int Cpt = 0; Cpt < NUM_ROCKS_MODELS; Cpt++)
				pRocksModels[Cpt] = &Models.Rocks[Cpt];

			CModelLoader ModelLoader;
			ModelLoader.LoadModelsSet("Models\\Rocks\\Rocks.M3D", pRocksModels[0], NUM_ROCKS_MODELS, true);
		}

		RenderLoading(90);

		Models.Water[HI_RES].Load("Models\\Water\\WaterHiRes.M3D", true);
		RenderLoading(93);
		Models.Water[LO_RES].Load("Models\\Water\\WaterLoRes.M3D", true);
		RenderLoading(95);
	
		Models.SkyDome.Load("Models\\Sky\\SkyDome.M3D", true);
		RenderLoading(97);
		Models.SkyPlane.Load("Models\\Sky\\SkyPlane.M3D", true);
		RenderLoading(99);

		// Load 3d shadows models
		Shadows3D.Load("Models\\Shadows\\Static.svm",  &SVM.StaticObjs);
		{
			glObject *pChessPiecesModels[NUM_CHESSPIECES];
			for(int Cpt = 0; Cpt < NUM_CHESSPIECES; Cpt++)
				pChessPiecesModels[Cpt] = &SVM.ChessPieces[Cpt];
			Shadows3D.LoadSet("Models\\Shadows\\ChessPieces.svm", pChessPiecesModels[0], NUM_CHESSPIECES);
		}
	}

	SetGameTextures();
	RenderLoading(100);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::FreeGameModels()
{
	for(int CptRes = 0; CptRes < NUM_LOD_LEVEL; CptRes++){
		for(int Cpt = 0; Cpt <= NUM_CHESSPIECES; Cpt++)
			Models.ChessPieces[CptRes][Cpt].Free(false);
	}
	
	Models.ChessBoard[CHESSBOARD_CASES].Free(false);
	Models.ChessBoard[CHESSBOARD_TOP].Free(false);
	Models.ChessBoard[CHESSBOARD_BOTTOM].Free(false);
	
	if(!LiteMode){
		Models.Column.Free(false);

		//Models.Terrain[HI_RES].FreeModel(false);
		Models.Terrain[LO_RES].Free(false);
		Models.Terrain[HI_RES].Free(false);
	
		for(int Cpt = 0; Cpt <= NUM_ROCKS_MODELS; Cpt++)
			Models.Rocks[Cpt].Free(false);
	
		Models.Water[HI_RES].Free(false);
		Models.Water[LO_RES].Free(false);

		Models.SkyDome.Free(false);
		Models.SkyPlane.Free(false);

		Shadows3D.FreeObject(&SVM.StaticObjs);
		for(int Cpt = NUM_CHESSPIECES - 1; Cpt >= 0; Cpt--)
			Shadows3D.FreeObject(&SVM.ChessPieces[Cpt]);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::ResetGame()
{
	AI.pInit();
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::SetGameTime(int h, int m)
{
	GameTime = ((float)h * 60.0f) + (float)m;
	if(GameTime >= 1440.0f)
		GameTime = (float)((int)GameTime % 1440);
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::SetupFog()
{
	static float FogColor[] = {0.65f, 0.75f, 0.70f, 0.85f};
	if(IsFogExtSupported()){
		glFogi(GL_FOG_MODE, GL_LINEAR);										// Fog Fade Is Linear
		glFogfv(GL_FOG_COLOR, FogColor);									// Set The Fog Color
		glFogf(GL_FOG_START,  0.0f);										// Set The Fog Start
		glFogf(GL_FOG_END,    1.0f);										// Set The Fog End
		glHint(GL_FOG_HINT, GL_NICEST);										// Per-Pixel Fog Calculation
		glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT);		// Set Fog Based On Vertice Coordinates
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::SetSkyDomeAlpha()
{
	if(GameTime >= 330.0f && GameTime < 390.0f){
		SkyDomeAlpha = 1.0f - ((390.0f - GameTime) / 60.0f);  // Dawn
	} else if(GameTime >= 1110.0f && GameTime < 1170.0f){
		SkyDomeAlpha = (1170.0f - GameTime) / 60.0f;  // Dusk
	} else if(GameTime < 330.0f || GameTime >= 1170.0f){
		SkyDomeAlpha = 0.0f;    // Night
	} else {
		SkyDomeAlpha = 1.0f;    // Day
	}
	if(SkyDomeAlpha < 0.25f){SkyDomeAlpha = 0.25f;}
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::SetShadowMapAlpha()
{
	if(GameTime >= 360.0f && GameTime < 450.0f){
		ShadowMapAlpha = (float)sin(DEGTORAD(GameTime - 360.0f));  // Dawn
	} else if(GameTime >= 990.0f && GameTime < 1080.0f){
		ShadowMapAlpha = (float)cos(DEGTORAD(GameTime - 990.0f));  // Crépuscule
	} else if(GameTime < 360.0f || GameTime >= 1080.0f){
		ShadowMapAlpha = 0.0f;    // Night
	} else {
		ShadowMapAlpha = 1.0f;    // Day
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::UpdateSunPos()
{
	//float Angle = g_GameTime / 4.0f; <- Remetre ca comme ca apres!
	static float Angle = 0.0f;
	Angle = GameTime / 4.0f;

	//float Angle = g_GameTime / 4.0f; <- REmetre ca comme ca apres!
	SunPosition.Y = -SUN_RADIUS * cos(DEGTORAD(Angle));
	SunPosition.Z = -SUN_RADIUS * sin(DEGTORAD(Angle));
	SunPosition.X =  SUN_OFFSET * sin(DEGTORAD(Angle));
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::UpdateFog()
{
	if(IsFogExtSupported()){
		float FogColor[] = {0.65f * SkyDomeAlpha, 0.75f * SkyDomeAlpha, 0.70f * SkyDomeAlpha, 0.85f * SkyDomeAlpha};
		glFogfv(GL_FOG_COLOR, FogColor);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::GenerateShadowMatrix(float *pShadowMatrix)
{
 	static float3 BoardNormal = {0.0f, 1.0f, 0.0f};
	static float3 BoardPoint = {0.0f, 0.5f, 0.0f};
	float LightPos[4] = {SunPosition.X, SunPosition.Y, SunPosition.Z, 0.0f};

	float d = -BoardNormal.X*BoardPoint.X - BoardNormal.Y*BoardPoint.Y - BoardNormal.Z*BoardPoint.Z;
	float dot = BoardNormal.X*LightPos[0] + BoardNormal.Y*LightPos[1] + BoardNormal.Z*LightPos[2] + d*LightPos[3];

	ShadowMatrix[0]  = -LightPos[0]*BoardNormal.X + dot;
	ShadowMatrix[4]  = -LightPos[0]*BoardNormal.Y;
	ShadowMatrix[8]  = -LightPos[0]*BoardNormal.Z;
	ShadowMatrix[12] = -LightPos[0]*d;
	ShadowMatrix[1]  = -LightPos[1]*BoardNormal.X;
	ShadowMatrix[5]  = -LightPos[1]*BoardNormal.Y + dot;
	ShadowMatrix[9]  = -LightPos[1]*BoardNormal.Z;
	ShadowMatrix[13] = -LightPos[1]*d;
	ShadowMatrix[2]  = -LightPos[2]*BoardNormal.X;
	ShadowMatrix[6]  = -LightPos[2]*BoardNormal.Y;
	ShadowMatrix[10] = -LightPos[2]*BoardNormal.Z + dot;
	ShadowMatrix[14] = -LightPos[2]*d;
	ShadowMatrix[3]  = -LightPos[3]*BoardNormal.X;
	ShadowMatrix[7]  = -LightPos[3]*BoardNormal.Y;
	ShadowMatrix[11] = -LightPos[3]*BoardNormal.Z;
	ShadowMatrix[15] = -LightPos[3]*d + dot;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

float CEcheck3DEngine::ClampColor(float color)
{
	if(color > 1.0f)
		color = 1.0f;
	else if(color < 0.0f)
		color = 0.0f;
	return color;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void CE3DCamera::SetCam()
{
	// Set the camera position
	gluLookAt(0.0f,0.0f,ModCamDist, 0.0f,0.0f,0.0f, 0.0f,1.0f,0.0f);
	// Rotate the entire scene, giving us the impression that 
	// the camera is rotating around the scene
	glRotatef(GetYRotation(), 1.0f, 0.0f, 0.0f);
	glRotatef(GetXRotation(), 0.0f, 1.0f, 0.0f);
	// Translate the scene
	glTranslatef(GetXTranslation(), GetYTranslation(), GetZTranslation());

}

void CE3DCamera::CalcLensFlareScreenCenter(float *cx, float *cy, int CurView)
{
	float w, h;
	if(pEngine->FBO.IsEnabled()){
		w = pEngine->FBO.GetTexWidthf();
		h = pEngine->FBO.GetTexHeightf();
	} else {
		w = pEngine->GameWindow.GetWidthf();
		h = pEngine->GameWindow.GetHeightf();
	}

	if(!pEngine->UseDualView){
		*cx = w / 2.0f;
		*cy = h / 2.0f;
	} else {
		switch(pEngine->DualViewMode)
		{
		case DUAL_VIEW_MODE_H: 
			*cx = w / 2.0f; 
			*cy = CurView == 0 ? h * 0.75f : h * 0.25f; 
			break;
		case DUAL_VIEW_MODE_V: 
			*cx = CurView == 0 ? w * 0.25f : w * 0.75f; 
			*cy = h / 2.0f;  
			break;
		}
	}
}

void CE3DCamera::Set3DMode(int Loop)
{
	int w, h;
	int VPCoords[4];

	if(pEngine->FBO.IsEnabled()){
		w = pEngine->FBO.GetTexWidth();
		h = pEngine->FBO.GetTexHeight();
	} else {
		w = pEngine->GameWindow.GetWidth();
		h = pEngine->GameWindow.GetHeight();
	}

	if(!pEngine->UseDualView){
		VPCoords[0] = 0;
		VPCoords[1] = 0;
		VPCoords[2] = w;
		VPCoords[3] = h;
	} else {
		if(pEngine->DualViewMode == DUAL_VIEW_MODE_H){
			VPCoords[0] = 0;
			VPCoords[1] = Loop != 0 ? 0 : h / 2;
			VPCoords[2] = w;
			VPCoords[3] = h / 2;
		} else {
			VPCoords[0] = Loop == 0 ? 0 : w / 2;
			VPCoords[1] = 0;
			VPCoords[2] = w / 2;
			VPCoords[3] = h;
		}
	}

	pEngine->Set3DMode(NEAR_PLANE, FAR_PLANE, &VPCoords[0]);
}

////////////////////////////////////////////////////////////////////////////////////////////

void CE3DCamera::TimeLensFlaresFadeEffect(float ElapsedTime, float *pTimeLeft, float *pLensFlareAlpha)
{
	if(*pTimeLeft > 0.0f){
		*pTimeLeft -= ElapsedTime;
		if(*pTimeLeft <= 0.0f){
			*pTimeLeft = 0.0f;
			*pLensFlareAlpha = 1.0f;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::SelectCamera(int x, int y)
{
	if(!UseDualView)
		pCurrentCam = &SingleViewCamera;
	else {
		if(DualViewMode == DUAL_VIEW_MODE_H){
			if(y < GameWindow.GetHeight() / 2){
				pCurrentCam = &DualViewCamera[0];

				RECT rWindow;
				rWindow.left	= 0;
				rWindow.right	= GameWindow.GetWidth();
				rWindow.top	    = 0;
				rWindow.bottom	= GameWindow.GetHeight() / 2;
				//ClipCursor(&rWindow);
			} else {
				pCurrentCam = &DualViewCamera[1];

				RECT rWindow;
				rWindow.left	= 0;
				rWindow.right	= GameWindow.GetWidth();
				rWindow.top	    = GameWindow.GetHeight() / 2;
				rWindow.bottom	= GameWindow.GetHeight();
				//ClipCursor(&rWindow);
			}
		} else if(DualViewMode == DUAL_VIEW_MODE_V){
			if(x < GameWindow.GetWidth() / 2){
				pCurrentCam = &DualViewCamera[0];

				RECT rWindow;
				rWindow.left	= 0;
				rWindow.right	= GameWindow.GetWidth() / 2;
				rWindow.top	    = 0;
				rWindow.bottom	= GameWindow.GetHeight();
				//ClipCursor(&rWindow);
			} else {
				pCurrentCam = &DualViewCamera[1];

				RECT rWindow;
				rWindow.left	= GameWindow.GetWidth() / 2;
				rWindow.right	= GameWindow.GetWidth();
				rWindow.top	    = 0;
				rWindow.bottom	= GameWindow.GetHeight();
				//ClipCursor(&rWindow);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::InitCameras()
{
	CE3DCamera *pCam = NULL;
	for(int Cpt = 0; Cpt < 3; Cpt++){
		switch(Cpt)
		{
		case 0: pCam = &SingleViewCamera;  break;
		case 1: pCam = &DualViewCamera[0]; break;
		case 2: pCam = &DualViewCamera[1]; break;
		}

		switch(Cpt)
		{
		case 0: pCam->SetDefaultValue(NEAR_PLANE,FAR_PLANE,  0.0f,  60.0f, 0.0f,0.0f,0.0f,  4000.0f,  4.0f,500.0f,  0.0f,360.0f,-75.0f,90.0f); break;
		//case 0: pCam->SetDefaultValue(NEAR_PLANE,FAR_PLANE,  -19.5f,  2.5f, 0.0f,0.0f,0.0f,  10000.0f,  4.0f,500.0f,  0.0f,360.0f,-75.0f,90.0f); break;
		case 1: pCam->SetDefaultValue(NEAR_PLANE,FAR_PLANE,  0.0f,  60.0f, 0.0f,0.0f,0.0f,  4000.0f,  4.0f,500.0f,  0.0f,360.0f,-75.0f,90.0f); break;
		case 2: pCam->SetDefaultValue(NEAR_PLANE,FAR_PLANE,  180.0f,60.0f, 0.0f,0.0f,0.0f,  4000.0f,  4.0f,500.0f,  0.0f,360.0f,-75.0f,90.0f); break;
		}
		pCam->EnableLocks();

		pCam->InitializeE3DCamera();

		pCam->ModCamDist = pCam->GetDistance();
	}

	SingleViewCamera.vp.SetVP(0, GameWindow.GetWidth(), 0, GameWindow.GetHeight());
	SetDualViewMode(DualViewMode);
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::SetDualViewMode(int Mode)
{
	DualViewMode = Mode;

	CE3DCamera *pCam = NULL;
	for(int Cpt = 1; Cpt < 3; Cpt++){
		switch(Cpt)
		{
		case 1: pCam = &DualViewCamera[0]; break;
		case 2: pCam = &DualViewCamera[1]; break;
		}
		
		if(Mode == DUAL_VIEW_MODE_H){
			//pCam->SetDistance(4000.0f);
			//pCam->ModCamDist = pCam->GetDistance();

			switch(Cpt)
			{
			case 1: pCam->vp.SetVP(0, GameWindow.GetWidth(), GameWindow.GetHeight() / 2, GameWindow.GetHeight() / 2); break;
			case 2: pCam->vp.SetVP(0, GameWindow.GetWidth(), 0, GameWindow.GetHeight() / 2);                          break;
			}
		} else if(Mode == DUAL_VIEW_MODE_V){
			//pCam->SetDistance(5000.0f);
			//pCam->ModCamDist = pCam->GetDistance();

			switch(Cpt)
			{
			case 1: pCam->vp.SetVP(0, GameWindow.GetWidth() / 2, 0, GameWindow.GetHeight());                         break;
			case 2: pCam->vp.SetVP(GameWindow.GetWidth() / 2, GameWindow.GetWidth() / 2, 0, GameWindow.GetHeight()); break;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::ResetCameraPosition(CE3DCamera *pCam)
{
	pCam->SetDefaultValue(10.0f,50000.0f,  pCam != &DualViewCamera[1] ? 0.0f : 180.0f,60.0f,  0.0f,0.0f,0.0f,  4000.0f,  4.0f,500.0f,  0.0f,360.0f,-75.0f,90.0f);
	pCam->ModCamDist = pCam->GetDistance();
}

////////////////////////////////////////////////////////////////////////////////////////////

bool CEcheck3DEngine::CenterPcxOnBoard(int *pPosX, int *pPosY)
{
	for(int CptY = 0; CptY < ROW_COUNT; CptY++){
		for(int CptX = 0; CptX < COL_COUNT; CptX++){
			// If the Pcx is in this area
			if((CurrentSelPos.X >= PreCalc.PiecesPositionTable.BoardMinPosL[CptX] && CurrentSelPos.X <= PreCalc.PiecesPositionTable.BoardMaxPosL[CptX]) 
				 && 
				(CurrentSelPos.Y <= PreCalc.PiecesPositionTable.BoardMinPosT[CptY] && CurrentSelPos.Y >= PreCalc.PiecesPositionTable.BoardMaxPosT[CptY])){

				*pPosX = CptX;
				*pPosY = CptY;

				// This is a good case...
				return true;       
			}
		}
	}

	// Isn't a good case...
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::CalcCoordsForPicking(int x, int y, int *px, int *py, int *pw, int *ph)
{
	int w, h;
	if(pEngine->FBO.IsEnabled()){
		x = (int)((float)x * (pEngine->GameWindow.GetDevWidthf() / pEngine->GameWindow.GetWidthf()));
		y = (int)((float)y * (pEngine->GameWindow.GetDevHeightf() / pEngine->GameWindow.GetHeightf()));

		w = pEngine->FBO.GetTexWidth();
		h = pEngine->FBO.GetTexHeight();
	} else {
		w = pEngine->GameWindow.GetWidth();
		h = pEngine->GameWindow.GetHeight();
	}

	if(!pEngine->UseDualView){
		*px = x;
		*py = pEngine->FBO.IsEnabled() ? h : h - y;
	} else {
		int CurView = pCurrentCam != &DualViewCamera[1];

		switch(DualViewMode)
		{
		case DUAL_VIEW_MODE_H:
			*px = x;
			switch(CurView)
			{
			case 0: *py = pEngine->FBO.IsEnabled() ? h : h - y; break;
			case 1: *py = pEngine->FBO.IsEnabled() ? (h / 2) + y: (h / 2) - y; break;
			}
			break;
		case DUAL_VIEW_MODE_V:
			switch(CurView)
			{
			case 0: *px = x; break;
			case 1: *px = x - (w / 2); break;
			}
			*py = pEngine->FBO.IsEnabled() ? h : h - y;
			break;
		}
	}

	/*#ifndef FIX_VMWARE_PICKING_BUG
		*pw = 1;
		*ph = 1;
	#else*/
		*pw = w;
		*ph = h;
	//#endif
}

////////////////////////////////////////////////////////////////////////////////////////////

bool CEcheck3DEngine::GetMousePositionOnPlane(int x, int y, double *ObjX, double *ObjY, double *ObjZ)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat WinX, WinY, WinZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	int TrueX, TrueY, CurView;
	CurView = pCurrentCam != &DualViewCamera[1] ? 0 : 1;
	GetPickingInfo(x, y, &TrueX, &TrueY, CurView);
	WinX = (float)TrueX;
	WinY = (float)TrueY;

	//Get WinZ value 
	#ifndef FIX_VMWARE_PICKING_BUG
	//glReadPixels(int(WinX), int(WinY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &WinZ);
	glReadPixels(TrueX, TrueY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &WinZ);
	#else
	int w, h;
	GetScreenSize(&w, &h, CurView);

	float *fDepth = new float[w * h];
	glReadPixels(0, 0, w, h, GL_DEPTH_COMPONENT, GL_FLOAT, &fDepth[0]);
	WinZ = fDepth[(TrueY * w) + TrueX];
	SAFE_DELETE_ARRAY(fDepth);
	#endif

	//Check if the cursor is under the plane
	float PixelColor;
	#ifndef FIX_VMWARE_PICKING_BUG
	//glReadPixels(int(WinX), int(WinY), 1,1, GL_LUMINANCE, GL_FLOAT, &PixelColor);
	glReadPixels(TrueX, TrueY, 1,1, GL_LUMINANCE, GL_FLOAT, &PixelColor);
	#else
	float *fPixels = new float[w * h];
	glReadPixels(0, 0, w, h, GL_LUMINANCE, GL_FLOAT, &fPixels[0]);
	PixelColor = fPixels[(TrueY * w) + TrueX];
	SAFE_DELETE_ARRAY(fPixels);
	#endif

	if(PixelColor == 0.0f)
		return false;
	
	//Get the 3D position of the object relative to the cursor position
	gluUnProject(WinX, WinY, WinZ, modelview, projection, viewport, ObjX, ObjY, ObjZ);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::GetScreenSize(int *w, int *h, int CurView)
{
	if(pEngine->FBO.IsEnabled()){
		*w = pEngine->FBO.GetTexWidth();
		*h = pEngine->FBO.GetTexHeight();
	} else {
		*w = pEngine->GameWindow.GetWidth();
		*h = pEngine->GameWindow.GetHeight();
	}
}

void CEcheck3DEngine::GetFrameBufferSize(int *w, int *h, int CurView)
{
	GetScreenSize(w, h, CurView);

	if(UseDualView){
		switch(DualViewMode)
		{
		case DUAL_VIEW_MODE_H: *h /= 2; break;
		case DUAL_VIEW_MODE_V: *w /= 2; break;	
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::GetPickingInfo(int InX, int InY, int *OutX, int *OutY, int CurView)
{
	int w, h;
	GetFrameBufferSize(&w, &h, CurView);

	if(pEngine->FBO.IsEnabled()){
		InX = (int)((float)InX * (GameWindow.GetDevWidthf() / GameWindow.GetWidthf()));
		InY = (int)((float)InY * (GameWindow.GetDevHeightf() / GameWindow.GetHeightf()));
	}

	*OutX = InX;
	*OutY = h - InY;
	if(UseDualView && DualViewMode == DUAL_VIEW_MODE_H && CurView == 0)
		*OutY += h;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::OnKeyDown(UINT KeyPressed)
{
	if(!IsGameEngineInitialized)
		return;

	// Safety...
	if(KeyPressed == 'q' || KeyPressed == 'Q'){
		PostQuitMessage(0);
		return;
	}

	if(!Menus.IsMenuVisible()){
		if(!InEditingChatMsg){
			switch(KeyPressed)
			{
			case VK_ESCAPE:
				Menus.ShowMainMenu();
				break;		
			case VK_BACK:
				AI.pProcessAICmd("undo");
				break;		
			case 189:
				if(TimeScale > 0)
					TimeScale--;
				break;		
			case 187:
				if(TimeScale < 20)
					TimeScale++;
				break;		
			case '1':
				UseDualView = false;
				break;		
			case '2':
				if(GameType == GAME_TYPE_LOCAL){
					UseDualView = true;
					SetDualViewMode(DUAL_VIEW_MODE_H);
				}
				break;		
			case '3':
				if(GameType == GAME_TYPE_LOCAL){
					UseDualView = true;
					SetDualViewMode(DUAL_VIEW_MODE_V);
				}
				break;		
			case 'B':
				Menus.Options.ShowChessboard = !Menus.Options.ShowChessboard;
				break;		
			case 'C':
				Menus.Options.ShowClouds = !Menus.Options.ShowClouds;
				break;		
			case 'D':
				ShowDebugInfo = !ShowDebugInfo;
				break;		
			case 'F':
				Menus.Options.ShowFog = !Menus.Options.ShowFog;
				break;		
			case 'H':
				Menus.Options.ShowShadows = !Menus.Options.ShowShadows;
				break;		
			case 'I':
				ForceLinesRendering = !ForceLinesRendering;
				break;		
			case 'K':
				Menus.Options.ShowRocks = !Menus.Options.ShowRocks;
				break;		
			case 'L':
				Menus.Options.ShowLensFlares = !Menus.Options.ShowLensFlares;
				break;		
			case 'M':
				Menus.Options.ShowNextMoves = !Menus.Options.ShowNextMoves;
				break;		
			case 'O':
				Menus.Options.ShowColumn = !Menus.Options.ShowColumn;
				break;		
			case 'P':
				Menus.Options.ShowChessPieces = !Menus.Options.ShowChessPieces;
				break;		
			case 'R':
				Menus.Options.ShowReflections = !Menus.Options.ShowReflections;
				break;		
			case 'S':
				Menus.Options.UseShader = !Menus.Options.UseShader;
				break;		
			case 'T':
				Menus.Options.ShowTerrain = !Menus.Options.ShowTerrain;
				break;		
			case 'U':
				Menus.Options.UseLODModels = !Menus.Options.UseLODModels;
				break;		
			case 'W':
				Menus.Options.ShowWater = !Menus.Options.ShowWater;
				break;		
			case 'Y':
				Menus.Options.ShowSky = !Menus.Options.ShowSky;
				break;		
			case 222: // Tildee
				{
					static bool UseVSync = true;
					UseVSync = !UseVSync;
					SetVSync(UseVSync);
					break;		
				}
			}
		} else if(GameType == GAME_TYPE_NETWORKED){ 
			if(KeyPressed == VK_ESCAPE){
				InEditingChatMsg = false;
				pChatMsgMemNode = NULL;
			} else if(KeyPressed == VK_DOWN || KeyPressed == VK_UP){
				if(KeyPressed == VK_UP){
					if(pChatMsgMemNode == NULL){
						pChatMsgMemNode = ChatMsgList.ClientChatMsgList.GetLastNode();
					} else {
						CClientChatMsgNode *pTmpNode = ChatMsgList.ClientChatMsgList.GetPrevNode(pChatMsgMemNode);
						if(pTmpNode != NULL)
							pChatMsgMemNode = pTmpNode;
					}
				} else {
					if(pChatMsgMemNode != NULL){
						CClientChatMsgNode *pTmpNode = ChatMsgList.ClientChatMsgList.GetNextNode(pChatMsgMemNode);
						if(pTmpNode != NULL)
							ChatMsgBuffer.Reset();

						pChatMsgMemNode = pTmpNode;
					}
				}
				if(pChatMsgMemNode != NULL){
					ChatMsgBuffer.Reset();
					ChatMsgBuffer.AddSring(&pChatMsgMemNode->pText[0]);
				}
			}
		}
	} else { // if menu visible...
		Menus.ProcessKeyDownMsg(KeyPressed);
		if(Menus.NetworkSettings.EditingMode == EDITING_IP || Menus.NetworkSettings.EditingMode == EDITING_PORT){
			if(KeyPressed == VK_BACK){
				if(Menus.NetworkSettings.EditingMode == EDITING_IP){
					int Len = strlen(&Menus.NetworkSettings.ip[0]);
					if(Len > 0)
						Menus.NetworkSettings.ip[Len - 1] = NULL;
				} else if(Menus.NetworkSettings.EditingMode == EDITING_PORT){
					int Len = strlen(&Menus.NetworkSettings.Port[0]);
					if(Len > 0)
						Menus.NetworkSettings.Port[Len - 1] = NULL;
				}
			}
		}
	}	
}

void CEcheck3DEngine::OnChar(UINT KeyPressed)
{
	if(!IsGameEngineInitialized)
		return;

	// If the menu is visible, process the keys pressed into this functions
	if(Menus.IsMenuVisible() && KeyPressed >= 32){
		Menus.ProcessCharMsg(KeyPressed);
	} else {
		if(GameType == GAME_TYPE_NETWORKED){
			if(!InEditingChatMsg){
				if((KeyPressed == VK_RETURN || KeyPressed == 'T' || KeyPressed == 't')/* && !HasReturnKeyBeenProcessed*/){
					//ChatCursorAnimTimer = 0.0f;
					//IsChatCursorVisible = true;

					ChatMsgBuffer.Reset();
					InEditingChatMsg = true;
				} 	
			} else {
				if(KeyPressed >= 32){
					ChatMsgBuffer.AddChar((char)KeyPressed);
					//IsChatCursorVisible = true;
					//ChatCursorAnimTimer = 0.0f;
				} else if(KeyPressed == VK_BACK){
					ChatMsgBuffer.RemChar();
					//IsChatCursorVisible = true;
					//ChatCursorAnimTimer = 0.0f;
				} else if(KeyPressed == VK_RETURN/* && !HasReturnKeyBeenProcessed*/){
					int szLen = strlen(ChatMsgBuffer.GetBuffer());
					if(szLen > 0){
						char *pChatMsgBuffer = ChatMsgBuffer.GetBuffer();
						NetClient.SendChatMsg(ClientID, &Menus.NetworkSettings.Name[0], ChatMsgBuffer.GetBuffer());
					}

					InEditingChatMsg = false;
					pChatMsgMemNode = NULL;
				}
			}
		}
	}	
}

int CEcheck3DEngine::ProcessMouseMoveMsgForPromotionMenu(float x, float y)
{
	float ScrRatio[2] = {GameWindow.GetWidthDevRatio(), GameWindow.GetHeightDevRatio()};
	float BtnCoords[4][4] = {
		{298.0f, 298.0f + 100.0f, 333.0f, 333.0f + 97.0f}, 
		{408.0f, 408.0f + 100.0f, 333.0f, 333.0f + 97.0f}, 
		{516.0f, 516.0f + 100.0f, 333.0f, 333.0f + 97.0f}, 
		{624.0f, 624.0f + 100.0f, 333.0f, 333.0f + 97.0f}, 
	};
	
	for(int Cpt = 0; Cpt < 4; Cpt++){
		if(IsInSquare(x, y, BtnCoords[Cpt][0], BtnCoords[Cpt][1], BtnCoords[Cpt][2], BtnCoords[Cpt][3], ScrRatio[0], ScrRatio[1]))
			return Cpt;
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////

void CEcheck3DEngine::OnMouseDown(int ButtonPressed, int x, int y)
{
	if(!IsGameEngineInitialized)
		return;

	if(!Menus.IsMenuVisible()){
		//Did we have presed the left mouse button and did the cpu has finished moving is pcx?
		if(ButtonPressed == LEFT_MOUSE_BUTTON/* && CpuMoveTimeLeft == 0.0f*/){

			if(!IsPromotePawnDlgVisible){
				//We'll igore the rest if the AI is "thinking"
				if(AI.pIsAIThreadRunning()){return;}
			
				SelectCamera(x, y);

				//Pick a pcx
				CurrentSel = RenderChessPiecesForPicking(x, y, Menus.Options.UseLODModels);
			
				//Did we selected a pcx?
				if(CurrentSel > -1){
					//Generate the ghost(move possibility)
					AI.pGenGhostBoard(CurrentSel);
					//Here we set CurrentSelPos to the picked pcx XY Coordinates
					CurrentSelPos.X = PreCalc.PiecesPositionTable.BoardPosL[CurrentSel % 8];
					CurrentSelPos.Y = PreCalc.PiecesPositionTable.BoardPosT[CurrentSel / 8];
				}
			} else {
			}
		} else if(ButtonPressed == MIDDLE_MOUSE_BUTTON) {
			//Reset the camera position
			SelectCamera(x, y);
			ResetCameraPosition(pCurrentCam);
		} else if(ButtonPressed == RIGHT_MOUSE_BUTTON){ 
			SelectCamera(x, y);
		}
	} else {
		Menus.ProcessMouseClickMsg(x, y, GameWindow.GetWidthDevRatio(), GameWindow.GetHeightDevRatio(), GameWindow.IsWideScreen());
	}
}

void CEcheck3DEngine::OnMouseMoveExt(int x, int y, int XDisp, int YDisp)
{
	if(!IsGameEngineInitialized)
		return;

	if(!Menus.IsMenuVisible()){
		if(GetMouseBtnStatus(RIGHT_MOUSE_BUTTON)){
			//Camera.IncRotation((float)XDisp, (float)YDisp);
			// Rotate the camera
			if(pCurrentCam != NULL){
				pCurrentCam->IncRotation((float)XDisp / SingleViewCamera.GetRotationSpeed(), (float)YDisp / SingleViewCamera.GetRotationSpeed());			
				// Ajust the camera relative to the terrain height
				if(!LiteMode)
					RenderTerrainForCamCollision(Menus.Options.UseLODModels);
			}
		} else if(GetMouseBtnStatus(LEFT_MOUSE_BUTTON)){
			if(pCurrentCam != NULL){
				if(CurrentSel != NO_PCX_SELECTED)
					RenderPlaneForPcxCoordinate(); 
			}
		}
	} else {
		Menus.ProcessMouseMoveMsg(x, y, GameWindow.GetWidthDevRatio(), GameWindow.GetHeightDevRatio(), GameWindow.IsWideScreen());
	}
}

void CEcheck3DEngine::OnMouseUp(int ButtonPressed, int x, int y)
{
	if(!IsGameEngineInitialized)
		return;

	if(!Menus.IsMenuVisible()){
		pCurrentCam = NULL;

		//Did we have presed the left mouse button and is there a pcx selected?
		if(ButtonPressed == LEFT_MOUSE_BUTTON && CurrentSel > -1){
				
			if(IsPromotePawnDlgVisible){
				int Res = ProcessMouseMoveMsgForPromotionMenu((float)x, (float)y);

				if(Res > -1){
					IsPromotePawnDlgVisible = false;
		
					switch(Res)
					{
					case 0: PromotionMove[4] = 'r'; break;
					case 1: PromotionMove[4] = 'n'; break;
					case 2: PromotionMove[4] = 'b'; break;
					case 3: PromotionMove[4] = 'R'; break;
					}
			
					// Remake our move with the promotion info
					AI.pProcessAICmd((char*)&PromotionMove[0]);

					// Send data throught the network
					if(GameType == GAME_TYPE_NETWORKED){
						NetClient.SendMoveMsg((char*)&PromotionMove[0]);
					}

					// Send to the AI a command to check if the AI must play next then, if so, start thinking...
					AI.pProcessAICmd("CPU_Check");

					// Unselect the pcx
					CurrentSel = NO_PCX_SELECTED;
					CurrentSelPos.X = 0.0f;
					CurrentSelPos.Y = 0.0f;
					AI.pGenGhostBoard(CurrentSel);
				}
				return;
			}

			//Hold the new XY coordinates of the moved pcx
			int NewPosX, NewPosY;

			//Check if this is a valid position and "Center" the pcx, if yes...
			if(CenterPcxOnBoard(&NewPosX, &NewPosY)){
			
				//We pass command to the AI with chars containing a pcx move 
				//or with one of the keywords:
				//"on"                     //Enable AI
				//"off"                    //Disable AI
				//"Set_CPU_To_WHITE"       //Set the AI to play white
				//"Set_CPU_To_BLACK"       //Set the AI to play black
				//"Set_CPU_To_NO_COLOR"    //Set the AI to don't play
				//"Set_CPU_To_BOTH"        //Set the AI to play both white and black
				//"CPU_Check"              //Force the AI to play a move for the current turn
				//"new"                    //Init the game(same as pInit())
				//"undo"                   //Takeback the last move
				//"clear"                  //Remove all pcx on the board
				//"nocastle"               //Disable castle
				//"@"                      //This command is used to edit the board, see below...
				//                         //format: (White->pbnrqk|Black->PBNRQK) example -> "put B@a3" - white bishop at a3 or "put Q@d7" - black queen at d7

			
				char AICmd[32];         //The command buffer...
				ZeroMemory(AICmd, 32);  //Init the buffer...

				AICmd[0] = (CurrentSel % 8) + 0x61;
				AICmd[1] = (CurrentSel / 8) + 0x31;
				AICmd[2] = NewPosX          + 0x61;
				AICmd[3] = NewPosY          + 0x31;
			
				int Turn = AI.pGetCurrentTurn();

				//Process our move command...
				switch(AI.pProcessAICmd(&AICmd[0]))
				{
				case MOVE_PUT_KING_IN_CHESS:
					break;

				case INVALID_CMD:
					if(!(AICmd[0] == AICmd[2] && AICmd[1] == AICmd[3]))
						SoundsFX.IllegalDrop.Play();
					break;
				case VALID_CMD:
					SoundsFX.Drop[Turn == WHITE ? BLACK : WHITE].Play();
					AI.pProcessAICmd("CPU_Check"); // Needed to play with a.i.
					if(GameType == GAME_TYPE_NETWORKED){
						NetClient.SendMoveMsg(&AICmd[0]);
					}
					break;

				case SHOW_PROMOTE_PCX_DLG:
					IsPromotePawnDlgVisible = true;
					PromoteColor = Turn;
					ZeroMemory(&PromotionMove[0], 8);
					memcpy(&PromotionMove[0], &AICmd[0], 4);
					return; // Skip the remaining lines of code
				}
			}

			//JmpAddMsg:
			// Clear the selected piece flag
			CurrentSel = NO_PCX_SELECTED;
			CurrentSelPos.X = 0.0f;
			CurrentSelPos.Y = 0.0f;
			AI.pGenGhostBoard(CurrentSel);
		}
	} else { // Menus...
	
	}
}

void CEcheck3DEngine::OnMouseRollExt(int RollCount)
{
	if(!IsGameEngineInitialized)
		return;

	if(!Menus.IsMenuVisible()){
		// Select the current camera
		if(!UseDualView){
			pCurrentCam = &SingleViewCamera;
		} else {
			//Get the cursor position, relative to window coordinate
			static POINT CursorPos;
			GetCursorPos(&CursorPos);
			ScreenToClient(GethWnd(), &CursorPos);
	
			switch(DualViewMode)
			{
			case DUAL_VIEW_MODE_H: pCurrentCam = &DualViewCamera[CursorPos.y < GameWindow.GetHeight() / 2 ? 0 : 1]; break;
			case DUAL_VIEW_MODE_V: pCurrentCam = &DualViewCamera[CursorPos.x < GameWindow.GetWidth()  / 2 ? 0 : 1]; break;
			}
		
		}

		// 
		float DistToAdd = (float)(-RollCount) * pCurrentCam->GetZoomSpeed();
		float NewTrueCamDist = pCurrentCam->GetDistance() + DistToAdd;

		if(NewTrueCamDist > 0.0f && NewTrueCamDist <= 10000.0f){
		
			// Change the true Camera Distance
			pCurrentCam->IncDistance(DistToAdd);

			// Save the old ModCamDist value
			float ModCamDistOffset = pCurrentCam->ModCamDist;

			// Change the mod Camera Distance
			if(!LiteMode)
				RenderTerrainForCamCollision(Menus.Options.UseLODModels);

			//Calculate the distance we've moved the camera
			ModCamDistOffset -= pCurrentCam->ModCamDist;
		}

		pCurrentCam = NULL;
	} else {
	}
}
