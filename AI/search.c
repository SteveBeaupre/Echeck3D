
// Search functions for Fimbulwinter

#include <stdio.h>
#include <stdlib.h> 
#include <time.h> 
#include <string.h>
#include <limits.h>
#include "defs.h"
#include "data.h"
#include "protos.h"
#include "eval.h"
#include "hash.h"
#include "book.h"

#include <setjmp.h>		// sweet beautiful GOTO...
#define CHECKUP_FREQ	1000000 // check the time left every X nodes
jmp_buf env;
BOOL stop_search;


int QDepth = 0;			// Quiescent Depth Counter

long nodes;  // number of nodes searched in alpha beta
long qnodes; // number of nodes searched in quiescent search


void Think(int format)	// output formats defined in defs.h
{

	int j,i, score;
	double time, last_time, cur_time;
	move bookmove;
	int total_nodes, total_qnodes;
	float ave_branch, ave_qbranch;
	int branch[MAX_PLY]; // for tracking branching factor
	int qbranch[MAX_PLY];

	memset(qbranch, 0, sizeof(qbranch));
	memset(branch, 0, sizeof(branch));
	memset(pv, 0, sizeof(pv));
	memset(hheuristic, 0, sizeof(hheuristic));
	ply = 0;
	nodes = 0;
	total_nodes = 0;
	total_qnodes = 0;
	qnodes = 0;
	QDepth = 0;

	// jump code thanks to Tom Kerrigan
	/* some code that lets us longjmp back here and return
	   from think() when our time is up */
	stop_search = FALSE;
	setjmp(env);
	if (stop_search) {
		
		/* make sure to take back the line we were searching */
		while (ply)
			UnmakeMove();
		return;
	}

//	

//	stop_time = GetTickCount();

//	
	

/*	From Tom Mann's Winboard docs -
The thinking output should be in the following format: 

ply score time nodes pv
Where: 
ply  Integer giving current search depth.  
score  Integer giving current evaluation in centipawns.  
time  Current search time in centiseconds (ex: 1028 = 10.28 seconds).  
nodes  Nodes searched.  
pv  Freeform text giving current "best" line. You can continue the pv onto another line if you start each continuation line with at least four space characters.  

Example: 

  9 156 1084 48000 Nf3 Nc6 Nc3 Nf6
Meaning: 

9 ply, score=1.56, time = 10.84 seconds, nodes=48000, PV = "Nf3 Nc6 Nc3 Nf6" 
*/


	
	start_time = GetTickCount();
	last_time = start_time;
	
	if(hply <= CHECK_IN_BOOK_UNTIL_PLY) {
		// Check to see if a book move is available
		bookmove = SearchBook();
		cur_time = GetTickCount();
		
		if(format == CONSOLE) {
			printf("Book %4.2g\n",(double)((long)cur_time-(long)last_time)/1000);
		}

		last_time = cur_time;

		if(bookmove.id != 0 && IsLegalMove(bookmove) == TRUE) {
			// found one
			pv[0][0].id = bookmove.id;
			pv_length[0] = 0;
			return;
		}
	}


	if (format == CONSOLE) {
		printf("ply  time    nodes   qnodes    total    score  pv\n");
	} 

	for (i = 1; i <= max_depth; ++i) {
		follow_pv = TRUE;
	//	score = TMiniMax(i);
		score = TAlphaBeta(i, -200000, 200000);//MiniMax(max_depth);
		
		

		cur_time = GetTickCount();
		
		if (format == CONSOLE) {
			printf("%2d  %5.2g%9d%9d  %9d %5d  ", i, (double)((long)cur_time-(long)last_time)/1000, nodes, qnodes, nodes+qnodes, score);
		} else {
			if (format == XBOARD) {
				printf("%d %d %d %d",i, score, (int)(((double)((long)GetTickCount()-(long)start_time)/1000)*100),nodes+qnodes);
				fflush(stdout);
			}
		}
		
		last_time = cur_time;

		if (format > NO_OUTPUT) {
			for (j = 0; j < pv_length[0]; ++j) 
				printf(" %s", MoveStr(pv[0][j]));
			printf("\n");
			fflush(stdout);
		}

		total_nodes += nodes;
		total_qnodes += qnodes;
		branch[i-1] = nodes;
		qbranch[i-1] = qnodes;
		nodes = 0;
		qnodes = 0;
	}

	time = (double)((long)GetTickCount()-(long)start_time)/1000;

	// Print branching factor
	if(format == CONSOLE) {
		ave_branch = 0;
		printf("Bfact: ");
		for(i = 0;;i++) {
			if(branch[i+1] != 0) {
				ave_branch += (float)branch[i+1]/branch[i];
				printf(" %.2f", (float)branch[i+1]/branch[i]);
			} else {
				if(i == 0) break;
				printf("  AveBfact: %.2f", ave_branch/i);
				break;
			}
		}
		printf("\n");
	}

	if(format == CONSOLE) {
		ave_qbranch = 0;
		printf("QBfact: ");
		for(i = 0;;i++) {
			if(qbranch[i+1] != 0) {
				ave_qbranch += (float)qbranch[i+1]/qbranch[i];
				printf(" %.2f", (float)qbranch[i+1]/qbranch[i]);
			} else {
				if(i == 0) break;
				printf("  AveQBfact: %.2f", ave_qbranch/i);
				break;
			}
		}
		printf("\n");
	}

	if(format == CONSOLE) printf("Total ABnodes: %d, Total qnodes: %d, Total nodes: %d\n", total_nodes, total_qnodes, total_nodes+total_qnodes);
	if(format == CONSOLE) printf("Execution time: %.4g seconds, %.4d nodes per second\n", time,(long)((total_nodes+total_qnodes)/time) );
	if(format == CONSOLE) printf("Max quiescent depth: %d\n\n", QDepth);


}

int MiniMax(int depth)
{
	int score;
	int best = INT_MIN;
	int i,j;
	BOOL check, legal;

	nodes++;
//	if(nodes % CHECKUP_FREQ == 0) CheckTime();

	pv_length[ply] = ply;

	if(depth < 1) {
		return EvaluateBoard();
	}

	if (ply && CountRepetitions())	// see if this is a draw by repetition
		return DRAW;
	

	GenerateMoves();

	check = InCheck(side);
	legal = FALSE;

	for(i = ms_begin[ply]; i < ms_end[ply]; i++) {
		if(MakeMove(movestack[i].m)) {
			legal = TRUE;
			score = -MiniMax(depth-1);
			UnmakeMove();
			if(score > best) {
				best = score;
				// Update PV
				pv[ply][ply] = movestack[i].m;
				
				for (j = ply + 1; j < pv_length[ply + 1]; ++j) {
					pv[ply][j] = pv[ply + 1][j];
				}

				pv_length[ply] = pv_length[ply + 1];
			}
		}
	}
		/* no legal moves? then we're in checkmate or stalemate */
	if (!legal) {
		if (check)
			return CHECKMATE + ply;
		else
			return DRAW;
	}

	/* fifty move draw rule */
	if (fifty >= 100)
		return DRAW;
	
	return best;
}

int TMiniMax(int depth)
{
	/*	Personally, I find the idea of a transposition table accelerated
		MiniMax search function to be intrinsically funny. That's what this is.
		I wrote it to test some of my hashing implementation and to help me
		figure out what is wrong with my TAlphaBeta function, which, at the 
		moment, is disgustingly broken.

		Good testbed to get Zobrist XORing working too...
	*/
	int score;
	int best = INT_MIN;
	int i,j;
	BOOL legal;
	move nm;
	nm.id = 0;

	nodes++;

	pv_length[ply] = ply;

	//score = TTableLookup(hply+depth,0,0,nm);
	//if(score != EVAL_UNKNOWN) return score;

	if(depth < 1) {
		score = TTableLookup(hply + depth, 0, 0, nm);
		if(score == EVAL_UNKNOWN) {
			score = EvaluateBoard();
			TTableInsert(nm, hply, score, hashfEXACT);
		} 
		return score;
	}

	if (ply && CountRepetitions())	// see if this is a draw by repetition
		return DRAW;
	

	GenerateMoves();

	legal = FALSE;

	for(i = ms_begin[ply]; i < ms_end[ply]; i++) {
		if(MakeMove(movestack[i].m)) {
			legal = TRUE;
			score = -TMiniMax(depth-1);
			UnmakeMove();
			if(score > best) {
				best = score;
				// Update PV
				pv[ply][ply] = movestack[i].m;
				
				for (j = ply + 1; j < pv_length[ply + 1]; ++j) {
					pv[ply][j] = pv[ply + 1][j];
				}

				pv_length[ply] = pv_length[ply + 1];
			}
		}
	}
		/* no legal moves? then we're in checkmate or stalemate */
	if (!legal) {
		if (InCheck(side))
			return CHECKMATE + ply;
		else
			return DRAW;
	}

	/* fifty move draw rule */
	if (fifty >= 100)
		return DRAW;
	
	return best;
}

int AlphaBeta(int depth, int alpha, int beta)
{
	int score;
	int i,j;
	BOOL legal;

	nodes++;
	//if(nodes % CHECKUP_FREQ == 0) CheckTime();

	pv_length[ply] = ply;

	if(depth < 1) {
		return QuiescentAlphaBeta(alpha, beta);
	//	return EvaluateBoard();
	}

	if (ply && CountRepetitions())	// see if this is a draw by repetition
		return DRAW;

	GenerateMoves();

	legal = FALSE;
	if(follow_pv) SortPV(); // great optimization! search the current PV first
	for(i = ms_begin[ply]; i < ms_end[ply]; i++) {
		Sort(i);
		if(MakeMove(movestack[i].m)) {
			legal = TRUE;
			score = -AlphaBeta(depth-1, -beta, -alpha);
			UnmakeMove();
			
			if(score >= beta) return beta;

			if(score > alpha) {
				alpha = score;

				// Used by MoveStackPush to try to order moves that have caused cutoffs
				// in the past higher
				hheuristic[(int)movestack[i].m.info.from][(int)movestack[i].m.info.to] += depth;


				// Update PV
				pv[ply][ply] = movestack[i].m;
				
				for (j = ply + 1; j < pv_length[ply + 1]; ++j) {
					pv[ply][j] = pv[ply + 1][j];
				}

				pv_length[ply] = pv_length[ply + 1];
			}
		}
	}
		/* no legal moves? then we're in checkmate or stalemate */
	if (!legal) {
		if (InCheck(side))
			return CHECKMATE + ply;
		else
			return DRAW;
	}

	/* fifty move draw rule */
	if (fifty >= 100)
		return DRAW;
	
	return alpha;
}

int QuiescentAlphaBeta(int alpha, int beta)
{
	int score,i;

	qnodes++;
//	pv_length[ply] = ply;

	score = EvaluateBoard();

	if(ply > QDepth) QDepth = ply;

	if(score >= beta) return beta;
	if(score > alpha) alpha = score;
	GenerateCaptures();
	
	for(i = ms_begin[ply]; i < ms_end[ply]; i++) {
		Sort(i);
		if(MakeMove(movestack[i].m)) {
			if(ply > QDepth) QDepth = ply;
			score = -QuiescentAlphaBeta(-beta, -alpha);
			UnmakeMove();
			if(score >= beta) return beta;
			if(score > alpha) {
				alpha = score;

				// I don't think that updating the PV here is a good 
				// idea. You might develop a PV that contains illegal
				// moves, since we don't check checks here. So to speak.

		/*		// Update PV
				pv[ply][ply] = movestack[i].m;
				
				for (j = ply + 1; j < pv_length[ply + 1]; ++j) {
					pv[ply][j] = pv[ply + 1][j];
				}

				pv_length[ply] = pv_length[ply + 1]; */
			}
		}
	}
	return alpha;
}

int TQuiescentAlphaBeta(int alpha, int beta, int depth)
{
	/*  Still unsure as to how functional this routine is. It's really hard to test.

		All I know is that:
			1) I don't see anything obviously wrong.
			2) It doesn't crash in a horrible horrible ball of fire.
			3) It seems to return the same results as QuiescentAlphaBeta().
			4) It can be several seconds faster than QuiescentAlphaBeta().

		Far from conclusive, these facts are enough to cause me to use this function
		until someone can point out my error(s). 

		If you find errors, please please email me!

		Not quite an error, but I am getting a bit of "quiescent explosion" - I often search
		more Qnodes than ABnodes. Maybe some move ordering would help. Or a depth cutoff.
	*/
	int score,i;
	int hashflag = hashfALPHA;
	
	move nm;
	nm.id = 0;


//	pv_length[ply] = ply;


	qnodes++;

//	score = TTableLookup(hply, alpha, beta, nm);
//	if (score == EVAL_UNKNOWN) {
		score = EvaluateBoard();
//		TTableInsert(nm, hply, score, hashfEXACT);
//	}

	if(depth <= 0) return score; // early bailout prevents quiescent explosion



	// Stats counter
	if(ply > QDepth) QDepth = ply;

	if(score >= beta) return beta;
	if(score > alpha) alpha = score;
	GenerateCaptures();
	
	for(i = ms_begin[ply]; i < ms_end[ply]; i++) {
		Sort(i);
		if(MakeMove(movestack[i].m)) {
			if(ply > QDepth) QDepth = ply;
			score = -TQuiescentAlphaBeta(-beta, -alpha, depth - 1);
			UnmakeMove();
			if(score >= beta) {
//				TTableInsert(nm, hply, score, hashfBETA);
				return beta;
			}
			if(score > alpha) {
				hashflag = hashfEXACT;
				alpha = score;

				// I don't think that updating the PV here is a good 
				// idea. You might develop a PV that contains illegal
				// moves, since we don't check checks here. So to speak.

		/*		// Update PV
				pv[ply][ply] = movestack[i].m;
				
				for (j = ply + 1; j < pv_length[ply + 1]; ++j) {
					pv[ply][j] = pv[ply + 1][j];
				}

				pv_length[ply] = pv_length[ply + 1]; */
			}
		}
	}
//	TTableInsert(nm, hply, alpha, hashflag);
	return alpha;
}

int TAlphaBeta(int depth, int alpha, int beta)
{
	// If not incredibly optimized, at least this seems to be finally working
	// The program spends a lot of time here, so optimizing would be worthwhile!

	// One thing: TAlphaBeta loses track of the last element of the pv_length array
	// when ply is even. This is fixed with a quick kludge in Think(). I don't think
	// that it's affecting the search.

	int score;
	int i,j;
	int hashflag = hashfALPHA;
	move nm;
	BOOL legal;

	nm.id = 0;

	nodes++;

	pv_length[ply] = ply;

//	score = TTableLookup(hply+depth+MAX_Q_DEPTH,alpha,beta,nm);
//	if(score != EVAL_UNKNOWN) return score;

	if(depth < 1) { // optimize this later by testing end condition before calling
		score = TTableLookup(hply, alpha, beta, nm);
		if(score == EVAL_UNKNOWN) {
			score = TQuiescentAlphaBeta(alpha, beta, MAX_Q_DEPTH);
			//score = EvaluateBoard();
			TTableInsert(nm, hply, score, hashfEXACT);		
		}
		return score;
	} 
	

	if (ply && CountRepetitions())	// see if this is a draw by repetition
		return DRAW;

	GenerateMoves();

	legal = FALSE;
	if(follow_pv) SortPV(); // great optimization! search the current PV first
	for(i = ms_begin[ply]; i < ms_end[ply]; i++) {
		Sort(i);
		if(MakeMove(movestack[i].m)) {
			legal = TRUE;
			score = -TAlphaBeta(depth-1, -beta, -alpha);
			UnmakeMove();
			if(score >= beta) {
	//			TTableInsert(nm, hply, score, hashfBETA);	
				return beta;
			}
			if(score > alpha) {
				hashflag = hashfEXACT;
				alpha = score;

				// Used by MoveStackPush to try to order moves that have caused cutoffs
				// in the past higher
				hheuristic[(int)movestack[i].m.info.from][(int)movestack[i].m.info.to] += depth;

				// Update PV
				pv[ply][ply] = movestack[i].m;
				
				for (j = ply + 1; j < pv_length[ply + 1]; ++j) {
					pv[ply][j] = pv[ply + 1][j];
				}

				pv_length[ply] = pv_length[ply + 1];
			}
		}
	}
		/* no legal moves? then we're in checkmate or stalemate */
	if (!legal) {
		if (InCheck(side))
			return CHECKMATE + ply;
		else
			return DRAW;
	}

	/* fifty move draw rule */
	if (fifty >= 100)
		return DRAW;
	
	//TTableInsert(nm, hply, alpha, hashflag);
	return alpha;
}



void Sort(int start)
{
	// Do a linear search through the current ply's movelist starting at start
	// and swap the best one with start.
	int best = -20000;
	int besti = start;
	int i;
	mstack_t temp;
	for(i = start; i < ms_end[ply]; i++) {
		if(movestack[i].score > best) {
			best = movestack[i].score;
			besti = i;
		}
	}
	if(besti == start) return;
	temp = movestack[start];
	movestack[start] = movestack[besti];
	movestack[besti] = temp;
	return;
}



// From Tom Kerrigan
/* sort_pv() is called when the search function is following
   the PV (Principal Variation). It looks through the current
   ply's move list to see if the PV move is there. If so,
   it adds 10,000,000 to the move's score so it's played first
   by the search function. If not, follow_pv remains FALSE and
   search() stops calling sort_pv(). */

void SortPV()
{
	int i;
	follow_pv = FALSE;
	for(i = ms_begin[ply]; i < ms_end[ply]; i++) {
		if(movestack[i].m.id == pv[0][ply].id) {
			follow_pv = TRUE;
			movestack[i].score += BASE_PV;
			return;
		}
	}
}

void CheckTime()
{
	// called every so often to make sure we're not using too much time
	if((GetTickCount() - start_time) > (unsigned long)max_time) {
		stop_search = TRUE;
		longjmp(env, 0);
	}
}
