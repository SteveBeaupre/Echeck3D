#include "ChatMsgBuffer.h"

CChatMsgBuffer::CChatMsgBuffer()
{
	pBuffer = NULL;
	Reset();
}

CChatMsgBuffer::~CChatMsgBuffer()
{
	SAFE_DELETE_ARRAY(pBuffer);
}

char* CChatMsgBuffer::GetBuffer()
{
	return pBuffer;
}

void CChatMsgBuffer::IncBufferSize()
{
	UINT OldBufferSize = BufferSize;
	char *pTmpBuf = new char[OldBufferSize];
	memcpy(pTmpBuf, pBuffer, OldBufferSize);
	SAFE_DELETE_ARRAY(pBuffer);

	BufferSize += MEM_BLOCK_SIZE;
	pBuffer = new char[BufferSize];
	memcpy(pBuffer, pTmpBuf, OldBufferSize);
	ZeroMemory(&pBuffer[OldBufferSize], MEM_BLOCK_SIZE);

	SAFE_DELETE_ARRAY(pTmpBuf);
}

void CChatMsgBuffer::DecBufferSize()
{
	UINT NewBufferSize = BufferSize - MEM_BLOCK_SIZE;
	char *pTmpBuf = new char[NewBufferSize];
	memcpy(pTmpBuf, pBuffer, NewBufferSize);
	SAFE_DELETE_ARRAY(pBuffer);

	BufferSize -= MEM_BLOCK_SIZE;
	pBuffer = new char[BufferSize];
	memcpy(pBuffer, pTmpBuf, BufferSize);

	SAFE_DELETE_ARRAY(pTmpBuf);
}

void CChatMsgBuffer::AddSring(char *txt)
{
	int TextLen = strlen(txt);

	for(int Cpt = 0; Cpt < TextLen; Cpt++)
		AddChar(txt[Cpt]);
}

void CChatMsgBuffer::AddChar(char c)
{
	pBuffer[TextLen] = c;
	TextLen++;

	if(TextLen % MEM_BLOCK_SIZE == 0)
		IncBufferSize();
}

void CChatMsgBuffer::RemChar()
{
	if(TextLen > 0){
		TextLen--;
		pBuffer[TextLen] = 0;

		if((TextLen + 1) % MEM_BLOCK_SIZE == 0)
			DecBufferSize();
	}
}

void CChatMsgBuffer::Reset()
{
	SAFE_DELETE_ARRAY(pBuffer);

	TextLen = 0;
	BufferSize = MEM_BLOCK_SIZE;
	pBuffer = new char[BufferSize];
	ZeroMemory(pBuffer, MEM_BLOCK_SIZE);
}
