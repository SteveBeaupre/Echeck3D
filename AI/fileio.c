#include <windows.h>   
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <iostream.h> 
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <io.h>
#include <fcntl.h>
#include <ctype.h>
#include <assert.h>

#include "defs.h"
#include "data.h"
#include "protos.h"
#include "hash.h" // for hash macros
#include "book.h"

#define NUM_KEYS 2

char iniKeys[NUM_KEYS][40] = {
		"TRANSPOSITION_TABLE_MEMORY_MB", "OPENING_BOOK_FILENAME"};

void ReadINIFile(char *filename)
{

	FILE *in;
	BOOL foundKey[NUM_KEYS];
	char buffer[255];
	char key[40];
	int c,i;

	memset(foundKey,FALSE, sizeof(BOOL)*NUM_KEYS);
	memset(buffer,0,sizeof(char) * 255);


	in = fopen(filename, "r");
	if(in == NULL) {
		printf("No %s file found - setting INI keys to default values.\n", filename);
		for(i = 0; i < NUM_KEYS; i++) {
			if(foundKey[i] == FALSE) SetINIKey(iniKeys[i], NULL, TRUE);
		}
		return;
	} else {
		printf("%s found. Loading engine attributes.\n", filename);	
	}


	for(;;) {
		c = fgetc(in);
		if(c == '#') {
			fscanf(in, "%s %s", key, buffer);
			if((i = KeyNum(key)) >= 0) { // test that key is legit
				SetINIKey(key, buffer, FALSE);
				foundKey[i] = TRUE;
				memset(buffer,0,sizeof(char) * 255);
			}
		}
		if(c == EOF) break;
	}

	// Find all keys whose values were no explicitely set in the ini file and set them to the default
	for(i = 0; i < NUM_KEYS; i++) {
		if(foundKey[i] == FALSE) SetINIKey(iniKeys[i], NULL, TRUE);
	}

	fclose(in);

	

}

int KeyNum(const char *key) {
	int i;
	for(i = 0; i < NUM_KEYS; i++) {
		if(!strcmp(key, (const char *)iniKeys[i])) return i;
	}
	return -1;
}

void SetINIKey(char *key, char *buffer, BOOL default_val)
{
	if(!strcmp(key, "TRANSPOSITION_TABLE_MEMORY_MB")) {
		if(default_val) {
			INI_TTABLE_MEMORY_MB = 4;
		} else {
			INI_TTABLE_MEMORY_MB = atoi(buffer);
		}
	}
	if(!strcmp(key, "OPENING_BOOK_FILENAME")) {
		if(default_val) {
			strcpy(INI_BOOK_FILE, "iccf10k.fim");
		} else {
			strcpy(INI_BOOK_FILE, buffer);
		}
	}
}


int GetBookSize(char *filename)
{
	// finds the size of a bookfile in book_t elements
	int bs,be;
	FILE *in;

	if(filename[0] == '\0') return 1;

	// find size of book
	in = fopen(filename, "rb");
	if(in == NULL) return 1; // avoid malloc(0) errors
	bs = ftell(in);
	fseek(in,0, SEEK_END);
	be = ftell(in);
	fseek(in,bs, SEEK_SET);
	fclose(in);
	return (((be-bs)/sizeof(book_t))-1); // 23 byte header in file will
								// cause over estimation here.
}


BOOL LoadBook(char *filename)
{
	FILE *in;
	int i=0;
	char buffer[sizeof(book_t)];
	char ver_check[24];

	memset(book, 0, sizeof(book));


	
	

	in = fopen(filename, "rb");
	if(in == NULL) {
		printf("Book file %s not found.\n", filename);
		return FALSE;
	}

	
	if(fread(ver_check,sizeof(char),23,in) != 23) {
		fclose(in);
		return FALSE;
	}

	if(strncmp(BOOK_IO_VERSION, ver_check+18,4)) {
		fclose(in);
		printf("Cannot read book file %s. The format is not supported by this version of Fimbulwinter.\n", filename);
		printf("Please provide a book with IO version: %s\n\n", BOOK_IO_VERSION);
		return FALSE;
	}

	board_key = ZobristKey();

	for(i = 0;;i++) {
		fread(buffer, sizeof(book_t),1,in);
		if(feof(in)) break;
		memcpy(book+i,buffer, sizeof(book_t));
	}

	fclose(in);

	printf("Opening book %s loaded!\n", filename, i);

	return TRUE;
}

BOOL MakeBookFromPGN(char *pgn_file, char *fim_file, int ply)
{
	/*  Takes a PGN file containing any number of games and attempts
		to make a .fim book file for it. Attempt is the key word, since
		this kind of thing is rather hard to debug and if the PGN files
		are not formatted well, or if they're using some kind of weird
		conventions, this function will probably fail. So, in summary,
		if you've making new books for Fimbulwinter, you'd best know what
		you're doing.
	*/

	// This code is ugly because PGN is an ugly format for a programmer to 
	// deal with.

	FILE *in;
	FILE *out;
	
	char cmove[PGN_MOVE_LEN];
	char coord[PGN_MOVE_LEN];
	move curmove;
	book_t curbook;
	int cind = 0;
	int brackets = 0;
	int moves_parsed = 0;
	BOOL period = FALSE;
	int c;
	int games_parsed = 0;
	double start_time, end_time;
	int ply_count = 0;

	

	ReInit(TRUE);

	in = fopen(pgn_file, "rb");
	if(in == NULL) return FALSE;
	out = fopen(fim_file, "wb");
	if(out == NULL) {
		fclose(in);
		return FALSE;
	}

	start_time = GetTickCount();


	fprintf(out, "[Book IO Version: %s]", BOOK_IO_VERSION); // DO NOT change this tag!
	


	for(;;) {
		// Loads a single game 
		brackets = 0;
		moves_parsed = 0;
		cind = 0;
		period = FALSE;
		ply_count = 0;
		ReInit(FALSE);
		// get to the header of the file
		for(;;) {
			c = getc(in);
			if(c == EOF) goto done;
			if(c == '[') {brackets++; break;}
		}
		if(games_parsed == 8793) {
			cind = 0;
			}
		for(;;) {
			c = getc(in);
			//if(c == EOF) break;
			if(c == '[') {brackets++; continue;}
			if(c == ']') {brackets--; continue;}
			if(brackets == 0) { // Not inside of header fields
				if(c == '.') {period = TRUE; memset(cmove,'\0', PGN_MOVE_LEN); continue;}
				if(period == TRUE) {
					// Figure out how to check for EOF and end of gamefile
					if(c != ' ' && c != 10 && c != 13) { // 10 + 13 = \n
						cmove[cind] = (char)c;
						cind++;
					} else {
						if(cmove[0] != '\0') {
							// found player's move!
							// Make it
							PGNtoCoordMove(cmove, coord);
							curmove = MoveFromCoords(coord);
							assert(curmove.id != 0); // fails if move is illegal
							assert(IsLegalMove(curmove));
							curbook.Move = curmove;
							curbook.hashkey = board_key;
					//		curbook.win = 0;
					//		curbook.lose = 0;
					//		curbook.draw = 0;
							fwrite(&curbook, sizeof(book_t),1,out);
							MakeMove(curmove);
							// make move, set cind = 0, clear cmove, get 2nd player's move
							moves_parsed++;
							ply_count++;
							cind = 0;
							memset(cmove, '\0', PGN_MOVE_LEN);
							if(moves_parsed >= 2) {period = FALSE; moves_parsed = 0;}
							if(CheckEndOfGame(in) == TRUE || ply_count > ply) {
								games_parsed++; 
								printf("Games parsed: %i\n", games_parsed);
								break;
							}
						}
					}
				}
			}
		}
	}

done:

	fclose(in);
	fclose(out);

	end_time = GetTickCount();

	printf("Successfully parsed %i games in %.4g seconds.\n", games_parsed, (double)((long)end_time-(long)start_time)/1000);
	printf("Book IO Version: %s\n\n", BOOK_IO_VERSION);
	return TRUE;
}

BOOL CheckEndOfGame(FILE *file)
{
	// returns TRUE if we're at the end of the PGN file or at the end of a game
	
	// This is hard to do because PGN files need to be precisely formatted for this to 
	// work (ie moves must have only one space delimiter between them).
	long old_pos;
	long end_pos;
	BOOL result = FALSE;
	char buffer[10];
	long bytes_left;
	int c;


	old_pos = ftell(file);
	fseek(file, 0, SEEK_END);
	end_pos = ftell(file);
	fseek(file, old_pos, SEEK_SET);

	bytes_left = (long)(end_pos - old_pos);

	if(bytes_left < 3) {result = TRUE; goto unseek;}

	for(;;) {
		c = fgetc(file);
		if(c == EOF) {result = TRUE; goto unseek;}
		if(c != ' ' && c != 10 && c != 13) { // 10 + 13 = \n
			ungetc(c, file);
			break;
		}
	}


	memset(buffer,'\0', 10);
	fread(buffer, sizeof(char), 3, file);
	if(buffer[0] == '{') {result = TRUE; goto unseek;}
	if(!strcmp(buffer, "0-1")) {result = TRUE; goto unseek;}
	if(!strcmp(buffer, "1-0")) {result = TRUE; goto unseek;}
	fread(buffer+3, sizeof(char), 4, file);
	if(!strcmp(buffer,"1/2-1/2")) {result = TRUE; goto unseek;}


unseek:

	fseek(file, old_pos, SEEK_SET);

	return result;
}

BOOL LoadFEN(char *filename)
{
	// returns TRUE if file is successfully loaded
	// otherwise returns false
	FILE *input;
	char buffer[256];
	int count,i;

	char side_to_move[2];
	char castle_permissions[5];
	char ep_sqr[3];



	int row = 7;
	int col = 0;

	ReInit(TRUE);
	ClearBoard();

	input = fopen(filename, "r");
	if(input == NULL) return FALSE;
	count = fread(buffer, sizeof(char), 256, input);
	buffer[count] = '\0';

	for(i = 0; i < count; i++) {
		if(buffer[i] == '/') {
			row--;
			col = 0;
			continue;
		}
		if(buffer[i] == ' ') break; // got to data fields
		if(isdigit(buffer[i])) {
			col+= (buffer[i]-'1'+1);
			continue;
		}
		if(islower(buffer[i])) {
			// set it, then make it upper
			BOARD(col, row).color = BLACK;
			buffer[i]-=('a'-'A');
		} else {
			BOARD(col, row).color = WHITE;
		}
		switch(buffer[i]) {
		case 'P':
			BOARD(col, row).piece = PAWN;
			break;
		case 'N':
			BOARD(col, row).piece = KNIGHT;
			break;
		case 'B':
			BOARD(col, row).piece = BISHOP;
			break;
		case 'R':
			BOARD(col, row).piece = ROOK;
			break;
		case 'Q':
			BOARD(col, row).piece = QUEEN;
			break;
		case 'K':
			BOARD(col, row).piece = KING;
			break;
		}
		col++;
	}
	// board is loaded, now extract those data fields...
	sscanf((buffer+i),"%s %s %s %d %d", side_to_move, castle_permissions, ep_sqr, &fifty, &hply);
	hply--;
	if(side_to_move[0] == 'w') {
		side = WHITE;
		xside = BLACK;
	} else {
		side = BLACK;
		xside = WHITE;
	}
	castle = 0;
	if(strchr(castle_permissions,'K') != NULL) castle |= CASTLE_WHITEKINGSIDE;
	if(strchr(castle_permissions,'Q') != NULL) castle |= CASTLE_WHITEQUEENSIDE;
	if(strchr(castle_permissions,'k') != NULL) castle |= CASTLE_BLACKKINGSIDE;
	if(strchr(castle_permissions,'q') != NULL) castle |= CASTLE_BLACKQUEENSIDE;
	if(ep_sqr[0] != '-') enpassant = (ep_sqr[0] - 'a')+(8*(ep_sqr[1]-'1'));

	fclose(input);

	board_key = ZobristKey();
	
	return TRUE;
}

void PGNtoCoordMove(char *pgn, char *coord)
{
	// modifies pgn, taking the "+" off of a checking move, if it's there
	// Coord buffer size must be at least PGN_MOVE_LEN!!!!!!!!!!!!!
	int sqr;
	char *ptr;
	char mover[PGN_MOVE_LEN];
	char tcoord[2];
	int i, rank, file,piece;
	memset(coord,'\0',PGN_MOVE_LEN);
	if((ptr = strchr(pgn,'+')) != NULL) *ptr = '\0';
	if((ptr = strchr(pgn, '#')) != NULL) *ptr = '\0';

	// move is a pawn move
	if(strlen(pgn) == 2) {
		sqr = CoordToSqr(pgn);
		coord[0] = pgn[0];
		coord[2] = pgn[0];
		coord[3] = pgn[1];
		if(side == WHITE) {
			if(board[sqr-8].piece == PAWN) {
				coord[1] = pgn[1]-1;
			} else {
				coord[1] = pgn[1]-2;
			}
		} else {
			if(board[sqr+8].piece == PAWN) {
				coord[1] = pgn[1]+1;
			} else {
				coord[1] = pgn[1]+2;
			} 
		}
		return;
	} 
	
	if(!strcmp(pgn, "O-O") || !strcmp(pgn, "0-0")) { // 0-0 is non-stardard, but I'm trying to provide robust support here. Some people seem unable to hold to the PGN standard.
		if(side == WHITE) {
			memcpy(coord,"e1g1", 5);
		} else {
			memcpy(coord, "e8g8", 5);
		}
		return;
	}
	if(!strcmp(pgn, "O-O-O") || !strcmp(pgn, "0-0-0")) {
		if(side == WHITE) {
			memcpy(coord, "e1c1",5);
		} else {
			memcpy(coord, "e8c8",5);
		}
		return;
	}

	// Moves that can combo 
	// Like exf7=Q
	sqr = -1;
	memset(mover,'\0',PGN_MOVE_LEN);
	// hunt for the destination sqr

	if((ptr = strchr(pgn, 'x')) != NULL) {
		// This move is a capture!
		sqr = CoordToSqr(ptr+sizeof(char));
		memcpy(mover,pgn,ptr-pgn);
	}

	if((ptr = strchr(pgn, '=')) != NULL) {
		// This move is a promotion!
		coord[4] = *(ptr+sizeof(char));
		if(sqr == -1) {
			sqr = CoordToSqr(ptr-(2*sizeof(char)));
		}
		if(mover[0] == '\0') {
			memcpy(mover,ptr-sizeof(char),ptr-pgn);
		}
	}

	// Catch cases when a pawn capture = promotion
	// This appears to be a regular piece move
	if(sqr == -1) {
		sqr = CoordToSqr(pgn + strlen(pgn)-2); // dest sqr is in last two positions
	}
	if(mover[0] == '\0') {
		memcpy(mover, pgn, strlen(pgn)-2);
	}


	// mover now has piece to move in it
	// sqr now has the destination square in it
	if(islower(mover[0])) {
		// this is a pawn move
		if(strlen(mover) == 2) {
			memcpy(coord,mover,2);
			SqrToCoord(sqr, tcoord);
			memcpy(coord,tcoord,2);
			return;
		}
		for(i = 0; i < 8; i++) {
			if(BOARD(mover[0]-'a',i).piece == PAWN && BOARD(mover[0]-'a',i).color == side) {
				if(!Attacks((i*8)+mover[0]-'a',sqr,FALSE)) continue;
				coord[0] = mover[0];
				coord[1] = '1'+i;
				SqrToCoord(sqr, tcoord);
				memcpy(coord+2,tcoord,2);
				return;
			}
		}
		
	} else {
		rank = -1;
		file = -1;
		// This is a piece move
		if(strlen(mover) > 1) {
			// stupid disambiguation
			if(isdigit(mover[1])) {
				rank = mover[1]-'1';
			}
			if(isdigit(mover[2])) {
				rank = mover[2]-'1';
			}
			if(isalpha(mover[1])) {
				file = mover[1]-'a';
			}
		}


		piece = -10;
		if(mover[0] == 'Q') piece = QUEEN;
		if(mover[0] == 'K') piece = KING;
		if(mover[0] == 'N') piece = KNIGHT;
		if(mover[0] == 'B') piece = BISHOP;
		if(mover[0] == 'R') piece = ROOK;

		if(piece == -10) {
			printf("Piece not found, PGN Error!\n");
			return;
		}

		for(i = 0; i < 64; i++) {
			if(board[i].color == side && board[i].piece == piece) {
				if(rank == -1 || GETRANK(i) == rank) {
					if(file == -1 || GETFILE(i) == file) {
						if(!Attacks(i, sqr, FALSE)) continue;
						SqrToCoord(i, tcoord);
						memcpy(coord,tcoord, 2);
						SqrToCoord(sqr,tcoord);
						memcpy(coord+2, tcoord, 2);
						return;
					}
				}
			}
		}
	}
	printf("ERROR!!! PGN file is ill-formed!\n");
}



int CoordToSqr(char *coord)
{
	return (coord[0] - 'a')+(8*(coord[1]-'1'));
}
void SqrToCoord(int sqr, char *coord)
{
	coord[0] = 'a' + GETFILE(sqr);
	coord[1] = '1' + GETRANK(sqr);
}

