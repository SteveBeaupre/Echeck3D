#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>   
#include <iostream.h> 
#include <conio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h> 
#include <io.h>

// Useful for log files
char logbuf[256];

void WriteToLog(char *str) {
	FILE *stream;
	stream = fopen("log.txt","a+");
	fprintf(stream, "%s\n", str);
	fclose(stream);
}

