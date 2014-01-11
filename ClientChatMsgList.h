#ifndef _CLIENT_CHAT_MSG_LIST_H_
#define _CLIENT_CHAT_MSG_LIST_H_
#ifdef __cplusplus

#define WIN32_LEAN_AND_MEAN // Trim libraies.
#define VC_LEANMEAN         // Trim even further.
#include "Windows.h"
#include "stdio.h"
#include <..\Common.h>
#include <SafeKill.h>

//----------------------------------------------------------------------//
// 
//----------------------------------------------------------------------//
#ifndef ROOT_NODE
	#define ROOT_NODE    -1
#endif

#define SYSTEM_TEXT_COLOR      1.0f,  1.0f,  0.0f
#define PLAYER_1_TEXT_COLOR    0.75f, 1.0f,  0.75f
#define PLAYER_2_TEXT_COLOR    1.0f,  0.75f, 0.0f

//----------------------------------------------------------------------//
// 
//----------------------------------------------------------------------//
struct CClientChatMsgNode {
	// Hold the node Index
	UINT Index;

	// Hold a line of text
	char *pText;

	struct CClientChatMsgNode *Prev;
	struct CClientChatMsgNode *Next;
};

//----------------------------------------------------------------------//
// 
//----------------------------------------------------------------------//
class CClientChatMsgList {
public:
	CClientChatMsgList();
	~CClientChatMsgList();
private:
	UINT NumNodesAllocated;

	CClientChatMsgNode Root;
	void InitRootNode();

	CClientChatMsgNode* GetRootNode();
public:
	CClientChatMsgNode* GetNode(UINT Indx);
	CClientChatMsgNode* GetLastNode(bool CanReturnRoot = false);

	CClientChatMsgNode* GetPrevNode(CClientChatMsgNode* pNode);
	CClientChatMsgNode* GetNextNode(CClientChatMsgNode* pNode);

	UINT GetNodesCount();
	UINT GetNumAllocatedNodes(){return NumNodesAllocated;}

	void Push(char* pText);
	bool Pop();
	void Clear();
};

#endif
#endif //_CLIENT_CHAT_MSG_LIST_H_
