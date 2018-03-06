#ifndef NFA_H
#define NFA_H

#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include "list"
#include "vector"
#include "state.h"
#include "stack"
using namespace std;

class NFA
{
	char *transitionTable;
	int numInputChars;
	char epsilon;
	char *inputChars;
	State *startState, *finalState;
	string originalRegex, postfixRegex; // originalRegex - Just for the record
	int stateCounter;
	list<State*> states;
	public:
		NFA(int numInputChar, char *inputChars);
		~NFA();
		bool isRegexValid(string regex);
		bool isOperator(char ch);
		int precedence(char op);
		void convertIntoPostfix(string regex);
		string transformRegex(string regex);
		void generateNFA(string regex);
		void solveUnion(stack< pair<State*, State*> > *NFA_Stack);
		void solveConcatenation(stack< pair<State*, State*> > *NFA_Stack);
		void solveOptional(stack< pair<State*, State*> > *NFA_Stack);
		void solveClosure(stack< pair<State*, State*> > *NFA_Stack);
		void solveCross(stack< pair<State*, State*> > *NFA_Stack);
		void solveComplement(stack< pair<State*, State*> > *NFA_Stack);
		void printTransitionTable();
};

#endif
