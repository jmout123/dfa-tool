/**
 * @file    error.h
 * @brief   Error codes used in input validation.
 * 
 * @author  J. Mouton
 */

typedef enum {
    ERR_OPEN_FILE,
    ERR_PARSING,
    
    ERR_ALPH_FORMAT,
    ERR_ALPH_SYMS,
    ERR_NO_SYMS,

    ERR_NUM_STATES,

    ERR_START_FORMAT,
    ERR_NO_START,
    ERR_MANY_START,
    ERR_INVALID_STATE,

    ERR_FINAL_FORMAT,

    ERR_NUM_EDGES_FORMAT,
    ERR_INVALID_NUM_EDGES,

    ERR_EDGES_FORMAT,
    ERR_INVALID_SYM,
    ERR_TOO_MANY_EDGES,
    ERR_TOO_FEW_EDGES,

    ERR_INVALID_TRANS,
} ErrorCode;

/**
 * Print error msg to stderr and exit.
 */
void eprintf(ErrorCode err, int line, int col);