
#include <stdlib.h>
#include <string.h> 
#include <ctype.h>
#include <stdio.h>

#include "scanner.h"
#include "error.h"
#include "dfa.h"

#define FALSE 0
#define TRUE 1

// List of pointers to constructed states
State **states_ptr = NULL;
DFAConfig *dfa = NULL;

// Queue control position pointers
Qnode *front = NULL;
Qnode *rear = NULL;

void parseDFA(FILE *fp) {
    Token *token;
    dfa = (DFAConfig*) malloc(sizeof(DFAConfig));
    dfa->finalStates = NULL;
    dfa->edges = NULL;

    initScanner(fp);

    token = (Token*) malloc(sizeof(Token));
    token->type = TOK_UNDEF;
    token->value = '#';
    
    parseAlphabet(token);
    parseNumStates(token);
    parseStart(token);
    parseFinal(token);
    parseNumEdges(token);
    parseEdges(token);

    free(token);
    closeScanner();
}

void parseAlphabet(Token *token) {
    int status, i;
    char sym;
    int num_symbols = 0;

    for (i = 0; i < 10; i++) {
        dfa->valid_syms[i] = 0;
    }

    // 'sigma'
    if (getToken(token) != TRUE) {
        eprintf(ERR_PARSING, getLine(), getCol());
    } else if (token->type != TOK_SIGMA) {
        eprintf(ERR_ALPH_FORMAT, getLine(), getCol());
    }
    // '='
    if (getToken(token) != TRUE) {
        eprintf(ERR_PARSING, getLine(), getCol());
    } else if (token->type != TOK_EQ) {
        eprintf(ERR_ALPH_FORMAT, getLine(), getCol());
    }
    // '{'
    if (getToken(token) != TRUE) {
        eprintf(ERR_PARSING, getLine(), getCol());
    } else if (token->type != TOK_LBRACK) {
        eprintf(ERR_ALPH_FORMAT, getLine(), getCol());
    }

    // '(<char>, )* <char>}'
    status = getToken(token);
    while (status == TRUE && token->type != TOK_RBRACK) {
        if (token->type != TOK_CHAR) {
            eprintf(ERR_ALPH_FORMAT, getLine(), getCol());
        }
        sym = token->value;
        if (sym < 'a' || sym > 'j' || dfa->valid_syms[sym - 'a'] != 0 || sym == 'e') {
            eprintf(ERR_ALPH_SYMS, getLine(), getCol());
        } else {
            dfa->valid_syms[sym - 'a'] = 1;
            if (sym - 'a' >= num_symbols) {
                num_symbols = sym - 'a' + 1;
            }
        }
        if (getToken(token) != TRUE) {
            eprintf(ERR_PARSING, getLine(), getCol());
        } else if (!(token->type == TOK_COMMA || token->type == TOK_RBRACK)) {
            eprintf(ERR_ALPH_FORMAT, getLine(), getCol());
        }
        if (token->type == TOK_RBRACK) break;
        status = getToken(token);
        if (token->type == TOK_RBRACK) {
            eprintf(ERR_ALPH_FORMAT, getLine(), getCol()-1);
        }
    }

    if (status != TRUE) {
        eprintf(ERR_PARSING, getLine(), getCol());
    } else if (num_symbols == 0) {
        eprintf(ERR_NO_SYMS, getLine(), getCol());
    }

    dfa->sigma = num_symbols;
}

void parseNumStates(Token *token) {
    int num_states = 0;

    // 'n'
    if (getToken(token) != TRUE) {
        eprintf(ERR_PARSING, getLine(), getCol());
    } else if (token->type != TOK_N) {
        eprintf(ERR_NUM_STATES, getLine(), getCol());
    }
    // '='
    if (getToken(token) != TRUE) {
        eprintf(ERR_PARSING, getLine(), getCol());
    } else if (token->type != TOK_EQ) {
        eprintf(ERR_NUM_STATES, getLine(), getCol());
    }
    // '<int>'
    if (getToken(token) != TRUE) {
        eprintf(ERR_PARSING, getLine(), getCol());
    } else if (token->type != TOK_NUM || token->value < 0) {
        eprintf(ERR_NUM_STATES, getLine(), getCol());
    } else {
        num_states = token->value;
        dfa->n = num_states;
    }
}

void parseStart(Token *token) {
    int start = 0;

    //'start'
    if (getToken(token) != TRUE) {
        eprintf(ERR_PARSING, getLine(), getCol());
    } else if (token->type != TOK_START) {
        eprintf(ERR_START_FORMAT, getLine(), getCol());
    }
    // '='
    if (getToken(token) != TRUE) {
        eprintf(ERR_PARSING, getLine(), getCol());
    } else if (token->type != TOK_EQ) {
        eprintf(ERR_START_FORMAT, getLine(), getCol());
    }
    // '{'
    if (getToken(token) != TRUE) {
        eprintf(ERR_PARSING, getLine(), getCol());
    } else if (token->type != TOK_LBRACK) {
        eprintf(ERR_START_FORMAT, getLine(), getCol());
    }
    // '<int>'
    if (getToken(token) != TRUE) {
        eprintf(ERR_PARSING, getLine(), getCol());
    } else if (token->type != TOK_NUM) {
        if (token->type == TOK_RBRACK) {
            eprintf(ERR_NO_START, getLine(), getCol());
        } else {
            eprintf(ERR_START_FORMAT, getLine(), getCol());
        }
    } else {
        start = token->value;
        if (start < 0 || start >= dfa->n) {
            eprintf(ERR_INVALID_STATE, getLine(), getCol());
        }
        dfa->startState = start;
    }
    // '}'
    if (getToken(token) != TRUE) {
        eprintf(ERR_PARSING, getLine(), getCol());
    } else if (token->type != TOK_RBRACK) {
        if (token->type == TOK_COMMA) {
            eprintf(ERR_MANY_START, getLine(), getCol());
        } else {
            eprintf(ERR_START_FORMAT, getLine(), getCol());
        }
    }
}

void parseFinal(Token *token) {
    int *finals = (int*) malloc((dfa->n) * sizeof(int));
    int cnt = 0;
    int status, num, i;

    // 'final'
    if (getToken(token) != TRUE) {
        free(finals);
        eprintf(ERR_PARSING, getLine(), getCol());
    } else if (token->type != TOK_FINAL) {
        free(finals);
        eprintf(ERR_FINAL_FORMAT, getLine(), getCol());
    }
    // '='
    if (getToken(token) != TRUE) {
        free(finals);
        eprintf(ERR_PARSING, getLine(), getCol());
    } else if (token->type != TOK_EQ) {
        free(finals);
        eprintf(ERR_FINAL_FORMAT, getLine(), getCol());
    }
    // '{'
    if (getToken(token) != TRUE) {
        free(finals);
        eprintf(ERR_PARSING, getLine(), getCol());
    } else if (token->type != TOK_LBRACK) {
        free(finals);
        eprintf(ERR_FINAL_FORMAT, getLine(), getCol());
    }

    // '(<int>,)*, <int>}'
    status = getToken(token);
    while (status == TRUE && token->type != TOK_RBRACK) {
        if (token->type != TOK_NUM) {
            free(finals);
            eprintf(ERR_FINAL_FORMAT, getLine(), getCol());
        }
        num = token->value;
        if (num < 0 || num >= dfa->n) {
            free(finals);
            eprintf(ERR_INVALID_STATE, getLine(), getCol());
        } else {
            finals[cnt++] = num;
        }
        if (getToken(token) != TRUE) {
            free(finals);
            eprintf(ERR_PARSING, getLine(), getCol());
        } else if (!(token->type == TOK_COMMA || token->type == TOK_RBRACK)) {
            free(finals);
            eprintf(ERR_FINAL_FORMAT, getLine(), getCol());
        }
        if (token->type == TOK_RBRACK) break;
        status = getToken(token);
        if (token->type == TOK_RBRACK) {
            free(finals);
            eprintf(ERR_FINAL_FORMAT, getLine(), getCol()-1);
        }
    }

    if (status != TRUE) {
        free(finals);
        eprintf(ERR_PARSING, getLine(), getCol());
    } 

    dfa->numFinals = cnt;
    dfa->finalStates = (int*) malloc(cnt * sizeof(int));
    for (i = 0; i < cnt; i++) {
        dfa->finalStates[i] = finals[i];
    }

    free(finals);
}

void parseNumEdges(Token *token) {
    int num = 0;

    // <int>
    if (getToken(token) != TRUE) {
        eprintf(ERR_PARSING, getLine(), getCol());
    } else if (token->type != TOK_NUM) {
        eprintf(ERR_NUM_EDGES_FORMAT, getLine(), getCol());
    } else if (token->value < 0) {
        eprintf(ERR_INVALID_NUM_EDGES, getLine(), getCol());
    } else {
        num = token->value;
        dfa->numEdges = num;
    }
}

void parseEdges(Token *token) {
    int i, from, to;
    char sym;
    Edge *e;

    dfa->edges = (Edge**) malloc((dfa->numEdges) * sizeof(&e));
    for (i = 0; i < dfa->numEdges; i++) {
        dfa->edges[i] = NULL;
    }

    // '<int> <char> <int>'
    for (i = 0; i < dfa->numEdges; i++) {
        // Parse 'from' state
        if (getToken(token) != TRUE) {
            eprintf(ERR_PARSING, getLine(), getCol());
        } else if (token->type != TOK_NUM) {
            if (token->type == TOK_EOF) {
                eprintf(ERR_TOO_FEW_EDGES, getLine(), getCol());
            } else {
                eprintf(ERR_EDGES_FORMAT, getLine(), getCol());
            }
        } else if (token->value < 0 || token->value >= dfa->n) {
            eprintf(ERR_INVALID_STATE, getLine(), getCol());
        } else {
            from = token->value;
        }

        // Parse input symbol
        if (getToken(token) != TRUE) {
            eprintf(ERR_PARSING, getLine(), getCol());
        } else if (token->type != TOK_CHAR) {
            eprintf(ERR_EDGES_FORMAT, getLine(), getCol());
        } else if (token->value < 'a' || token->value > 'j' || dfa->valid_syms[(token->value) - 'a'] != 1) {
            eprintf(ERR_INVALID_SYM, getLine(), getCol());
        } else {
            sym = token->value;
        }

        // Parse 'from' state
        if (getToken(token) != TRUE) {
            eprintf(ERR_PARSING, getLine(), getCol());
        } else if (token->type != TOK_NUM) {
            eprintf(ERR_EDGES_FORMAT, getLine(), getCol());
        } else if (token->value < 0 || token->value >= dfa->n) {
            eprintf(ERR_INVALID_STATE, getLine(), getCol());
        } else {
            to = token->value;
        }

        e = (Edge*) malloc(sizeof(Edge));
        e->from = from;
        e->symbol = sym;
        e->to = to;
        dfa->edges[i] = e;
    }

    if (getToken(token) != TRUE) {
        eprintf(ERR_PARSING, getLine(), getCol());
    } else if (token->type != TOK_EOF) {
        eprintf(ERR_TOO_MANY_EDGES, getLine(), getCol());
    }
}

State * constructDFA() {
    int i, j, from, to, sym_idx;
    
    // Add extra sink state for undefined transitions
    int num_states = (dfa->n) + 1;
    int num_symbols = dfa->sigma;
    int num_finals = dfa->numFinals;

    // initialize list of pointers to states
    states_ptr = (State**) malloc(num_states * sizeof(*states_ptr));

    // initialize each of the states
    for (i = 0; i < num_states; i++) {
        states_ptr[i] = (State*) malloc(sizeof(State));
        states_ptr[i]->id = i;
        states_ptr[i]->type = NON_FINAL;
        states_ptr[i]->next = (State**) malloc(num_symbols * sizeof(*states_ptr));
    }
    for (i = 0; i < num_states; i++) {
        for (j = 0; j < num_symbols; j++) {
            states_ptr[i]->next[j] = NULL;
        }
    }
    states_ptr[num_states-1]->type = SINK;

    // Designate final states 
    for (i = 0; i < num_finals; i++) {
        states_ptr[dfa->finalStates[i]]->type = FINAL;
    }

    // Add edges 
    for (i = 0; i < dfa->numEdges; i++) {
        from = dfa->edges[i]->from;
        to = dfa->edges[i]->to;
        sym_idx = (dfa->edges[i]->symbol) - 'a';
        if (states_ptr[from]->next[sym_idx] == NULL) {
            states_ptr[from]->next[sym_idx] = states_ptr[to];
        } else {
            eprintf(ERR_INVALID_TRANS, 0, 0);
        }
    }

    // Let undefined transitions transition to sink state
    for (i = 0; i < num_states; i++) {
        for (j = 0; j < num_symbols; j++) {
            if (states_ptr[i]->next[j] == NULL) {
                states_ptr[i]->next[j] = states_ptr[num_states-1];
            }
        }
    }

    return states_ptr[dfa->startState];
}

int accepts(State *start, char* string) {
    int i, c;
    State *curr = start;

    // Check for empty string 
    if (strlen(string) == 1 && string[0] == 'e') {
        if (start->type == FINAL) {
            return TRUE;
        } else {
            return FALSE;
        }
    }

    for (i = 0; i < strlen(string); i++) {
        // Check for valid string input
        if (string[i] < 'a' || string[i] > 'j') {
            return FALSE;
        } else if (dfa->valid_syms[string[i] - 'a'] != 1) {
            return FALSE;
        }
        c = string[i] - 'a';
        curr = curr->next[c];
    }
    // If in final state at end of input
    if (curr->type == FINAL) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void enumerate(State *start, int k) {
    char *str;
    int cnt = 0;
    int i;
    Qnode *n;
    State *s;
    int *can_reach_final;

    // Avoid states from which final state cannot be reached
    can_reach_final = (int *) malloc(dfa->n * sizeof(int));
    for (i = 0; i < dfa->n; i++) {
        can_reach_final[i] = reachable(states_ptr[i]);
    }

    // Check if final state is reachable from start state
    if (dfa->numFinals == 0 || can_reach_final[start->id] == FALSE) {
        printf("DFA recognizes the empty language. Nothing to enumerate");
        return;
    }

    // initialize queue by enqueuing start state
    str = (char*) malloc(sizeof(char));
    str[0] = '\0';
    enqueue(start, str);

    // BFS to find first k smallest strings
    while (qIsEmpty() != TRUE && cnt < k) {
        n = dequeue();
        s = n->state_ptr;
        str = n->str;

        // If state is final -> output string found so far
        if (s->type == FINAL) {
            if (strlen(str) == 0) {
                printf("e ");
            } else {
                printf("%s ", str);
            }
            cnt++;
        }

        // enqueue states reachable from current state
        for (i = 0; i < dfa->sigma; i++) {
            // Don't enqueue sink state to avoid infinite loops
            if (s->next[i]->type == SINK || can_reach_final[s->next[i]->id] == FALSE) {
                continue;
            }
            char *cstr = (char*) malloc((strlen(str) + 2) * sizeof(char));
            strcpy(cstr, str);
            cstr[strlen(str)] = 'a' + i;
            cstr[strlen(str) + 1] = '\0';
            enqueue(s->next[i], cstr);
        }
        free(n);
        free(str);
    }
    free(can_reach_final);
    return;
}

int reachable(State *s) {
    int *visited;
    int i;
    int reached_final;
    
    visited = (int *) malloc(dfa->n * sizeof(int));
    for (i = 0; i < dfa->n; i++) {
        visited[i] = FALSE;
    }

    reached_final = dfs(s, visited);
    free(visited);
    return reached_final;
}

int dfs(State *s, int *visited) {
    int i;

    if (s->type == FINAL) return TRUE;
    
    visited[s->id] = TRUE;

    for (i = 0; i < dfa->sigma; i++) {
        if (visited[s->next[i]->id] == TRUE) continue;
        if (dfs(s->next[i], visited) == TRUE) return TRUE;
    }

    return FALSE;
}

void enqueue(State *s, char *str) {
    Qnode *new = (Qnode*) malloc(sizeof(Qnode));
    new->state_ptr = s;
    new->str = str;
    // If queue is empty
    if (front == NULL) {
        front = new;
        rear = new;
    }  else {
        rear->next = new;
        rear = new;
    }
}

Qnode * dequeue() {
    Qnode *to_remove = front;
    if (front == rear) {
        front = NULL;
        rear = NULL;
    } else {
        front = front->next;
    }
    return to_remove;
}

int qIsEmpty() {
    if (front == NULL) return TRUE;
    else return FALSE;
}

void freeDFAStates() {
    if (states_ptr == NULL) return;

    for (int i = 0; i < (dfa->n) + 1; i++) {
        if (states_ptr[i] != NULL) {
            if (states_ptr[i]->next != NULL) {
                free(states_ptr[i]->next);
            }
            free(states_ptr[i]);
        }        
    }
    free(states_ptr);
}

void freeDFAConfig() {
    int i;

    if (dfa == NULL) return;
    if (dfa->finalStates != NULL) {
        free(dfa->finalStates);
    }
    if (dfa->edges != NULL) {
        for (i = 0; i < dfa->numEdges; i++) {
            free(dfa->edges[i]);
        }
        free(dfa->edges);
    }
    free(dfa);
}

void freeQueue() {
    Qnode *n;

    while (qIsEmpty() != TRUE) {
        n = dequeue();
        free(n->str);
        free(n);
    }
}

void eprintf(ErrorCode err, int line, int col) {
    fflush(stdout);
    switch (err) {
        case ERR_OPEN_FILE:
            fprintf(stderr, "Error: Could not open file.");
            break;
        case ERR_PARSING:
            fprintf(stderr, "Error: Error occurred whilst parsing input file (line %d).", line);
            break;
        case ERR_ALPH_FORMAT:
            fprintf(stderr, "Error: Incorrect format for alphabet specification (line %d, col. %d). Expected 'sigma={(<char>,)* <char>}'.", line, col);
            break;
        case ERR_ALPH_SYMS:
            fprintf(stderr, "Error: Duplicate or illegal symbol provided (line %d, col. %d).", line, col);
            break;
        case ERR_NO_SYMS:
            fprintf(stderr, "Error: No alphabet symbols provided.");
            break;
        case ERR_NUM_STATES:
            fprintf(stderr, "Error: Incorrect format for specification of number of states (line %d, col. %d). Expected 'n= <integer (>= 0)>'.", line, col);
            break;
        case ERR_START_FORMAT:
            fprintf(stderr, "Error: Incorrect format for specification of start state (line %d, col. %d). Expected 'start={<integer>}'.", line, col);
            break;
        case ERR_NO_START:
            fprintf(stderr, "Error: Start state set is empty (line %d, col. %d).", line, col);
            break;
        case ERR_MANY_START:
            fprintf(stderr, "Error: DFAs require a single start state. More than one start state is given (line %d, col. %d).", line, col);
            break;
        case ERR_INVALID_STATE:
            fprintf(stderr, "Error: Invalid state specified (line %d, col. %d). Expected state in [0, n).", line, col);
            break;
        case ERR_FINAL_FORMAT:
            fprintf(stderr, "Error: Incorrect format for specification of final states (line %d, col. %d). Expected 'final={(<integer>,)* <integer>}'.", line, col);
            break;
        case ERR_NUM_EDGES_FORMAT:
            fprintf(stderr, "Error: Incorrect format for specification of number of edges (line %d, col. %d). Expected '<number of edges>'.", line, col);
            break;
        case ERR_INVALID_NUM_EDGES:
            fprintf(stderr, "Error: Invalid number of edges provided (line %d, col. %d). Expected number of edges in [0, n*|sigma|].", line, col);
            break;
        case ERR_EDGES_FORMAT:
            fprintf(stderr, "Error: Incorrect format for specification of edges (line %d, col. %d). Expected 'list of edges given as <node> <char> <node>'.", line, col);
            break;
        case ERR_INVALID_SYM:
            fprintf(stderr, "Error: Invalid input symbol provided based on specified alphabet (line %d, col. %d).", line, col);
            break;
        case ERR_TOO_MANY_EDGES:
            fprintf(stderr, "Error: Too many edges provided given specification for number of edges (line %d, col. %d).", line, col);
            break;
        case ERR_TOO_FEW_EDGES:
            fprintf(stderr, "Error: Too few edges provided given specification for number of edges (line %d, col. %d).", line, col);
            break;
        case ERR_INVALID_TRANS:
            fprintf(stderr, "Error: Invalid transition given. More than one edge leaving state for a given symbol is not allowed for DFAs.");
            break;
        default:
            fprintf(stderr, "Error: Unknown.");
    }

    closeScanner();
    freeDFAStates();
    freeDFAConfig();
    exit(2);
}

int main(int argc, char *argv[]) {
    FILE *fp;
    char *filename;
    char *string;
    State *start;
    int k;

    if (argc < 3) {
        printf("Error: Too few arguments provided.");
        return 1;
    } else if (*argv[2] == 'X') {
        if (argc < 4) {
            printf("Error: Too few arguments provided.");
            return 1;
        } else if (argc > 4) {
            printf("Error: Too many arguments provided.");
            return 1;
        } else {
            k = atoi(argv[3]);
            if (k == 0 && argv[3][0] != '0') {
                printf("Error: Icorrect arguments provided. Expected '<path to file> X <int>'");
            return 1;
            }
        }
    } else if (argc > 3) {
        printf("Error: Too many arguments provided.");
        return 1;
    }

    // Construct DFA structure
    filename = argv[1];
    fp = fopen(filename, "r");
    if (fp == NULL) {
        eprintf(ERR_OPEN_FILE, 0, 0);
    }
    parseDFA(fp);
    start = constructDFA();

    if (*argv[2] == 'X') {
        enumerate(start, k);

    } else {
        string = argv[2];
        int accept = accepts(start, string);
        if (accept == TRUE) {
            printf("Accepted");
        } else {
            printf("Not accepted");
        }
    }
    
    // Free memory
    freeDFAStates();
    freeDFAConfig();
    freeQueue();

    return 0;
}

