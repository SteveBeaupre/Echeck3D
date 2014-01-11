// Evaluation functions for Fimbulwinter

#include "defs.h"
#include "data.h"
#include "protos.h"
#include "eval.h"


int piece_val[7] = {	// matches #defs in defs.h
	0, PAWN_VALUE, KNIGHT_VALUE, BISHOP_VALUE, ROOK_VALUE, QUEEN_VALUE, KING_VALUE
};

int material[2]; // material totals for both sides
int position[2]; // positional score for each side
int score[2];    // total position score for both sides
BOOL endgame;	 // in endgame?


					  // These two arrays have extra indices so that we can use the same pawn eval code without
					//   worrying about boundary pawns.
int pawn_list[2][10]; // replace this later with piece lists for everything
int pawn_profile[2][10];
int pawn_i[2];       // how many pawns in 



int pawn_lv[64] = {					
	0,   0,   0,   0,   0,   0,   0,   0,				// BLACK
   50,  50,  50,  50,  50,  50,  50,  50,
   10,  10,  13,  15,  15,  13,  10,  10,
	7,   7,  11,  10,  10,  11,   7,   7,
	5,   3,  12,  15,  15,  12,   3,   5,
    8,   2,   8, - 5, - 5,   8,   2,   8,
	0,   0,   0, - 5, - 5,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0				// WHITE
};



int knight_lv[64] = {
  -10, -10, -10, -10, -10, -10, -10, -10,
  -10,   0,   0,   0,   0,   0,   0, -10,
  -10,   5,  10,  12,  12,  10,   5, -10,
  -10,   5,  12,  15,  15,  12,   5, -10,
  -10,   5,  12,  15,  15,  12,   5, -10,
  -10,   5,  10,  12,  12,  10,   5, -10,
  -10,   0,   0,   0,   0,   0,   0, -10,
  -10, -10, -10, -10, -10, -10, -10, -10
};

int bishop_lv[64] = {
  -10,  -5,  -5,  -5,  -5,  -5,  -5, -10,
   -5,   0,   0,   0,   0,   0,   0,  -5,
   -5,   4,   4,   5,   5,   4,   4,  -5,
   -5,   8,   7,  10,  10,   7,   8,  -5,
   -5,   5,   7,  10,  10,   7,   5,  -5,
   -5,  10,   4,   5,   5,   4,  10,  -5,
   -5,  20,   0,   0,   0,   0,  20,  -5,
  -10,  -5, -10,  -5,  -5, -10,  -5, -10
};


int rook_lv[64] = {
    0,   0,   0,   0,   0,   0,   0,   0,
   30,  30,  30,  30,  30,  30,  30,  30,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,  10,  10,  10,  10,   0,   0
};

int begin_king_lv[64] = {
  -50, -50, -50, -50, -50, -50, -50, -50,
  -50, -50, -50, -50, -50, -50, -50, -50,
  -50, -50, -50, -50, -50, -50, -50, -50,
  -50, -50, -50, -50, -50, -50, -50, -50,
  -30, -30, -30, -30, -30, -30, -30, -30,
  -25, -25, -25, -25, -25, -25, -25, -25,
  -15, -15, -15, -15, -15, -15, -15, -15,
	0,   0,  10, -15, -15,   0,  20,   0			// fix later by giving bonus for
};													// a castled king

int endgame_king_lv[64] = {
  -10,   0,   0,   0,   0,   0,   0, -10,
	0,   5,   7,  10,  10,   7,   5,   0,
	0,   7,  15,  17,  17,  15,   7,   0,
	0,  10,  17,  20,  20,  17,  10,   0,
	0,  10,  17,  20,  20,  17,  10,   0,
	0,   7,  15,  17,  17,  15,   7,   0,
	0,   5,   7,  10,  10,   7,   5,   0,
  -10,   0,   0,   0,   0,   0,   0, -10
};

int flip[64] = { // used to get lv values for white
	 56,  57,  58,  59,  60,  61,  62,  63,
	 48,  49,  50,  51,  52,  53,  54,  55,
	 40,  41,  42,  43,  44,  45,  46,  47,
	 32,  33,  34,  35,  36,  37,  38,  39,
	 24,  25,  26,  27,  28,  29,  30,  31,
	 16,  17,  18,  19,  20,  21,  22,  23,
	  8,   9,  10,  11,  12,  13,  14,  15,
	  0,   1,   2,   3,   4,   5,   6,   7
};

int EvaluateBoard() 
{
	int i;

	material[WHITE] = 0;
	material[BLACK] = 0;

	position[WHITE] = 0;
	position[BLACK] = 0;


	score[WHITE] = 0;
	score[BLACK] = 0;

	for(i = 0; i < 64; i++) {
		if(board[i].piece != EMPTY) {
				material[board[i].color] += piece_val[board[i].piece];			
		}
	}

	score[WHITE] = material[WHITE];
	score[BLACK] = material[BLACK];


	if(material[WHITE] < ENDGAME_THRESHOLD || material[BLACK] < ENDGAME_THRESHOLD) {
		endgame = TRUE;
	} else {
		endgame = FALSE;
	}

	for(i = 0; i < 64; i++) {
		if(board[i].piece != EMPTY) {
			switch(board[i].piece) {
				case PAWN:
					if(board[i].color == WHITE) {
						position[WHITE] += pawn_lv[flip[i]];
					} else {
						position[BLACK] += pawn_lv[i];
					}
					break;
				case KNIGHT:
					if(board[i].color == WHITE) {
						position[WHITE] += knight_lv[flip[i]];
					} else {
						position[BLACK] += knight_lv[i];
					}
					break;
				case BISHOP:
					if(board[i].color == WHITE) {
						position[WHITE] += bishop_lv[flip[i]];
					} else {
						position[BLACK] += bishop_lv[i];
					}
					break;
/*				case ROOK:
					if(board[i].color == WHITE) {
						position[WHITE] += rook_lv[flip[i]];
					} else {
						position[BLACK] += rook_lv[i];
					}
					break;
*/				case KING:
					if(endgame) {
						if(board[i].color == WHITE) {
							position[WHITE] += endgame_king_lv[flip[i]];
						} else {
							position[BLACK] += endgame_king_lv[i];
						}
					} else {
						if(board[i].color == WHITE) {
							position[WHITE] += begin_king_lv[flip[i]];
						} else {
							position[BLACK] += begin_king_lv[i];
						}
					}
					break;
				}	
		}
	}

	BuildPawnLists();
	position[WHITE] += PawnEval(WHITE);
	position[BLACK] += PawnEval(BLACK);

	score[WHITE] += position[WHITE];
	score[BLACK] += position[BLACK];

	if(endgame) {
		score[WHITE] += KingSafety(WHITE);
		score[BLACK] += KingSafety(BLACK);
	}

	if (side == WHITE)
		return score[WHITE] - score[BLACK]; 
	return score[BLACK] - score[WHITE];
}

int KingSafety(int s)
{
	// called in the endgame (when there are more CPU cycles to burn) to evaluate
	// king safety for side s. By giving penalties for poor mobility, hopefully Fimbulwinter
	// will be able to mate better when it has a material advantage
	int i,r;
	int xs = s^1;
	int safety = 0;

	for(i = 0; i < 64; i++) {
		if(board[i].piece == KING && board[i].color == s) break;
	}

	for(r = 0; r < raynum[KING]; r++) {							// King is:
			if(rays[KING-KNIGHT][i][r][0] == NO_RAY) safety-=5; // boxed in
			if(Attacked(rays[KING-KNIGHT][i][r][0],xs)) safety-=15; // attacked
			if(board[rays[KING-KNIGHT][i][r][0]].color == s) safety+=10; // next to own piece
	}

	return safety;
}

int PawnEval(int color)
{
	int i;
	int xcolor = color^1;
	int total = 0;
	if(pawn_i[color] <= 0) return 0;
	for(i = 1; i < 9; i++) {
		if(pawn_list[color][i] > 0) total += DOUBLED_PAWNS*(pawn_list[color][i]-1);

		// No pawn is an island unto itself...
		if(pawn_list[color][i] > 0 && pawn_list[color][i-1] <= 0 && pawn_list[color][i+1] <= 0) total += PAWN_ISLAND; 

		if(pawn_i[color] > 0 && pawn_i[xcolor] <= 0) total += PASSED_PAWN;

		if(pawn_profile[color][i-1] <= pawn_profile[color][i] + 1 && pawn_profile[color][i-1] >= pawn_profile[color][i] - 1) total += PAWN_CHAIN;

	}

	return total;
	
}

void BuildPawnLists()
{
	int i;
	pawn_i[WHITE] = 0;
	pawn_i[BLACK] = 0;

	memset(pawn_list, 0, sizeof(pawn_list));
	memset(pawn_profile, -2, sizeof(pawn_profile)); // really low value, so profile test fails

	for(i = 0; i < 64; i++) {
		if(board[i].piece == PAWN) {
			pawn_list[board[i].color][GETFILE(i)+1]++;
			++pawn_i[board[i].color];
			if(board[i].color == WHITE && pawn_profile[WHITE][GETFILE(i)+1] < i) pawn_profile[WHITE][GETFILE(i)+1] = GETRANK(i);
			if(board[i].color == BLACK && pawn_profile[BLACK][GETFILE(i)+1] < i) pawn_profile[BLACK][GETFILE(i)+1] = GETRANK(flip[i]);

		}
	}
}