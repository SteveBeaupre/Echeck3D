extern float INI_BOOK_MEMORY_MB;	
extern int INI_BOOK_SIZE;	
extern char INI_BOOK_FILE[40];	

#define CHECK_IN_BOOK_UNTIL_PLY		20 // check for the first X plies if we have a book move

#define BOOK_IO_VERSION				"3.00" // MUST always be 4 chars long

// book entry
typedef struct {
	unsigned __int64 hashkey;
	move Move;
} book_t;

extern book_t	*book;