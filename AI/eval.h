// Evaluation constants
// Piece and positional valuations are in centripawns

#define		PAWN_VALUE			100
#define		KNIGHT_VALUE		300
#define		BISHOP_VALUE		300
#define		ROOK_VALUE			500
#define		QUEEN_VALUE			900
#define		KING_VALUE			0


#define		ENDGAME_THRESHOLD	1200		// Amount of material any side must have to
											// trigger endgame logic

#define		DOUBLED_PAWNS		-50			// doubled pawns penalty (compounded for multiple pawns)
#define		PAWN_ISLAND			-30			// pawn island penalty (single pawn with no friends)
#define		PAWN_CHAIN			10			// bonus given for each pawn for each
											// other pawn within 1 sqr of it (except
											// doubled pawns)
#define		PASSED_PAWN			50			// bonus for a passed pawn


#define		DRAW				0			// Lower to make computer more willing to draw
											// Raise to make computer less willing
							
#define		CHECKMATE			-10000		// no eval can be higher than |abs| this



// Evaluations for move-ordering

#define		BASE_CAPTURE		100000		// Modified with MVA/LVV
#define		BASE_PROMOTION		1000000		// Modified with promoted piece value
#define		BASE_PV				10000000    // Modified by SortPV