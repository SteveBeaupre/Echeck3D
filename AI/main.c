// Fimbulwinter
// John Shedletsky
//
// For up to date Fimbulwinter info, see:
// http://www.stanford.edu/~jjshed/chess

#define BUILD_DATE "November 29, 2002"
#define VERSION  "5.00"
#define INI_FILENAME "fimbulwinter_5_00.ini"

#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <string.h>
#include <signal.h>

#include <windows.h>   
#include <windowsx.h> 
#include <mmsystem.h>
#include <iostream.h> 
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h> 
#include <math.h>
#include <io.h>
#include <fcntl.h>


#include "defs.h"
#include "data.h"
#include "protos.h"
#include "stats.h"
#include "book.h"

void main()
{
	static char s[256];
	static char args[3][256];
	char pgn_trans[8];
	int from, to, bits,i;
	BOOL found;
	int computer_side = NO_COLOR;
	move CompMove;

	printf("\n");
	printf("FIMBULWINTER\n");
	printf("------------\n");
	printf("Version %s\n", VERSION);
	printf("%s Build\n", BUILD_DATE);
	printf("Copyright 2002 John Shedletsky\n");
	printf("shedletsky@stanford.edu\n");
	printf("Fimbulwinter Homepage:\nhttp://www.stanford.edu/~jjshed/chess\n");
	printf("\nType help for commands\n");
	printf("\n");

	Init();



	for(;;)
	{
		if (side == computer_side) {  /* computer's turn */
			
			if(GameStateCheck()) { // see if game is over 
				ReInit(TRUE);
				computer_side = NO_COLOR;
			}

			Think(CONSOLE);

			if (pv[0][0].id == 0) {
				printf("(no legal moves)\n");
				computer_side = NO_COLOR;
				continue;
			}
			CompMove = pv[0][0];
			printf("Computer's move: %s\n", MoveStr(CompMove));
			MakeMove(CompMove);
			ply = 0;

			if(GameStateCheck()) { // see if game is over 
				ReInit(TRUE);
				computer_side = NO_COLOR;
			}


			continue;
		}


		/* get user input */
		printf("Fimbulwinter> ");
		scanf("%s", s);
		

		if(!strcmp(s, "on")) {
			computer_side = side;
			continue;
		}

		if(!strcmp(s, "off")) {
			computer_side = NO_COLOR;
			continue;
		}

		if(!strcmp(s, "zobrist")) {
			// spit out this position's zobrist hash key
			printf("\nZobrist key: %I64u\n", ZobristKey());
			continue;
		}

		if(!strcmp(s, "ttstats")) {
			StatsDump_TTable();
			continue;
		}
		if(!strcmp(s, "clearstats")) {
			ClearOnePlyStats();
			continue;
		}
		if(!strcmp(s, "cleartable")) {
			ClearTTable();
			continue;
		}
		if(!strcmp(s, "cross")) {
			TTableCrossSection();
			continue;
		}
		if(!strncmp(s, "load", 4)) {
			printf("FEN position file: ");
			scanf("%s", args[0]);
			if(!LoadFEN(args[0])) printf("\n\nFile Error!\n\n");
			continue;
		}

		if (!strcmp(s, "exit")) {
			return;
		}
		if (!strcmp(s, "d")) {
			DrawBoard();
			continue;
		}
		if (!strcmp(s, "bench")) {
			TSCP_Bench();
			continue;
		}
		if (!strcmp(s, "new")) {
			ReInit(TRUE);
			continue;
		}
		if (!strcmp(s, "eval")) {
			printf("Assessment: %i\n", EvaluateBoard());
			continue;
		}
		if (!strcmp(s, "pawneval")) {
			BuildPawnLists();
			printf("Pawn structure assessment: %i\n", PawnEval(side));
			continue;
		}
		if (!strcmp(s, "safe")) {
			printf("King safety assessment: %i\n", KingSafety(side));
			continue;
		}
		if (!strcmp(s, "movegen")) { //generate and list current possible moves
			GenerateMoves();
			ListPossibleMoves(FALSE);
			continue;
		}
		if (!strcmp(s, "legalgen")) { // generates and lists all legal moves
			GenerateMoves();
			ListPossibleMoves(TRUE);
			continue;
		}
		if (!strcmp(s, "clear")) { // clear board
			ReInit(TRUE);
			ClearBoard();
			continue;
		}
		if (!strcmp(s, "nocastle")) { // sets castle flags to null
			castle = 0;
			continue;
		}
		if (!strcmp(s, "xboard")) {
			xboard();
			exit(0);
		}
		if (strpbrk( s, "@" )) { // put piece
			ParsePutPiece(s);
			continue;
		}
		if (!strcmp(s, "undo")) { // go back one ply
			UnmakeMove();
			continue;
		}
		
    	if (!strcmp(s, "makebookpgn")) { // make opening book from PGN
			printf("\nInput: ");
			scanf("%s", args[0]);
			printf("\nOutput: ");
			scanf("%s", args[1]);
			printf("\nExtract to ply: ");
			scanf("%i", (int *)args[2]);
			if(MakeBookFromPGN(args[0], args[1], *(int *)args[2])) { // <- Convoluted typecast
				printf("\nFinished!\n\n");
			} else {
				printf("\nError in compilation!\n\n");
			}
			continue;
		} 




		if (!strcmp(s, "help")) { // show help
			printf("\nFIMBULWINTER HELP\n");
			printf("-----------------\n");
			printf("Note: this is only a partial list. See source for advanced options.\n\n");
			printf("Command				Function\n");
			printf("new				Start new game\n");
			printf("d				Display board\n");
			printf("on\\off				Toggle computer player\n");
			printf("clear				Clears board\n");
			printf("nocastle			Voids castle permissions\n");
			printf("undo				Takes back last move\n");
			printf("bench				Loads benchmark position\n");
			printf("load             		Loads a FEN position\n");
			printf("exit				Quits program\n");
			printf("\n");
			printf("Moves are entered in co-ordinant notation\n\n");
			printf("Example Move			Effect\n");
			printf("d2d4				Move piece at d2 to d4\n");
			printf("e1g1				Kingside castle for white\n");
			printf("e7e8r				Pawn promote to rook\n");
			printf("P@d4				Puts a white pawn at d4\n\n");
			continue;
		}


		if(!IsCoordNotationMove(s)) {
			PGNtoCoordMove(s, pgn_trans);
			memcpy(s, pgn_trans, PGN_MOVE_LEN);
		}

		// else try to see if it's a legal move
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
			if (!MakeMove(movestack[i].m)) {
				printf("That move would leave your king in check!\n");
			} else {
				ply=0;
				if(GameStateCheck()) { // see if game is over 
					ReInit(TRUE);
					computer_side = NO_COLOR;
				}
			}
		} else {
			printf("Illegal Move\n");
		}
	}
}

void Init()
{
	// Read engine constants
	ReadINIFile(INI_FILENAME);

	// Register critical callback functions to clean up memory before we exit
	atexit(TTableCleanup);
	atexit(BookCleanup);

	// Setup memory allocation
	TTableInit();
	InitBook();


	side = WHITE; // Inited here because important for zkey generation
	xside = BLACK;
	enpassant = NO_EP_SQR;
	castle = CASTLE_WHITEKINGSIDE | CASTLE_WHITEQUEENSIDE | CASTLE_BLACKKINGSIDE | CASTLE_BLACKQUEENSIDE;

	// Only called once, when the program starts
	InitRays();
	InitZobristKeys(); // seeds random number generator to fixed value



	ReInit(TRUE);

	LoadBook(INI_BOOK_FILE);
	srand((unsigned int)GetTickCount());
}

void ReInit(BOOL clear_hash_table)
{
	// Called to reset board state to init
	//
	// clear_hash_table is usually set to TRUE, except when compiling an opening book, 
	// where the transposition table is not being used, but calling ClearTTable() is
	// an incredible performace bottleneck (profiler says about 90% the time spent
	// parsing games is spent clearing the hash table!). Which is kind of believable,
	// given that it can be around 64 MB or larger.
	InitBoard();
	if(clear_hash_table == TRUE) ClearTTable();
	side = WHITE;
	xside = BLACK;
	computer_side = NO_COLOR;
	ply = 0;
	hply = 0;
	castle = CASTLE_WHITEKINGSIDE | CASTLE_WHITEQUEENSIDE | CASTLE_BLACKKINGSIDE | CASTLE_BLACKQUEENSIDE;
	enpassant = NO_EP_SQR;
	board_key = ZobristKey();
}

void ParsePutPiece(char *s)
{
	// format: "put B@a3  - white bishop at a3
	char piece = EMPTY;
	char color;

	if(s[0] >= 'a') { color = BLACK; } else { color = WHITE; }
	if(s[0] >= 'a') s[0]-=32;
	if(s[0] == 'P') piece = PAWN;
	if(s[0] == 'B') piece = BISHOP;
	if(s[0] == 'N') piece = KNIGHT;
	if(s[0] == 'R') piece = ROOK;
	if(s[0] == 'Q') piece = QUEEN;
	if(s[0] == 'K') piece = KING;

	if (piece == EMPTY) {
		printf("Invalid syntax\n");
		return;
	}

	BOARD((s[2]-'a'),(s[3]-'1')).piece = piece;
	BOARD((s[2]-'a'),(s[3]-'1')).color = color;
}

void ClearBoard()
{
	int i;
	for(i = 0; i < 64; i++) {
		board[i].color = NO_COLOR;
		board[i].piece = EMPTY;
	}
}


void DrawBoard()
{
	// ascii for 'a' is = 'A'+32
	// Black in lower case

	int x,y,c;
	printf("\n\n");
	for(y = 7; y >= 0; y--) {
		printf("\t%d  ", y+1);
		for(x = 0; x < 8; x++) {
			if(BOARD(x,y).color == BLACK) { c = 1; } else { c = 0; }
			printf("%c ", piece_char[BOARD(x,y).piece] + (c*32));
		}
		printf("\n");
	}
	printf("\n\t   a b c d e f g h\n");
	printf("\n\n");
			

}

char *MoveStr(move Move) 
{
	static char str[6];
	

	// regular move
	str[0] = (Move.info.from%8)+'a';
	str[1] = (Move.info.from/8)+'1';

	str[2] = (Move.info.to%8)+'a';
	str[3] = (Move.info.to/8)+'1';

	if(Move.info.bits & SM_PROMOTE) {
		str[4] = piece_char[Move.info.promote]+32;
		str[5] = '\0';
		return str;
	}

	str[4] = '\0';

	return str;
}


BOOL GameStateCheck()
{
	// checks to see if the game is over and prints the result
	int i;

	ply = 0;
	GenerateMoves();

	// check for a legal move
	for(i = 0; i < ms_end[0]; ++i) {
		if(MakeMove(movestack[i].m)) {
			UnmakeMove();
			return FALSE;
		}
	}

	if(i == ms_end[0]) {
		if(InCheck(side)) {
			if(side == WHITE)
				printf("0-1 {-Black mates-}\n");
			else
				printf("1-0 {-White mates-}\n");
		} else {
			printf("1/2-1/2 {-Stalemate-}\n");
		}
		return TRUE;
	} else {
		if (CountRepetitions() == 3) {
			printf("1/2-1/2 {Draw by repetition}\n");
			return TRUE;
		}
		if (fifty >= 100) {
			printf("1/2-1/2 {Draw by fifty move rule}\n");
			return TRUE;
		}
	}

	return FALSE;
	
}

void ListPossibleMoves(BOOL verify)
{
	// prints all moves possible in current position
	// if verify is true, makes sure that all of them are legal
	int i, count;
	ply = 0;
	printf("\nPossible Moves:\n");
	count = 0;
	for (i = ms_begin[0]; i < ms_end[0]; i++) {
		if(verify == TRUE) {
			if(MakeMove(movestack[i].m)) {
				UnmakeMove();
				printf("%s ",MoveStr(movestack[i].m));
				count++;
			}
		} else {
			printf("%s ",MoveStr(movestack[i].m));
			count++;
		}
	}
	printf("\nTotal Moves: %i\n\n", count);

}


/* bench: This is a little benchmark code that calculates how many
   nodes per second TSCP searches.
   It sets the position to move 17 of Bobby Fischer vs. J. Sherwin,
   New Jersey State Open Championship, 9/2/1957.
   Then it searches five ply three times. It calculates nodes per
   second from the best time. */

/*	I'm including the same benchmark position as Tom Kerrigan did to 
	facilitate relative performance assessments. I'm planning to add
	other bench marks later
	*/

void TSCP_Bench()
{
	int i;

	int bench_color[64] = {
	2, 1, 1, 2, 2, 1, 1, 2,
	1, 2, 2, 2, 2, 1, 1, 1,
	2, 1, 2, 1, 1, 2, 1, 2,
	2, 2, 2, 1, 2, 2, 0, 2,
	2, 2, 1, 0, 2, 2, 2, 2,
	2, 2, 0, 2, 2, 2, 0, 2,
	0, 0, 0, 2, 2, 0, 0, 0,
	0, 2, 0, 2, 0, 2, 0, 2
	};

	int bench_piece[64] = {
	0, 4, 3, 0, 0, 4, 6, 0,
	1, 0, 0, 0, 0, 1, 1, 1,
	0, 1, 0, 5, 1, 0, 2, 0,
	0, 0, 0, 2, 0, 0, 2, 0,
	0, 0, 1, 1, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 1, 0,
	1, 1, 5, 0, 0, 1, 3, 1,
	4, 0, 3, 0, 4, 0, 6, 0
	};

	int flip[64] = { // used to flip pos, since I use different board coords than Tom
	 56,  57,  58,  59,  60,  61,  62,  63,
	 48,  49,  50,  51,  52,  53,  54,  55,
	 40,  41,  42,  43,  44,  45,  46,  47,
	 32,  33,  34,  35,  36,  37,  38,  39,
	 24,  25,  26,  27,  28,  29,  30,  31,
	 16,  17,  18,  19,  20,  21,  22,  23,
	  8,   9,  10,  11,  12,  13,  14,  15,
	  0,   1,   2,   3,   4,   5,   6,   7
	};

	for (i = 0; i < 64; ++i) {
		board[i].color = bench_color[flip[i]];
		board[i].piece = bench_piece[flip[i]];
	}
	side = WHITE;
	xside = BLACK;
	castle = 0;
	enpassant = NO_EP_SQR;
	fifty = 0;
	ply = 0;
	hply = 20;
	board_key = ZobristKey();
}




// xboard code largely borrowed from Tom Kerrigan

/* xboard() is a substitute for main() that is XBoard
   and WinBoard compatible. See the following page for details:
   http://www.research.digital.com/SRC/personal/mann/xboard/engine-intf.html */

void xboard()
{
	int computer_side = BLACK;
	char line[256], command[256], msg[356];
	int from, to, i;
	BOOL found;
	int post = 0;
	move CompMove;

	ReInit(TRUE); // Init called in main

	signal(SIGINT, SIG_IGN);
	printf("\n");

	for (;;) {
		if (side == computer_side) {

			if(GameStateCheck()) { // see if game is over 
				fflush(stdout);
				ReInit(TRUE);
				computer_side = NO_COLOR;
			}

			Think(XBOARD);
			if (pv[0][0].id == 0) {
				printf("(no legal moves)\n");
				fflush(stdout);
				computer_side = NO_COLOR;
				continue;
			}

			CompMove = pv[0][0];
			printf("move %s\n", MoveStr(CompMove));
			fflush(stdout);
			MakeMove(CompMove);
			ply = 0;

			if(GameStateCheck()) { // see if game is over 
				fflush(stdout);
				ReInit(TRUE);
				computer_side = NO_COLOR;
			}


			continue;
		}

		fgets(line, 256, stdin);
		if (line[0] == '\n')
			continue;
		sscanf(line, "%s", command);
#ifdef DEBUG
		WriteToLog(command);
#endif
		if (!strcmp(command, "xboard"))
			continue;

		// NEW XBOARD INTERFACE CODE
		if (!strcmp(command, "protover")) {
			// called when xboard starts up
			printf("feature myname=\"Fimbulwinter v%s\"\n", VERSION);
			printf("feature ics=1\n");
			fflush(stdout);
			continue;
		}

		// HACKED "FIMBULBOARD" WINBOARD MESSAGES /////////////////////////

		// All of the form (form of input) {who} {what}
		if (!strcmp(command, "told")) {
			// told via "tell" msg or left a msg via "message"
			strcpy(msg, (line+5));
			printf("tellics kibitz %s\n",msg);
			printf("tellics draw\n");
			fflush(stdout);

		}
		if (!strcmp(command, "whispered")) {
			// whispered to via "whisper"


		}
		if (!strcmp(command, "heardkibitz")) {
			// picked up a kibitz

		}
		if (!strcmp(command, "heardshout")) {

		}
		if (!strcmp(command, "heardsshout")) {


		}

		

		/*
			HACKED FIMBULBOARD OUTPUTS

			kibitz {arg}		
			whisper {arg}
			shout {arg}			// Don't use these two on ICC
			sshout {arg}
			tell {arg} {arg}  // who, what

		*/


		///////////////////////////////////////////////////////////////////



		if (!strcmp(command, "ics")) { // are we on an internet chess server?
			if(line[4] == '-') {
				icc = FALSE;
			} else {
				icc = TRUE;
			}
			continue;
		}

		if (!strcmp(command, "new")) {
			ReInit(TRUE);
			computer_side = BLACK;
			printf("tellics say Thanks for playing me!\n");
			fflush(stdout);
			continue;
		}
		if (!strcmp(command, "quit"))
			return;
		if (!strcmp(command, "force")) {
			computer_side = NO_COLOR;
			continue;
		}
		if (!strcmp(command, "white")) {
			side = WHITE;
			xside = BLACK;
			ply = 0;
			hply = 0;
			computer_side = BLACK;
			continue;
		}
		if (!strcmp(command, "black")) {
			side = BLACK;
			xside = WHITE;
			ply = 0;
			hply = 0;
			computer_side = WHITE;
			continue;
		}
		if (!strcmp(command, "st")) {
			sscanf(line, "st %d", &max_time);
			max_time *= 1000;
			max_depth = 10000;
			continue;
		}
		if (!strcmp(command, "sd")) {
			sscanf(line, "sd %d", &max_depth);
			max_time = 1000000;
			continue;
		}
		if (!strcmp(command, "time")) {
			sscanf(line, "time %d", &max_time);
			max_time *= 10;
		//	max_time /= 30;
			// Very crude time management code
			max_depth = 6;
		//	if(hply < 14) max_depth = 6;
		//	if(max_time < 180000) max_depth = 6;
			if(max_time < 60000) max_depth = 5;
			if(max_time < 10000) max_depth = 4;
			if(max_time < 3000) max_depth = 3;
			continue;
		}
		if (!strcmp(command, "otim")) {
			continue;
		}
		if (!strcmp(command, "go")) {
			computer_side = side;
			continue;
		}
		if (!strcmp(command, "hint")) {
			Think(CONSOLE);
			if (pv[0][0].id == 0)
				continue;
			printf("Hint: %s\n", MoveStr(pv[0][0]));
			fflush(stdout);
			continue;
		}
		if (!strcmp(command, "undo")) {
			if (!hply)
				continue;
			UnmakeMove();
			ply = 0;
			continue;
		}
		if (!strcmp(command, "remove")) {
			if (hply < 2)
				continue;
			UnmakeMove();
			UnmakeMove();
			ply = 0;
			continue;
		}
		if (!strcmp(command, "post")) {
			post = 2;
			continue;
		}
		if (!strcmp(command, "nopost")) {
			post = 0;
			continue;
		}
		from = (line[0] - 'a')+(8*(line[1]-'1'));
		to = (line[2] - 'a')+(8*(line[3]-'1'));

		ply = 0;
		found = FALSE;
		GenerateMoves();
		for(i = 0; i < ms_end[0]; i++) {
			if(movestack[i].m.info.from == from && movestack[i].m.info.to == to) {
				found = TRUE;
				// read promotion choice
				if(movestack[i].m.info.bits & SM_PROMOTE) {
					switch(line[4]) {
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
		if (!found || !MakeMove(movestack[i].m)) {
			printf("Error (unknown command): %s\n", command);
			fflush(stdout);
		}
		else {
			ply = 0;
			if(GameStateCheck()) { // see if game is over 
				fflush(stdout);
				ReInit(TRUE);
				computer_side = NO_COLOR;
			}
		}
	}
}

BOOL IsCoordNotationMove(char *buffer)
{
	// Returns true if this move looks like it's in CoordNotation
	unsigned int i,a,n;
	a = 0;
	n = 0;
	for(i = 0; i < strlen(buffer); i++) {
		if(isupper(buffer[i])) return FALSE;
		if(buffer[i] == 'x') return FALSE;
		if(buffer[i] == '=') return FALSE;
		if(isalpha(buffer[i])) a++;
		if(isdigit(buffer[i])) n++;
	}

	if(a >= 2 && n == 2) return TRUE;
	return FALSE;
}

