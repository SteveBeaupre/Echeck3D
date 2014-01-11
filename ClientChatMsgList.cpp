#include "ClientChatMsgList.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CClientChatMsgList::CClientChatMsgList()
{
	NumNodesAllocated = 0;
	InitRootNode();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CClientChatMsgList::~CClientChatMsgList()
{
	Clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CClientChatMsgList::InitRootNode()
{
	// Set the node index to -1
	Root.Index = -1;
	// Erase the text buffer
	Root.pText = NULL;

	Root.Prev = NULL;
	Root.Next = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

UINT CClientChatMsgList::GetNodesCount()
{
	/*UINT Count = 0;
	CClientChatMsgNode *pNode = &Root;
	
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

CClientChatMsgNode* CClientChatMsgList::GetNode(UINT Indx)
{
	UINT Count = 0;
	CClientChatMsgNode *pNode = &Root;
	
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

CClientChatMsgNode* CClientChatMsgList::GetRootNode()
{
	return &Root;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CClientChatMsgNode* CClientChatMsgList::GetLastNode(bool CanReturnRoot)
{
	CClientChatMsgNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	while(pNode->Next != NULL)
		pNode = pNode->Next;

	if(CanReturnRoot)
		return pNode;

	// Return the last node pointer
	return pNode == &Root ? NULL : pNode;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CClientChatMsgNode* CClientChatMsgList::GetPrevNode(CClientChatMsgNode* pNode)
{
	//
	if(pNode == NULL)
		return NULL;

	// Loop throught the nodes until we hit the last one
	if(pNode->Prev != NULL && pNode->Prev != &Root)
		return pNode->Prev;

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CClientChatMsgNode* CClientChatMsgList::GetNextNode(CClientChatMsgNode* pNode)
{
	if(pNode == NULL)
		return NULL;

	// Loop throught the nodes until we hit the last one
	if(pNode->Next != NULL)
		return pNode->Next;

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CClientChatMsgList::Push(char* pText)
{
	CClientChatMsgNode *pNode = GetLastNode(true);

	// Allocate the new node
	pNode->Next = new CClientChatMsgNode;
	NumNodesAllocated++;
	CClientChatMsgNode *pNewNode = pNode->Next;

	// Update pointers
	pNewNode->Next = NULL;
	pNewNode->Prev = pNode;

	// Save the node index
	pNewNode->Index = pNode->Index + 1;

	// Allocate the buffers
	pNewNode->pText = new char[strlen(pText)+1];
	// Create the text
	strcpy(&pNewNode->pText[0], pText);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool CClientChatMsgList::Pop()
{
	CClientChatMsgNode *pNode = GetLastNode(true);

	// If this the root node, we're done...
	if(pNode == NULL || pNode == &Root){return false;} // return faillure

	// Update pointers
	pNode->Prev->Next = NULL;

	// Delete allocated memory
	SAFE_DELETE_ARRAY(pNode->pText);

	// Delete the node
	SAFE_DELETE_OBJECT(pNode);
	NumNodesAllocated--;
	
	// return success
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CClientChatMsgList::Clear()
{
	// Delete everything...
	while(Pop());
}
