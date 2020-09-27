/**
 * @file    scanner.h
 * @brief   Definitions of tokens recognized and scanner function definitions.
 * 
 * @author  J. Mouton
 */

typedef enum {
    TOK_EOF,
    TOK_NUM,
    TOK_SIGMA,
    TOK_START,
    TOK_FINAL,
    TOK_N,
    TOK_CHAR,
    TOK_EQ,
    TOK_LBRACK,
    TOK_RBRACK,
    TOK_COMMA,
    TOK_UNDEF,
} TokenType;

typedef struct token {
    TokenType type;
    int value;
} Token;

/**
 * Intialises the scanner.
 *
 * @param   fp
 *     the (already open) source file
 */
void initScanner(FILE *fp);

/**
 * Close FILE input stream.
 */
void closeScanner(void);

/**
 * Gets the next token from the input (source) file.
 *
 * @param  token
 *     contains the token just scanned
 */
int getToken(Token *token);

/**
 * Get the current column position of the scanner.
 * 
 * @return the current column position
 */
int getCol();

/**
 * Get the current line number of the scanner.
 * 
 * @return the current line number
 */
int getLine();