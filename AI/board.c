#include <memory.h>
#include <assert.h>
#include "defs.h"
#include "data.h"
#include "protos.h"
#include "eval.h"
#include "hash.h"

void InitBoard()
{
	int x,y;
	memset(board,0,sizeof(tile)*64);

	for(x = 0; x < 8; x++) {
		for(y = 0; y < 8; y++) {
			if(y < 2) BOARD(x,y).color = WHITE;
			if(y > 5) BOARD(x,y).color = BLACK;
			if(!(y < 2) && !(y > 5)) BOARD(x,y).color = NO_COLOR;
			if(y == 1 || y == 6) BOARD(x,y).piece = PAWN;
			if(y == 0 || y == 7) {
				if(x == 0 || x == 7) BOARD(x,y).piece = ROOK;
				if(x == 1 || x == 6) BOARD(x,y).piece = KNIGHT;
				if(x == 2 || x == 5) BOARD(x,y).piece = BISHOP;
				if(x == 3) BOARD(x,y).piece = QUEEN;
				if(x == 4) BOARD(x,y).piece = KING;
			}
		}
	}
}

void GenerateMoves()
{
	// Generates all moves for the current ply

	ms_end[ply] = ms_begin[ply]; // set stack indexes
	GeneratePawnMoves();
	GeneratePieceMoves();
	ms_begin[ply+1] = ms_end[ply];  // next ply's moves begin where this ply's end

}

BOOL InCheck(int s)
{
	// returns TRUE is side s is in check.
	// very slow - replace with piece lists later
	int i;

	for(i = 0; i < 64; i++) {
		if(board[i].piece == KING && board[i].color == s) {
			return Attacked(i,s^1);
		}
	}
	// should never get here, except when debugging
	return FALSE;

}


BOOL Attacks(int sqr1, int sqr2, BOOL ValidateSqr2HasPiece)
{
	int c,d,r;
	// returns TRUE if piece at sqr1 attacks a piece on sqr2
	if(board[sqr1].piece == EMPTY) return FALSE;
	if(ValidateSqr2HasPiece == TRUE && board[sqr2].piece == EMPTY) return FALSE;
	if(board[sqr1].color == board[sqr2].color) return FALSE;
	c = board[sqr1].color;

	if(board[sqr1].piece == PAWN) {
		if(c == WHITE) {
			if(GETFILE(sqr1) != 0 && sqr1+7 == sqr2) return TRUE;
			if(GETFILE(sqr1) != 7 && sqr1+9 == sqr2) return TRUE;
		} else {
			if(GETFILE(sqr1) != 0 && sqr1-9 == sqr2) return TRUE;
			if(GETFILE(sqr1) != 7 && sqr1-7 == sqr2) return TRUE;
		}
	} else {
		// not a pawn
		for(r = 0; r < raynum[board[sqr1].piece]; r++) {
			for(d = 0; d < 7; d++) {
				if(rays[board[sqr1].piece-KNIGHT][sqr1][r][d] == sqr2) return TRUE;
				if(rays[board[sqr1].piece-KNIGHT][sqr1][r][d] == NO_RAY) break;
				if(board[rays[board[sqr1].piece-KNIGHT][sqr1][r][d]].color != NO_COLOR) break;
			}
		}
	} 
	return FALSE;
}

BOOL Attacked(int sqr, int s)
{
	// returns TRUE if the square sqr is attacked by side s
	// used to detect checks
	int i,r,d;
	for(i = 0; i < 64; i++) {
		if(board[i].color == s) {
			if(board[i].piece == PAWN) {
				if(board[i].color == WHITE) {
					// find if pawn attacks sqr
					if(GETFILE(i) != 0 && i+7 == sqr) return TRUE;
					if(GETFILE(i) != 7 && i+9 == sqr) return TRUE;
				} else { // black
					if(GETFILE(i) != 0 && i-9 == sqr) return TRUE;
					if(GETFILE(i) != 7 && i-7 == sqr) return TRUE;
				}
			}
			else {
			// Not a pawn... hmm.
			// fastest way I can think to do this at the moment
				for(r = 0; r < raynum[board[i].piece]; r++) {
					for(d = 0; d < 7; d++) {
						if(rays[board[i].piece-KNIGHT][i][r][d] == sqr) return TRUE;
						if(rays[board[i].piece-KNIGHT][i][r][d] == NO_RAY) break;
						if(board[rays[board[i].piece-KNIGHT][i][r][d]].color != NO_COLOR) break;
					}
				}
			} 
		}
	}
	return FALSE;
}

void GeneratePieceMoves()
{
	int i,r,d;
	for(i = 0; i < 64; i++) {
		if(board[i].piece > PAWN && board[i].color == side) {
			for(r = 0; r < raynum[board[i].piece]; r++) {
				for(d = 0; d < 7; d++) {
					if(rays[board[i].piece-KNIGHT][i][r][d] == NO_RAY) break;
					if(board[rays[board[i].piece-KNIGHT][i][r][d]].color == side) break;

					if(board[rays[board[i].piece-KNIGHT][i][r][d]].color == xside) {
						MoveStackPush(i,rays[board[i].piece-KNIGHT][i][r][d],SM_CAPTURE);
						break;
					} else {
						MoveStackPush(i,rays[board[i].piece-KNIGHT][i][r][d],0);
					}		
				}
			}
		}
	}

		/* generate castle moves */
	if (side == WHITE) {
		if (castle & CASTLE_WHITEKINGSIDE)
			MoveStackPush(E1, G1, SM_CASTLE);
		if (castle & CASTLE_WHITEQUEENSIDE)
			MoveStackPush(E1, C1, SM_CASTLE);
	}
	else {
		if (castle & CASTLE_BLACKKINGSIDE)
			MoveStackPush(E8, G8, SM_CASTLE);
		if (castle & CASTLE_BLACKQUEENSIDE)
			MoveStackPush(E8, C8, SM_CASTLE);
	}
}


void GeneratePawnMoves()
{
	int i;
	for(i = 0; i < 64; i++) {
		if(board[i].piece == PAWN && board[i].color == side) {
			if(side == WHITE) {
				if(GETFILE(i) != 0 && board[i+7].color == BLACK) {
					MoveStackPush(i,i+7, SM_PAWNMOVE | SM_CAPTURE);
				}
				if(GETFILE(i) != 7 && board[i+9].color == BLACK) {
					MoveStackPush(i, i+9, SM_PAWNMOVE | SM_CAPTURE);
				}
				if(board[i+8].color == NO_COLOR) {
					MoveStackPush(i,i+8,SM_PAWNMOVE);
					if(GETRANK(i) == 1 && board[i+16].color == NO_COLOR) {
						MoveStackPush(i,i+16,SM_PAWNMOVE | SM_DOUBLEPUSH);
					}
				}
			} else { // side = BLACK
				if(GETFILE(i) != 0 && board[i-9].color == WHITE) {
					MoveStackPush(i,i-9, SM_PAWNMOVE | SM_CAPTURE);
				}
				if(GETFILE(i) != 7 && board[i-7].color == WHITE) {
					MoveStackPush(i, i-7, SM_PAWNMOVE | SM_CAPTURE);
				}
				if(board[i-8].color == NO_COLOR) {
					MoveStackPush(i,i-8,SM_PAWNMOVE);
					if(GETRANK(i) == 6 && board[i-16].color == NO_COLOR) {
						MoveStackPush(i,i-16,SM_PAWNMOVE | SM_DOUBLEPUSH);
					}
				}
				
			}
		}
	}
	// Generate en passant moves
	if(enpassant != NO_EP_SQR) {
		if(side == WHITE) {
			if(GETFILE(enpassant) != 0 && board[enpassant-9].piece == PAWN && board[enpassant-9].color == WHITE) {
				MoveStackPush(enpassant-9,enpassant, SM_CAPTURE | SM_PAWNMOVE | SM_ENPASSANT);
			}
			if(GETFILE(enpassant) != 7 && board[enpassant-7].piece == PAWN && board[enpassant-7].color == WHITE) {
				MoveStackPush(enpassant-7,enpassant, SM_CAPTURE | SM_PAWNMOVE | SM_ENPASSANT);
			}
		} else { // side = BLACK
			if(GETFILE(enpassant) != 0 && board[enpassant+7].piece == PAWN && board[enpassant+7].color == BLACK) {
				MoveStackPush(enpassant+7,enpassant, SM_CAPTURE | SM_PAWNMOVE | SM_ENPASSANT);
			}
			if(GETFILE(enpassant) != 7 && board[enpassant+9].piece == PAWN && board[enpassant+9].color == BLACK) {
				MoveStackPush(enpassant+9,enpassant, SM_CAPTURE | SM_PAWNMOVE | SM_ENPASSANT);
			}
		}
	}


}

void GenerateCaptures()
{
	int i,d,r;
	ms_end[ply] = ms_begin[ply]; // set stack indexes
	for(i = 0; i < 64; i++) {
		if(board[i].piece == PAWN && board[i].color == side) {
			if(side == WHITE) {
				if(GETFILE(i) != 0 && board[i+7].color == BLACK) {
					MoveStackPush(i,i+7, SM_PAWNMOVE | SM_CAPTURE);
				}
				if(GETFILE(i) != 7 && board[i+9].color == BLACK) {
					MoveStackPush(i, i+9, SM_PAWNMOVE | SM_CAPTURE);
				}
			} else { // side = BLACK
				if(GETFILE(i) != 0 && board[i-9].color == WHITE) {
					MoveStackPush(i,i-9, SM_PAWNMOVE | SM_CAPTURE);
				}
				if(GETFILE(i) != 7 && board[i-7].color == WHITE) {
					MoveStackPush(i, i-7, SM_PAWNMOVE | SM_CAPTURE);
				}				
			}
		}
	}
	for(i = 0; i < 64; i++) {
		if(board[i].piece > PAWN && board[i].color == side) {
			for(r = 0; r < raynum[board[i].piece]; r++) {
				for(d = 0; d < 7; d++) {
					if(rays[board[i].piece-KNIGHT][i][r][d] == NO_RAY) break;
					if(board[rays[board[i].piece-KNIGHT][i][r][d]].color == side) break;

					if(board[rays[board[i].piece-KNIGHT][i][r][d]].color == xside) {
						MoveStackPush(i,rays[board[i].piece-KNIGHT][i][r][d],SM_CAPTURE);
						break;
					} 	
				}
			}
		}
	}
	// Generate en passant moves
	if(enpassant != NO_EP_SQR) {
		if(side == WHITE) {
			if(GETFILE(enpassant) != 0 && board[enpassant-9].piece == PAWN && board[enpassant-9].color == WHITE) {
				MoveStackPush(enpassant-9,enpassant, SM_CAPTURE | SM_PAWNMOVE | SM_ENPASSANT);
			}
			if(GETFILE(enpassant) != 7 && board[enpassant-7].piece == PAWN && board[enpassant-7].color == WHITE) {
				MoveStackPush(enpassant-7,enpassant, SM_CAPTURE | SM_PAWNMOVE | SM_ENPASSANT);
			}
		} else { // side = BLACK
			if(GETFILE(enpassant) != 0 && board[enpassant+7].piece == PAWN && board[enpassant+7].color == BLACK) {
				MoveStackPush(enpassant+7,enpassant, SM_CAPTURE | SM_PAWNMOVE | SM_ENPASSANT);
			}
			if(GETFILE(enpassant) != 7 && board[enpassant+9].piece == PAWN && board[enpassant+9].color == BLACK) {
				MoveStackPush(enpassant+9,enpassant, SM_CAPTURE | SM_PAWNMOVE | SM_ENPASSANT);
			}
		}
	}

	ms_begin[ply+1] = ms_end[ply];  // next ply's moves begin where this ply's end
}


void MoveStackPush(int from, int to, int bits)
{
	mstack_t *cur;

	if(bits & SM_PAWNMOVE) { // test for promotions
		if(side == WHITE && to > 55) {
			MoveStackPushPromotion(from, to, bits);
			return;
		}
		if(side == BLACK && to < 8) {
			MoveStackPushPromotion(from, to, bits);
			return;
		}
	}


	cur = &movestack[ms_end[ply]];
	++ms_end[ply];
	cur->m.info.from = (char)from;
	cur->m.info.to =   (char)to;
	cur->m.info.bits = (char)bits;
	if(bits & SM_CAPTURE) {
		cur->score = BASE_CAPTURE + (board[to].piece * 10) - board[from].piece;
	} else {
		cur->score = hheuristic[from][to];
	}
}

void MoveStackPushPromotion(int from, int to, int bits)
{
	mstack_t *cur;
	int i;
	for(i = KNIGHT; i < KING; i++) {
		cur = &movestack[ms_end[ply]];
		++ms_end[ply];
		cur->m.info.from = (char)from;
		cur->m.info.to   = (char)to;
		cur->m.info.bits = (char)(bits | SM_PROMOTE);
		cur->m.info.promote = (char)i;
		cur->score = BASE_PROMOTION + i;
	}

}
BOOL MakeMove(move Move)
{
	// Makes the move and returns TRUE if it was legal,
	// otherwise it undoes what it did and returns FALSE
	

	// This new version of MakeMove dynamically updates the current board_key instead of generating a
	// new one every move from scratch. This is much more efficient.
	unsigned __int64 new_key = board_key;
	int old_enpassant = enpassant;
	int old_castle = castle;

	if(Move.info.bits & SM_CASTLE) {
		int to, from;
			if (InCheck(side)) // early abort
			return FALSE;
		switch (Move.info.to) {
			case G1:
				if (board[F1].color != NO_COLOR || board[G1].color != NO_COLOR ||
						Attacked(F1, xside) || Attacked(G1, xside))
					return FALSE;
				from = H1;
				to = F1;
				break;
			case C1:
				if (board[B1].color != NO_COLOR || board[C1].color != NO_COLOR || board[D1].color != NO_COLOR ||
						Attacked(C1, xside) || Attacked(D1, xside))
					return FALSE;
				from = A1;
				to = D1;
				break;
			case G8:
				if (board[F8].color != NO_COLOR || board[G8].color != NO_COLOR ||
						Attacked(F8, xside) || Attacked(G8, xside))
					return FALSE;
				from = H8;
				to = F8;
				break;
			case C8:
				if (board[B8].color != NO_COLOR || board[C8].color != NO_COLOR || board[D8].color != NO_COLOR ||
						Attacked(C8, xside) || Attacked(D8, xside))
					return FALSE;
				from = A8;
				to = D8;
				break;
			default:  // shouldn't get here - these values should make our code
				from = -1; // crash in a spectacular way.
				to = -1;
				break;
		}


		board[to].color = board[from].color;
		board[to].piece = board[from].piece;
		
		// XOR in new piece location in zkey, XOR out old position
		// be very careful, a bug here will be all but impossible to find.
		new_key ^= ZobristKeys[board[to].piece-PAWN][board[to].color][to];
		new_key ^= ZobristKeys[board[from].piece-PAWN][board[from].color][from];
		
		board[from].color = NO_COLOR;
		board[from].piece = EMPTY;
	}

	history[hply].m = Move;
	history[hply].capture = board[Move.info.to].piece;
	history[hply].castle = castle;
	history[hply].ep = enpassant;
	history[hply].fifty = fifty;
	history[hply].score = 0;
	history[hply].zkey = board_key;
	++ply;
	++hply;
	
	
	// update en passant square
	if(Move.info.bits & SM_DOUBLEPUSH) {
		if(side == WHITE) {
			enpassant = Move.info.to-8;
		} else {
			enpassant = Move.info.to+8;
		}
	} else {
		enpassant = NO_EP_SQR;
	}

	if(old_enpassant != enpassant) {
		new_key ^= ZobristEPKeys[old_enpassant]; // XOR out old key
		new_key ^= ZobristEPKeys[enpassant];	  // replace
	}

		// update castle permissions
	castle &= castle_mask[Move.info.from] & castle_mask[Move.info.to];

	if(castle != old_castle) { // Check castle key
		new_key ^= ZobristCastleKeys[old_castle];
		new_key ^= ZobristCastleKeys[castle];
	}

	// update fifty move count
	if(Move.info.bits & (SM_CAPTURE | SM_PAWNMOVE)) {
		fifty = 0;
	} else {
		fifty++;
	}

	if((Move.info.bits & SM_CAPTURE) && !(Move.info.bits & SM_ENPASSANT)) {
		// remove captured piece from zkey
		new_key ^= ZobristKeys[board[Move.info.to].piece-PAWN][board[Move.info.to].color][Move.info.to];
	}

	board[Move.info.to].piece = board[Move.info.from].piece;
	board[Move.info.to].color = board[Move.info.from].color;
	
	// XOR out old location
	new_key ^= ZobristKeys[board[Move.info.from].piece-PAWN][board[Move.info.from].color][Move.info.from];

	board[Move.info.from].piece = EMPTY;
	board[Move.info.from].color = NO_COLOR;

	if(Move.info.bits & SM_PROMOTE) {
		board[Move.info.to].piece = Move.info.promote;
	}

	// XOR in new piece location here, to make sure that we catch the promotion case
	new_key ^= ZobristKeys[board[Move.info.to].piece-PAWN][board[Move.info.to].color][Move.info.to];

	// IMPORTANT TO DO THIS BEFORE RESETTING EP SQR
	if(Move.info.bits & SM_ENPASSANT) {
		if(xside == WHITE) {
			// update pawn removal in zkey
			new_key ^= ZobristKeys[board[Move.info.to+8].piece-PAWN][board[Move.info.to+8].color][Move.info.to+8];
			board[Move.info.to+8].color = NO_COLOR;
			board[Move.info.to+8].piece = EMPTY;
		} else {
			// update pawn removal in zkey
			new_key ^= ZobristKeys[board[Move.info.to-8].piece-PAWN][board[Move.info.to-8].color][Move.info.to-8];
			board[Move.info.to-8].color = NO_COLOR;
			board[Move.info.to-8].piece = EMPTY;
		}
	}

	side ^= 1;
	xside ^= 1;

	new_key ^= BlackMoveKey; // update side to move in zkey

	if(InCheck(xside)) {
		UnmakeMove();
		return FALSE;
	}

	board_key = ZobristKey();
	assert(board_key == new_key);
	return TRUE;
}

void UnmakeMove()
{
	// Undoes the previous move
	move Move;


	// little debug sanity check
	if(hply < 1) {
		return;
	}

	side ^= 1;
	xside ^= 1;
	ply--;
	hply--;

	Move = history[hply].m;
	enpassant = history[hply].ep;
	fifty = history[hply].fifty;
	castle = history[hply].castle;
	board_key = history[hply].zkey;

	// from square color undo
	board[Move.info.from].color = side;

	// from square piece undo
	if(Move.info.bits & SM_PROMOTE) {
		board[Move.info.from].piece = PAWN;
	} else {
		board[Move.info.from].piece = board[Move.info.to].piece;
	}

	// to square piece/color undo
	if(Move.info.bits & SM_CAPTURE) {
		board[Move.info.to].piece = history[hply].capture;
		board[Move.info.to].color = xside;
	} else {
		board[Move.info.to].piece = EMPTY;
		board[Move.info.to].color = NO_COLOR;
	}

	if (Move.info.bits & SM_CASTLE) {
		int from, to;

		switch(Move.info.to) {
			case G1:
				from = F1;
				to = H1;
				break;
			case C1:
				from = D1;
				to = A1;
				break;
			case G8:
				from = F8;
				to = H8;
				break;
			case C8:
				from = D8;
				to = A8;
				break;
			default:  /* shouldn't get here */
				from = -1;
				to = -1;
				break;
		}
		board[to].color = side;
		board[to].piece = ROOK;
		board[from].color = NO_COLOR;
		board[from].piece = EMPTY;
	}

	if(Move.info.bits & SM_ENPASSANT) {
		board[enpassant].color = NO_COLOR;
		board[enpassant].piece = EMPTY;
		if(xside == WHITE) {
			board[enpassant+8].color = xside;
			board[enpassant+8].piece = PAWN;
		} else {
			board[enpassant-8].color = xside;
			board[enpassant-8].piece = PAWN;
		}
	}

}


// From Tom Kerrigan's Simple Chess Program
/* reps() returns the number of times that the current
   position has been repeated. Thanks to John Stanback
   for this clever algorithm. */

int CountRepetitions()
{
	int i;
	int b[64];
	int c = 0;  /* count of squares that are different from
				   the current position */
	int r = 0;  /* number of repetitions */

	/* is a repetition impossible? */
	if (fifty <= 3)
		return 0;

	for (i = 0; i < 64; ++i)
		b[i] = 0;

	/* loop through the reversible moves */
	for (i = hply - 1; i >= hply - fifty - 1; --i) {
		if (++b[history[i].m.info.from] == 0)
			--c;
		else
			++c;
		if (--b[history[i].m.info.to] == 0)
			--c;
		else
			++c;
		if (c == 0)
			++r;
	}

	return r;
}

move MoveFromCoords(char *s)
{
	// returns null move if illegal
	move Move, nm;
	char from, to, bits;
	BOOL found;
	int i;

	nm.id = 0;

	from = (s[0] - 'a')+(8*(s[1]-'1'));
	to   = (s[2] - 'a')+(8*(s[3]-'1'));
	bits = 0;

	ply = 0;

	found = FALSE;
	GenerateMoves();
	for(i = 0; i < ms_end[0]; i++) {
		if(movestack[i].m.info.from == from && movestack[i].m.info.to == to) {
			found = TRUE;
			// read promotion choice
			if(movestack[i].m.info.bits & SM_PROMOTE) {
				switch(s[4]) {
				case 'n':
					break;
				case 'b':
					i+=1;
					break;
				case 'r':
					i+=2;
					break;
				default:
					i+=3;
					break;
				}
			}
			break;
		}
	}
	if (found) {
		Move.id = movestack[i].m.id;
		if (!MakeMove(movestack[i].m)) {
			// king in check
			return nm;
		} else {
			UnmakeMove();
			return Move;
		}
	} else {
		return nm;
	}
}

BOOL IsLegalMove(move Move) 
{
	BOOL result;
	result = MakeMove(Move);
	if(result == TRUE) UnmakeMove();
	return result;
}