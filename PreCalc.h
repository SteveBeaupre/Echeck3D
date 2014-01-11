#ifndef _CPRECALC_H_
#define _CPRECALC_H_
#ifdef __cplusplus

#include <Windows.h>
#include "Stdio.h"
#include "SafeKill.h"

#define ROW_COUNT  8
#define COL_COUNT  8

#define SPACE_BETWEEN_PCX             274.25f 

struct CPiecesPositionTable {
	float BoardPosL[ROW_COUNT];
	float BoardPosT[COL_COUNT];
	float BoardMinPosL[ROW_COUNT];
	float BoardMinPosT[COL_COUNT];
	float BoardMaxPosL[ROW_COUNT];
	float BoardMaxPosT[COL_COUNT];
};

struct CMainMenuPrecalcData {
	int   NumSel;
	float Arrow[6][4];
	float Mask[6][4];
	float MouseInputCoords[6][4];
};

struct CNetworkMenuPrecalcData {
	int   NumSel;
	float Arrow[5][4];
	float NetTypeSelArrow[2][4];
	float Mask1[5][4];
	float Mask2[2][4];
	float NetTypeSelSettingsMask[2][4];
	float MouseInputCoords[5][4];
	float NetTypeSelMouseInputCoords[2][4];
	float IPBox[4];
	float PortBox[4];
};

struct COptionsMenuPrecalcData {
	int   NumSel;
	float Arrow[19][4];
	float Mask[19][4];
	float SettingsMask[17][4];
	float MouseInputCoords[19][4];
};

struct CAISetupMenuPrecalcData {
	int   NumSel;
	float Arrow[4][4];
	float Mask[4][4];
	float MouseInputCoords[4][4];
};

struct CMenusPrecalcData {
	CMainMenuPrecalcData       MainMenu;
	COptionsMenuPrecalcData    OptionsMenu;
	CNetworkMenuPrecalcData    NetworkMenu;
	//CConnectingMenuPrecalcData ConnectingMenu;
	CAISetupMenuPrecalcData    AISetupMenu;
};

class CPreCalc {
public:
	CPreCalc();
private:
	int  WideScreenOffset;
	CPiecesPositionTable *pPPT;
	void BuildPositionTable();
public:
	void BuildMenusStuffs(int LeftOffset = 0);
	void SetWideScreenOffset(int Offset);
	int  GetWideScreenOffset();
	//
	CMenusPrecalcData MenusPrecalcData;
	//Hold the case offset we need to now when we move and draw the pcx 
	CPiecesPositionTable PiecesPositionTable;
};

#endif
#endif //_CPRECALC_H_
