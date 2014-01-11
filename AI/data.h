#include <windows.h> 

extern tile board[64];
extern unsigned __int64 board_key; // Zobrist key for current board position

extern char piece_char[7];
extern char rays[5][64][8][7];

extern char raynum[7];


extern int side;  /* the side to move */
extern int xside;  /* the side not to move */
extern int computer_side;
extern int castle;  /* a bitfield with the castle permissions. if 1 is set,
                white can still castle kingside. 2 is white queenside.
				4 is black kingside. 8 is black queenside. */
extern int enpassant;  /* the en passant square. if white moves e2e4, the en passant
            square is set to e3, because that's where a pawn would move
			in an en passant capture */
extern int fifty;  /* the number of moves since a capture or pawn move, used
               to handle the fifty-move-draw rule */
extern int ply;  /* the number of half-moves (ply) since the
             root of the search tree */
extern int hply;  /* h for history; the number of ply since the beginning
              of the game */

/* gen_dat is some memory for move lists that are created by the move
   generators. The move list for ply n starts at gen_begin[n] of gen_dat.
   gen_end[n] is right after the move list. */
extern mstack_t movestack[MOVE_STACK_SIZE];
extern int ms_begin[MAX_PLY], ms_end[MAX_PLY];
extern history_t history[HISTORY_STACK]; // game history

extern int castle_mask[64];

/* the engine will search for max_time milliseconds or until it finishes
   searching max_depth ply. */
extern long max_time;
extern int max_depth;

/* the time when the engine starts searching, and when it should stop */
extern DWORD start_time;
extern DWORD stop_time;



/* a "triangular" PV array */
extern move pv[MAX_PLY][MAX_PLY];
extern int pv_length[MAX_PLY];
extern BOOL follow_pv;

extern int hheuristic[64][64];

extern char logbuf[256]; // from log.c - logging text buffer

extern BOOL icc;