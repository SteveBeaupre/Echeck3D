#include "Echeck3DEngine.h"

class CGraphicsEngine : public CEcheck3DEngine {
public:
	bool CanRenderSky();
	bool CanRenderClouds();
	bool CanRenderTerrain();
	//bool CanRenderTrees();
	bool CanRenderRocks();
	bool CanRenderWater();
	bool CanRenderChessBoard();
	bool CanRenderColumn();
	bool CanRenderChessPieces();
	bool CanRenderChessPiecesNextMoves();
	bool CanRenderLensFlares();

	bool CanUseShaders();
	bool CanUseShadow();
	bool CanUseFog();
	bool ShowPiecesReflection();
	bool ShowGhostPieces();
	bool UseLodModels();
public:
	void Cast3DShadows(CE3DCamera *pCam);
	void CastPlanarShadow();
	void RenderShadowMap();
	void RenderSkyDome();
	void RenderSkyPlane();
	void RenderColumn();
	void RenderTerrain(BOOL LoRes);
	void RenderTerrainForCamCollision(BOOL LoRes);
	void RenderRocks();
	//void RenderTrees();
	void RenderWater(BOOL LoRes);
	void RenderChessBoard();
	void RenderPlaneForPcxCoordinate();
	void RenderChessPieces(BOOL LoRes);
	void RenderChessPiecesNextMoves(BOOL LoRes);
	void RenderReflectedPcx(BOOL LoRes);
	void RenderChessPiecesForPlanarShadowMapping(BOOL LoRes);
	int  RenderChessPiecesForPicking(int x, int y, BOOL LoRes);
	void RenderChessPiecesFor3DShadows(float *lp);
	int  DoLensFlareTests(CE3DCamera *pCam, bool ShowLine);
	void RenderLensFlares(CE3DCamera *pCam, int CurView);
	void LensFlareRenderer(CE3DCamera *pCam, int CurView);
	void RenderDebugInfo(CE3DCamera *pCam);
	void RenderPromotionMenu();
	void RenderMenus();
	void RenderLoadScreen(int PercentDone);
	void RenderGameMsg();
	void RenderChatMsg();
public:
	void RenderScene();
	void UpdateScene(float ElapsedTime);
};