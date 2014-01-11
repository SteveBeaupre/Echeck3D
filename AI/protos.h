#include <windows.h>
#include <stdio.h>


// Main.c
void DrawBoard();
void Init();
void ReInit(BOOL clear_hash_table);
char *MoveStr(move Move);
void ListPossibleMoves(BOOL verify);
void ClearBoard();
void ParsePutPiece(char *s);
BOOL GameStateCheck();
void xboard();
void TSCP_Bench();
BOOL IsCoordNotationMove(char *buffer);


// Board.c
void InitBoard();
void GenerateMoves();
void GeneratePawnMoves();
void GeneratePieceMoves();
void GenerateCaptures();
void MoveStackPush(int from, int to, int bits);
BOOL MakeMove(move Move);
void UnmakeMove();
void MoveStackPushPromotion(int from, int to, int bits);
BOOL Attacked(int sqr, int s);
BOOL InCheck(int s);
int CountRepetitions();
move MoveFromCoords(char *s);
BOOL Attacks(int sqr1, int sqr2, BOOL ValidateSqr2HasPiece);
BOOL IsLegalMove(move Move);


// Data.c
void InitRays();

// Eval.c
int EvaluateBoard();
int KingSafety();
void BuildPawnLists();
int PawnEval(int color);

// Search.c
void Think(int format);
int MiniMax(int depth);
int TMiniMax(int depth);
int AlphaBeta(int depth, int alpha, int beta);
int TAlphaBeta(int depth, int alpha, int beta);
int QuiescentAlphaBeta(int alpha, int beta);
int TQuiescentAlphaBeta(int alpha, int beta, int depth);
void CheckTime();
void Sort(int start);
void SortPV();

// Log.c
void WriteToLog(char *str);

// FileIO.c
BOOL LoadFEN(char *filename);
BOOL LoadBook(char *filename);
int GetBookSize(char *filename);
BOOL MakeBookFromPGN(char *pgn_file, char *fim_file, int ply);
void PGNtoCoordMove(char *pgn, char *coord);
int CoordToSqr(char *coord);
void SqrToCoord(int sqr, char *coord);
BOOL CheckEndOfGame(FILE *file);
void ReadINIFile(char *filename);
void SetINIKey(char *key, char *buffer, BOOL default_val);
int KeyNum(const char *key);

// Book.c
void InitBook();
void BookCleanup(void);
move SearchBook();

// Hash.c
void TTableInit(void);
void TTableCleanup(void);
unsigned __int64 rand64();
void TTableInsert(move BestMove, int depth, int eval, int hashflags);
int TTableLookup(int depth, int alpha, int beta, move BestMove);
unsigned __int64 ZobristKey();
void InitZobristKeys();
void ClearTTable();


// Stats.c
void StatsDump_TTable();
void ClearOnePlyStats();
void TTableCrossSection();
