#include "defs.h"
#include "data.h"
#include "protos.h"


tile board[64];	// board representation
unsigned __int64 board_key; // Zobrist key for current board position

BOOL icc = FALSE;	// we are playing on the ICC (Internet Chess Club)

int side;  /* the side to move */
int xside;  /* the side not to move */
int computer_side;
int castle;  /* a bitfield with the castle permissions. if 1 is set,
                white can still castle kingside. 2 is white queenside.
				4 is black kingside. 8 is black queenside. */
int enpassant;  /* the en passant square. if white moves e2e4, the en passant
            square is set to e3, because that's where a pawn would move
			in an en passant capture */
int fifty;  /* the number of moves since a capture or pawn move, used
               to handle the fifty-move-draw rule */
int ply;  /* the number of half-moves (ply) since the
             root of the search tree */
int hply;  /* h for history; the number of ply since the beginning
              of the game */

/* gen_dat is some memory for move lists that are created by the move
   generators. The move list for ply n starts at gen_begin[n] of gen_dat.
   gen_end[n] is right after the move list. */
mstack_t movestack[MOVE_STACK_SIZE];
int ms_begin[MAX_PLY], ms_end[MAX_PLY];
history_t history[HISTORY_STACK]; // game history

/* the history heuristic array (used for move ordering) */
// I gave it this name so, through repetative use of it, I can learn how to 
// spell "heuristic".
int hheuristic[64][64];


/* the engine will search for max_time milliseconds or until it finishes
   searching max_depth ply. */
long max_time = 10000000;
int max_depth = 6;

/* the time when the engine starts searching, and when it should stop */
DWORD start_time;
DWORD stop_time;



/* a "triangular" PV array */
move pv[MAX_PLY][MAX_PLY];
int pv_length[MAX_PLY];
BOOL follow_pv;




// precomputed piece move array
char rays[5][64][8][7];

/*	
	5 pieces
	64 squares
	8 possible directions
	7 possible squares along each


	saves square numbers
*/



char piece_char[7] = {	// matches #defs in defs.h
	'.', 'P', 'N', 'B', 'R', 'Q', 'K'
};

char raynum[7] = { //matches #defs in defs.h
	0,0,8,4,4,8,8
};


// Castle_mask implementation borrowed from Tom Kerrigan's Simple
// Chess Program

/* This is the castle_mask array. We can use it to determine
   the castling permissions after a move. What we do is
   logical-AND the castle bits with the castle_mask bits for
   both of the move's squares. Let's say castle is 1, meaning
   that white can still castle kingside. Now we play a move
   where the rook on h1 gets captured. We AND castle with
   castle_mask[63], so we have 1&14, and castle becomes 0 and
   white can't castle kingside anymore. */

// flipped
int castle_mask[64] = {
	13, 15, 15, 15, 12, 15, 15, 14,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	 7, 15, 15, 15,  3, 15, 15, 11
};


void InitRays()
{
	int x,y,i,d,dx,dy;
	int temp;
	// i = ray index
	// d = distance along ray


	// Ray Info - deltas
	int rookray[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
	int bishopray[4][2] = {{1,1},{-1,1},{-1,-1},{1,-1}};
	int queenray[8][2] = {{1,0},{-1,0},{0,1},{0,-1},{1,1},{-1,1},{-1,-1},{1,-1}};
	int knightray[8][2] = {{2,1}, {2,-1},{-2,1},{-2,-1},{1,2},{-1,2},{-1,-2},{1,-2}};

	// This code is very redundant looking, but compressing it would make it harder to debug


	// Rooks
	for(x = 0; x < 8; x++) {
		for(y = 0; y < 8; y++) {
			for(i = 0; i < ROOK_RAYS; i++) {
				for(d = 1; d < 8; d++) {
					dx = d*rookray[i][0];
					dy = d*rookray[i][1];
					temp = (x+dx)+(8*(y+dy));
					if(x+dx > 7 || x+dx < 0) temp = NO_RAY;
					if(y+dy > 7 || y+dy < 0) temp = NO_RAY;
					rays[ROOK-KNIGHT][x+(y*8)][i][d-1] = temp;
				}
			}
		}
	}
	
	// Bishops
	for(x = 0; x < 8; x++) {
		for(y = 0; y < 8; y++) {
			for(i = 0; i < BISHOP_RAYS; i++) {
				for(d = 1; d < 8; d++) {
					dx = d*bishopray[i][0];
					dy = d*bishopray[i][1];
					temp = (x+dx)+(8*(y+dy));
					if(x+dx > 7 || x+dx < 0) temp = NO_RAY;
					if(y+dy > 7 || y+dy < 0) temp = NO_RAY;
					rays[BISHOP-KNIGHT][x+(y*8)][i][d-1] = temp;
				}
			}
		}
	}

	// Queens
	for(x = 0; x < 8; x++) {
		for(y = 0; y < 8; y++) {
			for(i = 0; i < QUEEN_RAYS; i++) {
				for(d = 1; d < 8; d++) {
					dx = d*queenray[i][0];
					dy = d*queenray[i][1];
					temp = (x+dx)+(8*(y+dy));
					if(x+dx > 7 || x+dx < 0) temp = NO_RAY;
					if(y+dy > 7 || y+dy < 0) temp = NO_RAY;
					rays[QUEEN-KNIGHT][x+(y*8)][i][d-1] = temp;
				}
			}
		}
	}

	// Kings
	for(x = 0; x < 8; x++) {
		for(y = 0; y < 8; y++) {
			for(i = 0; i < KING_RAYS; i++) {
				for(d = 1; d < 8; d++) {
					dx = d*queenray[i][0];
					dy = d*queenray[i][1];
					temp = (x+dx)+(8*(y+dy));
					if(d != 1) temp = NO_RAY;
					if(x+dx > 7 || x+dx < 0) temp = NO_RAY;
					if(y+dy > 7 || y+dy < 0) temp = NO_RAY;
					rays[KING-KNIGHT][x+(y*8)][i][d-1] = temp;
				}
			}
		}
	}

	//Knights
	for(x = 0; x < 8; x++) {
		for(y = 0; y < 8; y++) {
			for(i = 0; i < KNIGHT_RAYS; i++) {
				for(d = 1; d < 8; d++) {
					dx = d*knightray[i][0];
					dy = d*knightray[i][1];
					temp = (x+dx)+(8*(y+dy));
					if(d != 1) temp = NO_RAY;
					if(x+dx > 7 || x+dx < 0) temp = NO_RAY;
					if(y+dy > 7 || y+dy < 0) temp = NO_RAY;
					rays[KNIGHT-KNIGHT][x+(y*8)][i][d-1] = temp;
				}
			}
		}
	}
	
}







