#ifndef _CHAT_MSG_LIST_H_
#define _CHAT_MSG_LIST_H_
#ifdef __cplusplus

#define WIN32_LEAN_AND_MEAN // Trim libraies.
#define VC_LEANMEAN         // Trim even further.
#include "Windows.h"
#include "stdio.h"
#include <..\Common.h>
#include <SafeKill.h>
#include "ClientChatMsgList.h"

//----------------------------------------------------------------------//
// 
//----------------------------------------------------------------------//
#ifndef ROOT_NODE
	#define ROOT_NODE    -1
#endif

#define SERVER_TEXT_COLOR      1.0f,  1.0f,  0.0f
#define CLIENT_TEXT_COLOR      0.25f, 1.0f,  0.25f
#define PLAYER_1_TEXT_COLOR    0.75f, 1.0f,  0.75f
#define PLAYER_2_TEXT_COLOR    1.0f,  0.75f, 0.0f

//----------------------------------------------------------------------//
// 
//----------------------------------------------------------------------//
struct CChatMsgNode {
	// Hold the node Index
	UINT Index;

	// Hold the player ID
	UINT PlayerID;

	// Text Color
	float r,g,b;

	// Hold a line of text
	char *pText;

	struct CChatMsgNode *Prev;
	struct CChatMsgNode *Next;
};

//----------------------------------------------------------------------//
// 
//----------------------------------------------------------------------//
class CChatMsgList {
public:
	CChatMsgList();
	~CChatMsgList();
private:
	UINT NumNodesAllocated;
	UINT CurrentNode;

	CChatMsgNode Root;
	void InitRootNode();

	CChatMsgNode* GetRootNode();

	void SetNodeTxtColor(CChatMsgNode *pNode, const float *rgb);
public:
	CClientChatMsgList ClientChatMsgList;
public:
	CChatMsgNode* GetNode(UINT Indx);
	CChatMsgNode* GetLastNode(bool CanReturnRoot = false);
	UINT GetNodesCount();
	UINT GetNumAllocatedNodes(){return NumNodesAllocated;}

	UINT GetCurrentNode();
	void DecCurrentNode();
	void IncCurrentNode();
	
	void Push(UINT PlayerID, char* pName, char* pText);
	bool Pop();
	void Clear();
};

#endif
#endif //_CHAT_MSG_LIST_H_
