#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "defs.h"
#include "data.h"
#include "protos.h"
#include "hash.h"
#include "stats.h"

/*
	Thanks to Bruce Moreland for the excellent discussion on Zobrist hashing
	http://www.seanet.com/%7Ebrucemo/topics/zobrist.htm

	Made it a lot easier to implement.
*/


unsigned __int64 ZobristKeys[6][2][64];
unsigned __int64 ZobristEPKeys[65]; // NO_EP_SQR is element 64
unsigned __int64 ZobristCastleKeys[16]; //15 is the largest value of castle, if all permission 
										//flags are set
unsigned __int64 BlackMoveKey;


// type of piece, color, square
// when accessing, remember to subtract one to the piece type on the board

int INI_TTABLE_MEMORY_MB = 0; // Set by INI file
int INI_TTABLE_SIZE	= 0;	  // derived from INI file setting	

hash_t *ttable = NULL;


void TTableInit(void)
{
	INI_TTABLE_SIZE = (int)((INI_TTABLE_MEMORY_MB*1000000.0)/(float)sizeof(hash_t));
	ttable = (hash_t *)malloc(INI_TTABLE_SIZE*sizeof(hash_t));
	if(ttable == NULL) {
		printf("Transposition table memory allocation failed!!!\n");
		exit(EXIT_FAILURE);
	}
	printf("Transposition table memory allocated.\n");
	printf("Table size: %d\tTable memory footprint: %d MB\n", INI_TTABLE_SIZE, INI_TTABLE_MEMORY_MB);
}

void TTableCleanup(void)
{
	if(ttable != NULL) free(ttable);
	printf("Transposition table memory freed.\n");
}


void TTableInsert(move BestMove, int depth, int eval, int hashflags)
{
	hash_t *target;

	target = &ttable[board_key % INI_TTABLE_SIZE];

    target->zkey = board_key;
    target->best = BestMove;
    target->eval = eval;
    target->flags = hashflags;
    target->depth = depth;
}

int TTableLookup(int depth, int alpha, int beta, move BestMove)
{
	
    hash_t *target = &ttable[board_key % INI_TTABLE_SIZE];

#ifdef STATS
	TTablePeeks++;
	TTableHits++;
#endif

    if (target->zkey == board_key) {
        if (target->depth >= depth) {
            if (target->flags == hashfEXACT)
                return target->eval;
            if ((target->flags == hashfALPHA) &&
                (target->eval <= alpha))
                return alpha;
            if ((target->flags == hashfBETA) &&
                (target->eval >= beta))
                return beta;
		}
       // RememberBestMove();
    }

#ifdef STATS
	TTableMisses++;
	TTableHits--;
#endif

    return EVAL_UNKNOWN;
}


unsigned __int64 ZobristKey()
{
	// returns the zobrist key for the current position
	int i;
	unsigned __int64 key = 0; // <--- VERY IMPORTANT! Seems VC6 doesn't catch uninited __int64s
	for(i = 0; i < 64; i++) {
		if(board[i].piece != EMPTY) {
			key ^= ZobristKeys[board[i].piece-PAWN][board[i].color][i];
		}
	}
	key ^= ZobristEPKeys[enpassant];
	key ^= ZobristCastleKeys[castle];
	if(side == BLACK) key ^= BlackMoveKey;	
	return key;
}

void InitZobristKeys()
{
	int x,y,z,i;
	srand(HASH_SEED); // important that this not change - needed for book I/O
	for(x = 0; x < 6; x++) {
		for(y = 0; y < 2; y++) {
			for(z = 0; z < 64; z++) {
				ZobristKeys[x][y][z] = rand64();
			}
		}
	}
	for(i = 0; i < 65; i++) {
		ZobristEPKeys[i] = rand64();
	}
	for(i = 0; i < 16; i++) {
		ZobristCastleKeys[i] = rand64();
	}
	BlackMoveKey = rand64();
}

void ClearTTable()
{
	memset(ttable,0,sizeof(hash_t)*INI_TTABLE_SIZE);
}

unsigned __int64 rand64()
{
	// returns a random number between 0 and 2^64 (1.8446744073710e+019)
	// Thanks to Bruce Moreland for this snippet
	
	// I modified this to make it generate the same random numbers in Debug and Release mode in MSVC++
	// It seems order of execution is not spec-ed out in C when all these are on one line.
	unsigned __int64 temp = rand();
	temp ^= ((unsigned __int64)rand() << 15);
	temp ^= ((unsigned __int64)rand() << 30);
	temp ^= ((unsigned __int64)rand() << 45);
	temp ^= ((unsigned __int64)rand() << 60);
	return temp;
}