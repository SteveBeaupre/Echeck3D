#ifndef _CCHATMSGBUFFER_H_
#define _CCHATMSGBUFFER_H_
#ifdef __cplusplus

#include <Windows.h>
#include "Stdio.h"
#include "SafeKill.h"

#define MEM_BLOCK_SIZE   512

class CChatMsgBuffer {
public:
	CChatMsgBuffer();
	~CChatMsgBuffer();
private:
	UINT  TextLen;
	UINT  BufferSize;
	char *pBuffer;

	void IncBufferSize();
	void DecBufferSize();
public:
	char* GetBuffer();
	
	void AddChar(char c);
	void RemChar();

	void AddSring(char *txt);

	void Reset();
};

#endif
#endif //_CCHATMSGBUFFER_H_
