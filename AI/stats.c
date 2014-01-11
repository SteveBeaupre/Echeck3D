#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "data.h"
#include "protos.h"
#include "eval.h"
#include "hash.h"

long TTablePeeks=0;
long TTableHits=0;
long TTableMisses=0;


void StatsDump_TTable()
{
	printf("TTablePeeks: %d, TTableHits: %d, TTableMisses: %d\n",TTablePeeks,TTableHits, TTableMisses);
}

void TTableCrossSection()
{
	long count[40];
	int i,j;
	BOOL found = FALSE;
	long t;
	long elements=0;

	int start, finish;
	start = hply - 20;
	finish = hply + 20;
	if (start < 0) start = 0;

	for(i = start; i < finish; i++) {
		count[i] = 0;
		for(t = 0; t < INI_TTABLE_SIZE; t++) {
			if(ttable[t].zkey != 0) {
				if(ttable[t].depth == i) {
					count[i]++;
					elements++;
				}
			}
		}
	}

	for(j = start; j < finish; j++) {
		printf("hply %3d: %10d     ", j, count[j]);
		if(j%3 == 0) printf("\n");
	}
	printf("\nTotal entries: %d\tTable size: %d\tTable memory footprint: %d MB\n", elements, INI_TTABLE_SIZE, INI_TTABLE_MEMORY_MB );
	

}

void ClearOnePlyStats()
{
	TTablePeeks = 0;
	TTableHits = 0;
	TTableMisses = 0;
}