# dfa-tool
This tool reads a textual description of a DFA from a file, and declares, for a given input string, whether the string is accepted by the automaton, or not. It also allows the k smallest strings, accepted by the DFA, to be enumerated.

## Compile and Run

The tool can be compiled as follows:

```shell
make
```

## Execute

To test whether the language recognized by the provided DFA, accepts a specific string, run the tool as follows:

```shell
./dfa <path to input specification file> string
```

To enumerate the first k smallest strings accepted by the DFA, run the tool as follows:

```shell
./dfa <path to input file> X k
```

## DFA Specification format

The specification file should follow the following format:

```
sigma={(<char>,)* <char>}
n=<number of states>
start={<int>}
final={(<int>,)* <int>}
<number of edges>
list of edges as <int> <char> <int>
```

where

1. The alphabet, sigma, may only consist of the first 10 alphabet symbols [a-j], exluding the character 'e'.
2. The character 'e' is treated as the empty string symbol in both input and output.
3. There may be only one start state.
4. State indices must be in the range [0, n-1].
