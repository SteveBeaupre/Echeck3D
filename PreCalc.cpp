#include "PreCalc.h"

CPreCalc::CPreCalc()
{
	WideScreenOffset = 0;
	pPPT = &PiecesPositionTable;
	BuildPositionTable();
	BuildMenusStuffs();
}

void CPreCalc::SetWideScreenOffset(int Offset)
{
	WideScreenOffset = Offset;
}

int CPreCalc::GetWideScreenOffset()
{
	return WideScreenOffset;
}

void CPreCalc::BuildMenusStuffs(int LeftOffset/* = 0*/)
{
	WideScreenOffset = LeftOffset;

	// Main Menu 
	{
		// Arrow
		const int NumSel = 6;
		MenusPrecalcData.MainMenu.NumSel = NumSel;
		float ArrowPos[NumSel][4] = {
			{LeftOffset + 245.0f, 371.0f, 64.0f, 32.0f}, 
			{LeftOffset + 245.0f, 423.0f, 64.0f, 32.0f}, 
			{LeftOffset + 245.0f, 476.0f, 64.0f, 32.0f}, 
			{LeftOffset + 245.0f, 530.0f, 64.0f, 32.0f}, 
			{LeftOffset + 245.0f, 584.0f, 64.0f, 32.0f}, 
			{LeftOffset + 245.0f, 637.0f, 64.0f, 32.0f}, 
		};
		memcpy(&MenusPrecalcData.MainMenu.Arrow[0][0], &ArrowPos[0][0], sizeof(float) * (NumSel * 4));
		
		// Mask
		float Mask[NumSel][4] = {
			{LeftOffset + 315.0f, 363.0f, 400.0f, 45.0f}, 
			{LeftOffset + 315.0f, 416.0f, 400.0f, 45.0f}, 
			{LeftOffset + 315.0f, 470.0f, 400.0f, 45.0f}, 
			{LeftOffset + 315.0f, 527.0f, 400.0f, 45.0f}, 
			{LeftOffset + 315.0f, 578.0f, 400.0f, 45.0f}, 
			{LeftOffset + 315.0f, 630.0f, 400.0f, 45.0f}, 
		};
		memcpy(&MenusPrecalcData.MainMenu.Mask[0][0], &Mask[0][0], sizeof(float) * (NumSel * 4));
		
		// Text
		float MouseInputCoords[NumSel][4] = {
			{LeftOffset + 315.0f, LeftOffset + 315.0f + 400.0f, 363.0f, 363.0f + 45.0f}, 
			{LeftOffset + 315.0f, LeftOffset + 315.0f + 400.0f, 416.0f, 416.0f + 45.0f}, 
			{LeftOffset + 315.0f, LeftOffset + 315.0f + 400.0f, 470.0f, 470.0f + 45.0f}, 
			{LeftOffset + 315.0f, LeftOffset + 315.0f + 400.0f, 527.0f, 527.0f + 45.0f}, 
			{LeftOffset + 315.0f, LeftOffset + 315.0f + 400.0f, 578.0f, 578.0f + 45.0f}, 
			{LeftOffset + 315.0f, LeftOffset + 315.0f + 400.0f, 630.0f, 630.0f + 45.0f}, 
		};
		memcpy(&MenusPrecalcData.MainMenu.MouseInputCoords[0][0], &MouseInputCoords[0][0], sizeof(float) * (NumSel * 4));
	}

	// Network Menu 
	{
		// Arrow
		const int NumSel = 5;
		MenusPrecalcData.NetworkMenu.NumSel = NumSel;
		float ArrowPos[NumSel][4] = {
			{LeftOffset + 245.0f, 371.0f, 64.0f, 32.0f}, 
			{LeftOffset + 245.0f, 503.0f, 64.0f, 32.0f}, 
			{LeftOffset + 245.0f, 591.0f, 64.0f, 32.0f}, 
			{LeftOffset + 788.0f, 658.0f, 64.0f, 32.0f}, 
			{LeftOffset + 788.0f, 698.0f, 64.0f, 32.0f}, 
		};
		memcpy(&MenusPrecalcData.NetworkMenu.Arrow[0][0], &ArrowPos[0][0], sizeof(float) * (NumSel * 4));

		// Arrow for server/client choice
		float NetTypeSelArrow[2][4] = {
			{LeftOffset + 271.0f, 411.0f, 64.0f, 32.0f}, 
			{LeftOffset + 272.0f, 451.0f, 64.0f, 32.0f}, 
		};
		memcpy(&MenusPrecalcData.NetworkMenu.NetTypeSelArrow[0][0], &NetTypeSelArrow[0][0], sizeof(float) * (2 * 4));

		// Maks #1
		float Mask1[NumSel][4] = {
			{LeftOffset + 308.0f, 360.0f, 340.0f, 51.0f}, 
			{LeftOffset + 308.0f, 496.0f, 340.0f, 42.0f}, 
			{LeftOffset + 308.0f, 587.0f, 113.0f, 39.0f}, 
			{LeftOffset + 847.0f, 656.0f, 148.0f, 34.0f}, 
			{LeftOffset + 848.0f, 697.0f, 147.0f, 33.0f}, 
		};		
		memcpy(&MenusPrecalcData.NetworkMenu.Mask1[0][0], &Mask1[0][0], sizeof(float) * (NumSel * 4));
		
		// Maks #2
		float Mask2[2][4] = {
			{LeftOffset + 374.0f, 412.0f, 96.0f, 31.0f}, 
			{LeftOffset + 374.0f, 451.0f, 96.0f, 31.0f}, 
		};		
		memcpy(&MenusPrecalcData.NetworkMenu.Mask2[0][0], &Mask2[0][0], sizeof(float) * (2 * 4));

		// Mask for server/client choice
		float NetTypeSelSettingsMask[2][4] = {
			{LeftOffset + 354.0f, 422.0f, 9.0f, 9.0f}, 
			{LeftOffset + 354.0f, 462.0f, 9.0f, 9.0f}, 
		};
		memcpy(&MenusPrecalcData.NetworkMenu.NetTypeSelSettingsMask[0][0], &NetTypeSelSettingsMask[0][0], sizeof(float) * (2 * 4));

		// Mouse Coordinates
		float MouseInputCoords[NumSel][4] = {
			{LeftOffset + 308.0f, LeftOffset + 308.0f + 340.0f, 360.0f, 360.0f + 51.0f}, 
			{LeftOffset + 308.0f, LeftOffset + 308.0f + 340.0f, 496.0f, 496.0f + 42.0f}, 
			{LeftOffset + 308.0f, LeftOffset + 308.0f + 113.0f, 587.0f, 587.0f + 39.0f}, 
			{LeftOffset + 847.0f, LeftOffset + 847.0f + 148.0f, 656.0f, 656.0f + 34.0f}, 
			{LeftOffset + 848.0f, LeftOffset + 848.0f + 147.0f, 697.0f, 697.0f + 33.0f}, 
		};
		memcpy(&MenusPrecalcData.NetworkMenu.MouseInputCoords[0][0], &MouseInputCoords[0][0], sizeof(float) * (NumSel * 4));
		
		// Mouse Coordinates for server/client choice
		float NetTypeSelMouseInputCoords[2][4] = {
			{LeftOffset + 374.0f, LeftOffset + 374.0f + 96.0f, 412.0f, 412.0f + 31.0f}, 
			{LeftOffset + 374.0f, LeftOffset + 374.0f + 96.0f, 451.0f, 451.0f + 31.0f}, 
		};
		memcpy(&MenusPrecalcData.NetworkMenu.NetTypeSelMouseInputCoords[0][0], &NetTypeSelMouseInputCoords[0][0], sizeof(float) * (2 * 4));
		
		// IP and Port box coords
		float IPBox[4] = {LeftOffset + 323.0f, LeftOffset + 323.0f + 239.0f, 541.0f, 541.0f + 28.0f};
		float PortBox[4] = {LeftOffset + 323.0f, LeftOffset + 323.0f + 83.0f, 630.0f, 630.0f + 28.0f};
		memcpy(&MenusPrecalcData.NetworkMenu.IPBox[0], &IPBox[0], sizeof(float) * 4);
		memcpy(&MenusPrecalcData.NetworkMenu.PortBox[0], &PortBox[0], sizeof(float) * 4);
	}
		
	// Options Menu 
	{
		// Arrow
		const int NumSel = 19;
		MenusPrecalcData.OptionsMenu.NumSel = NumSel;
		float ArrowPos[NumSel][4] = {
			{LeftOffset + 176.0f, 403.0f, 33.0f, 17.0f}, 
			{LeftOffset + 176.0f, 432.0f, 33.0f, 17.0f}, 
			{LeftOffset + 176.0f, 461.0f, 33.0f, 17.0f}, 
			{LeftOffset + 176.0f, 490.0f, 33.0f, 17.0f}, 
			{LeftOffset + 176.0f, 519.0f, 33.0f, 17.0f}, 
			{LeftOffset + 176.0f, 548.0f, 33.0f, 17.0f}, 
			{LeftOffset + 176.0f, 577.0f, 33.0f, 17.0f}, 
			{LeftOffset + 176.0f, 606.0f, 33.0f, 17.0f}, 
			{LeftOffset + 524.0f, 403.0f, 33.0f, 17.0f}, 
			{LeftOffset + 524.0f, 432.0f, 33.0f, 17.0f}, 
			{LeftOffset + 524.0f, 461.0f, 33.0f, 17.0f}, 
			{LeftOffset + 524.0f, 490.0f, 33.0f, 17.0f}, 
			{LeftOffset + 524.0f, 519.0f, 33.0f, 17.0f}, 
			{LeftOffset + 524.0f, 548.0f, 33.0f, 17.0f}, 
			{LeftOffset + 543.0f, 606.0f, 33.0f, 17.0f}, 
			{LeftOffset + 543.0f, 635.0f, 33.0f, 17.0f}, 
			{LeftOffset + 543.0f, 664.0f, 33.0f, 17.0f}, 
			{LeftOffset + 788.0f, 658.0f, 64.0f, 32.0f}, 
			{LeftOffset + 788.0f, 698.0f, 64.0f, 32.0f}, 
		};		
		memcpy(&MenusPrecalcData.OptionsMenu.Arrow[0][0], &ArrowPos[0][0], sizeof(float) * (NumSel * 4));

		float Mask[NumSel][4] = {
			{LeftOffset + 215.0f, 400.0f, 208.0f, 24.0f}, 
			{LeftOffset + 215.0f, 429.0f, 208.0f, 24.0f}, 
			{LeftOffset + 215.0f, 458.0f, 208.0f, 24.0f}, 
			{LeftOffset + 215.0f, 487.0f, 208.0f, 24.0f}, 
			{LeftOffset + 215.0f, 516.0f, 208.0f, 24.0f}, 
			{LeftOffset + 215.0f, 545.0f, 208.0f, 24.0f}, 
			{LeftOffset + 215.0f, 574.0f, 208.0f, 24.0f}, 
			{LeftOffset + 215.0f, 603.0f, 208.0f, 24.0f}, 
			{LeftOffset + 566.0f, 397.0f, 208.0f, 24.0f}, 
			{LeftOffset + 566.0f, 426.0f, 208.0f, 24.0f}, 
			{LeftOffset + 566.0f, 455.0f, 208.0f, 24.0f}, 
			{LeftOffset + 566.0f, 484.0f, 208.0f, 24.0f}, 
			{LeftOffset + 566.0f, 513.0f, 208.0f, 24.0f}, 
			{LeftOffset + 566.0f, 542.0f, 208.0f, 24.0f}, 
			{LeftOffset + 566.0f, 604.0f, 208.0f, 24.0f}, 
			{LeftOffset + 566.0f, 632.0f, 208.0f, 24.0f}, 
			{LeftOffset + 566.0f, 660.0f, 208.0f, 24.0f}, 
			{LeftOffset + 871.0f, 658.0f, 110.0f, 35.0f}, 
			{LeftOffset + 871.0f, 698.0f, 110.0f, 35.0f}, 
		};
		memcpy(&MenusPrecalcData.OptionsMenu.Mask[0][0], &Mask[0][0], sizeof(float) * (NumSel * 4));

		const int NumMaskSel = NumSel - 2;
		float SettingsMask[NumMaskSel][4] = {
			{LeftOffset + 403.0f, 405.0f, 14.0f, 14.0f}, 
			{LeftOffset + 403.0f, 434.0f, 14.0f, 14.0f}, 
			{LeftOffset + 403.0f, 463.0f, 14.0f, 14.0f}, 
			{LeftOffset + 403.0f, 492.0f, 14.0f, 14.0f}, 
			{LeftOffset + 403.0f, 521.0f, 14.0f, 14.0f}, 
			{LeftOffset + 403.0f, 550.0f, 14.0f, 14.0f}, 
			{LeftOffset + 403.0f, 579.0f, 14.0f, 14.0f}, 
			{LeftOffset + 403.0f, 608.0f, 14.0f, 14.0f}, 
			{LeftOffset + 754.0f, 402.0f, 14.0f, 14.0f}, 
			{LeftOffset + 754.0f, 431.0f, 14.0f, 14.0f}, 
			{LeftOffset + 754.0f, 460.0f, 14.0f, 14.0f}, 
			{LeftOffset + 754.0f, 489.0f, 14.0f, 14.0f}, 
			{LeftOffset + 754.0f, 518.0f, 14.0f, 14.0f}, 
			{LeftOffset + 754.0f, 547.0f, 14.0f, 14.0f}, 
			{LeftOffset + 756.0f, 612.0f, 9.0f, 9.0f}, 
			{LeftOffset + 756.0f, 640.0f, 9.0f, 9.0f}, 
			{LeftOffset + 756.0f, 668.0f, 9.0f, 9.0f}, 
		};
		memcpy(&MenusPrecalcData.OptionsMenu.SettingsMask[0][0], &SettingsMask[0][0], sizeof(float) * (NumMaskSel * 4));

		float MouseInputCoords[NumSel][4] = {
			{LeftOffset + 215.0f, LeftOffset + 215.0f + 208.0f, 400.0f, 400.0f + 24.0f}, 
			{LeftOffset + 215.0f, LeftOffset + 215.0f + 208.0f, 429.0f, 429.0f + 24.0f}, 
			{LeftOffset + 215.0f, LeftOffset + 215.0f + 208.0f, 458.0f, 458.0f + 24.0f}, 
			{LeftOffset + 215.0f, LeftOffset + 215.0f + 208.0f, 487.0f, 487.0f + 24.0f}, 
			{LeftOffset + 215.0f, LeftOffset + 215.0f + 208.0f, 516.0f, 516.0f + 24.0f}, 
			{LeftOffset + 215.0f, LeftOffset + 215.0f + 208.0f, 545.0f, 545.0f + 24.0f}, 
			{LeftOffset + 215.0f, LeftOffset + 215.0f + 208.0f, 574.0f, 574.0f + 24.0f}, 
			{LeftOffset + 215.0f, LeftOffset + 215.0f + 208.0f, 603.0f, 603.0f + 24.0f}, 
			{LeftOffset + 566.0f, LeftOffset + 566.0f + 208.0f, 397.0f, 397.0f + 24.0f}, 
			{LeftOffset + 566.0f, LeftOffset + 566.0f + 208.0f, 426.0f, 426.0f + 24.0f}, 
			{LeftOffset + 566.0f, LeftOffset + 566.0f + 208.0f, 455.0f, 455.0f + 24.0f}, 
			{LeftOffset + 566.0f, LeftOffset + 566.0f + 208.0f, 484.0f, 484.0f + 24.0f}, 
			{LeftOffset + 566.0f, LeftOffset + 566.0f + 208.0f, 513.0f, 513.0f + 24.0f}, 
			{LeftOffset + 566.0f, LeftOffset + 566.0f + 208.0f, 542.0f, 542.0f + 24.0f}, 
			{LeftOffset + 566.0f, LeftOffset + 566.0f + 208.0f, 604.0f, 604.0f + 24.0f}, 
			{LeftOffset + 566.0f, LeftOffset + 566.0f + 208.0f, 632.0f, 632.0f + 24.0f}, 
			{LeftOffset + 566.0f, LeftOffset + 566.0f + 208.0f, 660.0f, 660.0f + 24.0f}, 
			{LeftOffset + 871.0f, LeftOffset + 871.0f + 110.0f, 658.0f, 658.0f + 35.0f}, 
			{LeftOffset + 871.0f, LeftOffset + 871.0f + 110.0f, 698.0f, 698.0f + 35.0f}, 
		};
		memcpy(&MenusPrecalcData.OptionsMenu.MouseInputCoords[0][0], &MouseInputCoords[0][0], sizeof(float) * (NumSel * 4));
	}

	// AI Setup Menu 
	{
		// Arrow
		const int NumSel = 4;
		MenusPrecalcData.AISetupMenu.NumSel = NumSel;
		float ArrowPos[NumSel][4] = {
			{LeftOffset + 245.0f, 302.0f, 64.0f, 32.0f}, 
			{LeftOffset + 245.0f, 368.0f, 64.0f, 32.0f}, 
			{LeftOffset + 245.0f, 436.0f, 64.0f, 32.0f}, 
			{LeftOffset + 607.0f, 700.0f, 64.0f, 32.0f}, 
		};		
		memcpy(&MenusPrecalcData.AISetupMenu.Arrow[0][0], &ArrowPos[0][0], sizeof(float) * (NumSel * 4));
		
		float Mask[NumSel][4] = {
			{LeftOffset + 320.0f, 292.0f, 380.0f, 54.0f}, 
			{LeftOffset + 320.0f, 356.0f, 317.0f, 62.0f}, 
			{LeftOffset + 320.0f, 424.0f, 316.0f, 60.0f}, 
			{LeftOffset + 674.0f, 693.0f, 310.0f, 46.0f}, 		
		};
		memcpy(&MenusPrecalcData.AISetupMenu.Mask[0][0], &Mask[0][0], sizeof(float) * (NumSel * 4));
		
		float MouseInputCoords[NumSel][4] = {
			{LeftOffset + 320.0f, LeftOffset + 320.0f + 380.0f, 292.0f, 292.0f + 54.0f}, 
			{LeftOffset + 320.0f, LeftOffset + 320.0f + 317.0f, 356.0f, 356.0f + 62.0f}, 
			{LeftOffset + 320.0f, LeftOffset + 320.0f + 316.0f, 424.0f, 424.0f + 60.0f}, 
			{LeftOffset + 674.0f, LeftOffset + 674.0f + 310.0f, 693.0f, 693.0f + 46.0f}, 
		};
		memcpy(&MenusPrecalcData.AISetupMenu.MouseInputCoords[0][0], &MouseInputCoords[0][0], sizeof(float) * (NumSel * 4));
	}
}

void CPreCalc::BuildPositionTable()
{
	for(int CptBoardPos = 0; CptBoardPos < ROW_COUNT; CptBoardPos++){	
		// Get the middle pos of the cases
		pPPT->BoardPosL[CptBoardPos] = -((SPACE_BETWEEN_PCX*(ROW_COUNT-1)/2) - (SPACE_BETWEEN_PCX * CptBoardPos));
		pPPT->BoardPosT[CptBoardPos] = ((SPACE_BETWEEN_PCX*(COL_COUNT-1)/2) - (SPACE_BETWEEN_PCX * CptBoardPos));
		// Get the min pos(left, top) of a case
		pPPT->BoardMinPosL[CptBoardPos] = pPPT->BoardPosL[CptBoardPos] - (SPACE_BETWEEN_PCX/2);
		pPPT->BoardMinPosT[CptBoardPos] = pPPT->BoardPosT[CptBoardPos] + (SPACE_BETWEEN_PCX/2);
		// Get the max pos(right, bottom) of a case
		pPPT->BoardMaxPosL[CptBoardPos] = pPPT->BoardPosL[CptBoardPos] + (SPACE_BETWEEN_PCX/2);
		pPPT->BoardMaxPosT[CptBoardPos] = pPPT->BoardPosT[CptBoardPos] - (SPACE_BETWEEN_PCX/2);
	}
}