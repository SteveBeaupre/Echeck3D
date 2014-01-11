#include "ChatMsgList.h"

const float ServerMsgColor[3]  = {SYSTEM_TEXT_COLOR};
const float ClientMsgColor[3]  = {CLIENT_TEXT_COLOR};
const float Player1MsgColor[3] = {PLAYER_1_TEXT_COLOR};
const float Player2MsgColor[3] = {PLAYER_2_TEXT_COLOR};

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CChatMsgList::CChatMsgList()
{
	NumNodesAllocated = 0;
	CurrentNode = -1;
	InitRootNode();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CChatMsgList::~CChatMsgList()
{
	Clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CChatMsgList::InitRootNode()
{
	// Set the node index to -1
	Root.Index = -1;
	// Set the player id to 0
	Root.PlayerID = 0;
	// Erase the text buffer
	Root.pText = NULL;

	Root.Prev = NULL;
	Root.Next = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

UINT CChatMsgList::GetNodesCount()
{
	/*UINT Count = 0;
	CChatMsgNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	while(pNode->Next != NULL){
		pNode = pNode->Next;
		Count++;
	}

	// Return the number of nodes of this pile
	return Count;*/
	
	return NumNodesAllocated;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CChatMsgNode* CChatMsgList::GetNode(UINT Indx)
{
	UINT Count = 0;
	CChatMsgNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the one we want
	while(pNode->Next != NULL){
		pNode = pNode->Next;
		if(Count == Indx)
			return pNode;
		Count++;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CChatMsgNode* CChatMsgList::GetRootNode()
{
	return &Root;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CChatMsgNode* CChatMsgList::GetLastNode(bool CanReturnRoot)
{
	CChatMsgNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	while(pNode->Next != NULL)
		pNode = pNode->Next;

	if(CanReturnRoot)
		return pNode;

	// Return the last node pointer
	return pNode == &Root ? NULL : pNode;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CChatMsgList::SetNodeTxtColor(CChatMsgNode *pNode, const float *rgb)
{
	pNode->r = rgb[0];
	pNode->g = rgb[1];
	pNode->b = rgb[2];
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CChatMsgList::Push(UINT PlayerID, char* pName, char* pText)
{
	CChatMsgNode *pNode = GetLastNode(true);

	// Allocate the new node
	pNode->Next = new CChatMsgNode;
	NumNodesAllocated++;
	CChatMsgNode *pNewNode = pNode->Next;

	// Update pointers
	pNewNode->Next = NULL;
	pNewNode->Prev = pNode;

	// Save the node index
	pNewNode->Index = pNode->Index + 1;
	// If CurrentNode value == the previous last node index, then increment it
	if(CurrentNode == pNode->Index)
		CurrentNode++;

	// Save the player id
	pNewNode->PlayerID = PlayerID;

	// Calculate the buffers size
	UINT TextLen = 1 + strlen(pName) + 8 + strlen(pText) + 1;
	// Allocate the buffers
	pNewNode->pText = new char[TextLen];
	// Create the text
	sprintf(&pNewNode->pText[0], "<%s> Says: %s", pName, pText);

	// Set the text color
	switch(PlayerID)
	{
	case 1:  SetNodeTxtColor(pNewNode, &Player1MsgColor[0]); break;
	case 2:  SetNodeTxtColor(pNewNode, &Player2MsgColor[0]); break;
	default: SetNodeTxtColor(pNewNode, !strcmp(pName, "SERVER") ? &ServerMsgColor[0] : &ClientMsgColor[0]); break;
	}

	if(PlayerID == 1)
		ClientChatMsgList.Push(&pText[0]);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool CChatMsgList::Pop()
{
	CChatMsgNode *pNode = GetLastNode(true);

	// If CurrentNode value == the previous last node index, then decrement it
	if((pNode && pNode->Prev) && CurrentNode == pNode->Prev->Index)
		CurrentNode--;

	// If this the root node, we're done...
	if(pNode == NULL || pNode == &Root){return false;} // return faillure

	// Update pointers
	pNode->Prev->Next = NULL;

	// Delete allocated memory
	SAFE_DELETE_ARRAY(pNode->pText);

	//
	if(pNode->PlayerID == 1)
		ClientChatMsgList.Pop();


	// Delete the node
	SAFE_DELETE_OBJECT(pNode);
	NumNodesAllocated--;
	
	// return success
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CChatMsgList::Clear()
{
	// Delete everything...
	while(Pop());
	CurrentNode = -1;
	ClientChatMsgList.Clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

UINT CChatMsgList::GetCurrentNode()
{
	return CurrentNode;
}

void CChatMsgList::DecCurrentNode()
{
	if(CurrentNode > 0)
		CurrentNode--;
}

void CChatMsgList::IncCurrentNode()
{
	if(GetNodesCount() > 0 && CurrentNode < GetNodesCount()-1)
		CurrentNode++;
}
