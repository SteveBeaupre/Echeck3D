extern int INI_TTABLE_MEMORY_MB;
extern int INI_TTABLE_SIZE;

extern unsigned __int64 ZobristKeys[6][2][64];
extern unsigned __int64 ZobristEPKeys[65]; // NO_EP_SQR is element 64
extern unsigned __int64 ZobristCastleKeys[16]; //15 is the largest value of castle, if all permission 
										//flags are set
extern unsigned __int64 BlackMoveKey;


#define    hashfEXACT   0
#define    hashfALPHA   1
#define    hashfBETA    2

#define		EVAL_UNKNOWN	-900000000 // must be lower than any possible positional
								   // eval
typedef struct {
    unsigned __int64 zkey;
    int depth;
    int flags;
    int eval;
    move best;
}   hash_t;

extern hash_t	*ttable;
