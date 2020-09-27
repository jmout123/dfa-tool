#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "scanner.h"

#define FALSE 0
#define TRUE 1

FILE *file;
char buff[4000];
int line;
int col;

void initScanner(FILE *fp) {
    file = fp;
    col = -1;
    line = 0;
}

void closeScanner() {
    fclose(file);
}

int getToken(Token *token) {
    // Scan in new line
    if (col == -1) {
        if (fgets(buff,4000, file) == NULL) {
            return FALSE;
        }
        col = 0;
        line++;
    }

    // Search for start of next token
    while (isspace(buff[col]) || buff[col] == '\n' || buff[col] == '\0' || buff[col] == EOF) {
        if (buff[col] == EOF) {
            token->type = TOK_EOF;
            col = -1;
            return TRUE;
        }
        if (buff[col] == '\n' || buff[col] == '\0') {
            if (fgets(buff,4000, file) == NULL) {
                if (feof(file)) {
                    token->type = TOK_EOF;
                    return TRUE;
                } else {
                    return FALSE;
                }
            }
            col = 0;
            line++;
        } else {
            col++;
        }
     }

    // Scan next token
    // Token: 'sigma', 'start', 'final'
    if (strlen(&buff[col]) >= 5) {
        if (strncmp(&buff[col], "sigma", 5) == 0) {
            token->type = TOK_SIGMA;
            col += 5;
            return TRUE;
        } else if (strncmp(&buff[col], "start", 5) == 0) {
            token->type = TOK_START;
            col += 5;
            return TRUE;
        } else if (strncmp(&buff[col], "final", 5) == 0) {
            token->type = TOK_FINAL;
            col += 5;
            return TRUE;
        }
    }

    // Token: ['a'-'j'], 'n'
    if ('a' <= buff[col] && buff[col] <= 'j') {
        token->type = TOK_CHAR;
        token->value = buff[col];
        col++;
        return TRUE;
    }
    if (buff[col] == 'n') {
        token->type = TOK_N;
        col++;
        return TRUE;
    }

    // Token: '{', '}', '=', ','
    if (buff[col] == '{') {
        token->type = TOK_LBRACK;
        col++;
        return TRUE;
    }
    if (buff[col] == '}') {
        token->type = TOK_RBRACK;
        col++;
        return TRUE;
    }
    if (buff[col] == '=') {
        token->type = TOK_EQ;
        col++;
        return TRUE;
    }
    if (buff[col] == ',') {
        token->type = TOK_COMMA;
        col++;
        return TRUE;
    }
    
    // Token: int
    int num = atoi(&buff[col]);
    if (!(num == 0 && buff[col] != '0')) {
        token->type = TOK_NUM;
        token->value = num;
        col++;
        while ('0' <= buff[col] && buff[col] <= '9') col++;
        return TRUE;
    }

    token->type = TOK_UNDEF;
    return TRUE;
}

int getLine() {
    return line;
}

int getCol() {
    return col+1;
}