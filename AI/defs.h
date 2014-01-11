


// Comment out to turn off debug mode
//#define DEBUG 1

// When defined, prints a lot of stats to the console
#define STATS	1

#define PGN_MOVE_LEN	8 // chars for PGN move buffer

#define HASH_SEED 12345

#define MAX_Q_DEPTH		50 //ply

#define EMPTY			0
#define PAWN			1
#define KNIGHT			2
#define BISHOP			3
#define ROOK			4
#define QUEEN			5
#define KING			6

#define WHITE 0
#define BLACK 1
#define NO_COLOR 2

// rays
#define KNIGHT_RAYS		8
#define BISHOP_RAYS		4
#define ROOK_RAYS		4
#define QUEEN_RAYS		8
#define KING_RAYS		8

#define NO_RAY			100 //used as delimiter in rays array

#define MAX_PLY			32
#define MOVE_STACK_SIZE 2000 // Don't know where this number comes from
#define HISTORY_STACK	400  // If the game is longer than 400 ply, we're in trouble

// special moves
// (from below)

#define SM_CAPTURE		1
#define SM_CASTLE		2
#define SM_ENPASSANT	4
#define SM_DOUBLEPUSH	8
#define SM_PAWNMOVE		16
#define SM_PROMOTE		32

// flags for castle bitfield
#define CASTLE_WHITEKINGSIDE	1
#define CASTLE_WHITEQUEENSIDE	2
#define CASTLE_BLACKKINGSIDE	4
#define CASTLE_BLACKQUEENSIDE	8

#define NO_EP_SQR		64 // do not change - used by hash.c

// useful macros
#define GETFILE(i)	(i%8)		
#define GETRANK(i)	(i/8) // replace with bit shift later?
#define BOARD(x, y)		board[x+(y*8)]


// good squares to know
#define A1	0
#define B1	1
#define C1	2
#define D1	3
#define E1	4
#define F1	5
#define G1	6
#define H1	7

#define A8	56
#define B8	57
#define C8	58
#define D8	59
#define E8  60
#define F8	61
#define G8	62
#define H8	63

// Think function output types
#define NO_OUTPUT	0
#define CONSOLE		1
#define	XBOARD		2



// tile on the board
typedef struct {
	char piece;
	char color;
} tile;


// Move representation taken from Tom Kerrigan's Simple Chess Program

/* This is the basic description of a move. promote is what
   piece to promote the pawn to, if the move is a pawn
   promotion. bits is a bitfield that describes the move,
   with the following bits:

   1	capture
   2	castle
   4	en passant capture
   8	pushing a pawn 2 squares
   16	pawn move
   32	promote

   It's union'ed with an integer so two moves can easily
   be compared with each other. */




// A single move
typedef struct {
	char from;
	char to;
	char promote;
	char bits;
} move_bytes;

// A move element in the movestack
typedef union {
	move_bytes info;
	long id;	// for same moves, this will be the same number
} move;

/* an element of the move stack. it's just a move with a
   score, so it can be sorted by the search functions. */
typedef struct {
	move m;
	long score;
} mstack_t;

/* an element of the history stack, with the information
   necessary to take a move back. */
typedef struct {
	move m;
	int capture;
	int castle;
	int ep;
	int fifty;
	int score; // just for our own information - what was the score assigned to this
				// position?
	unsigned __int64 zkey; // Zobrist key for this position - cached for speed.
} history_t;

