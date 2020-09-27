/**
 * @file    dfa.h
 * @brief   Data type and function definitions associated with a DFA.
 * 
 * @author  J. Mouton
 */

typedef enum type {
    FINAL,
    NON_FINAL,
    SINK
} StateType;

typedef struct state {
    int id;
    StateType type;
    struct state **next;    
} State;

typedef struct edge {
    int from;               /**< Index of leaving state              */
    int to;                 /**< Index of transitioned to state      */
    char symbol;            /**< Input symbol on which to transition */
} Edge;

typedef struct dfaConfig {
    int sigma;              /**< Number of alphabet symbols */
    int valid_syms[10];     /**< Valid alphabet symbols     */
    int n;                  /**< Number of states           */
    int startState;         /**< Start state index          */
    int numFinals;          /**< Number of final states     */
    int *finalStates;       /**< Array of final states      */
    int numEdges;           /**< Number of edges            */
    Edge **edges;           /**< List of edges              */
} DFAConfig;

/** Structure used in linked list implementation of queue 
 *  for BFS of DFA. 
 */
typedef struct qnode {
    State *state_ptr;       /**< Enqueued state                               */
    char *str;              /**< Symbols encountered on path from start state */
    struct qnode *next;     /**< Next node in queue                           */
} Qnode;

/**
 * Parse DFA configuration from specification file.
 * 
 * @param   fp
 *     the (already open) source file.
 * @return  the DFA configuration information
 */
void parseDFA(FILE *fp);

/**
 * Construct DFA structure from the parsed configuration.
 * 
 * @return  the start state of the DFA
 */
State * constructDFA(void);

/**
 * Determine if DFA accepts input string.
 * 
 * @param   dfa
 *     start state of the DFA
 * @param   string
 *     input string
 * @return  0 if DFA accepts strings
 */
int accepts(State *start, char* string);

/**
 * Use BFS to finds the k smallest strings accepted by DFA, printed to stdout 
 * in lexicographic order. 
 * 
 * @param   dfa
 *     start state of the DFA
 * @param   k
 *     find k smallest strings
 */
void enumerate(State *start, int k);

/**
 * Check if final state can be reached from current state.
 * 
 * @param   s
 *      state from which to check reachability of final state
 */
int reachable(State *s);

/**
 * Performs DFS of DFA graph to determine reachability of final state.
 * 
 * @param   s
 *      State from which to test reachability
 * @param   visited
 *      Array of truth values about visited status
 */
int dfs(State *s, int *visited);

/**
 * Removes state from queue. Caller is responsible for freeing memory allocated
 * for removed Qnode.
 * 
 * @return  Qnode at front of queue
 */
Qnode * dequeue(void);

/**
 * Add State to queue. Initializes new queue if empty.
 * 
 * @param   s
 *     state to enqueue
 * @param   str
 *     alphabet symbol that transitioned DFA to state s
 */
void enqueue(State *s, char *str);

/**
 * Check if queue is empty
 * 
 * @return  0 if queue is empty
 */
int qIsEmpty(void);

/**
 * Free memory allocated to DFA structure.
 */
void freeDFAStates(void);

/**
 * Free memory allocated to DFA configuration structure.
 */
void freeDFAConfig(void);

/**
 * Free memory allocated to queue.
 */
void freeQeueu(void);

/**
 * Parse the provided alphabet.
 * 
 * @param   token
 *     Token to store type and value of input read from file
 */
void parseAlphabet(Token *token);

/**
 * Parse the provided number of states.
 * 
 * @param   token
 *     Token to store type and value of input read from file
 */
void parseNumStates(Token *token);

/**
 * Parse the provided start state.
 * 
 * @param   token
 *     Token to store type and value of input read from file
 */
void parseStart(Token *token);

/**
 * Parse the provided final state(s).
 * 
 * @param   token
 *     Token to store type and value of input read from file
 */
void parseFinal(Token *token);

/**
 * Parse the provided number of edges.
 * 
 * @param   token
 *     Token to store type and value of input read from file
 */
void parseNumEdges(Token *token);

/**
 * Parse the provided edges.
 * 
 * @param   token
 *     Token to store type and value of input read from file
 */
void parseEdges(Token *token);
