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
	int numInputChars;
	char epsilon;
	list<char> inputChars;
	State *startState, *finalState;
	string originalRegex, postfixRegex; // originalRegex - Just for record, nothing else.
	int stateCounter;
	list<State*> states;
	
	// Functions
	bool isRegexValid(string regex);
	bool isOperator(char ch);
	int precedence(char op);
	void convertIntoPostfix(string regex);
	string transformRegex(string regex);
	void solveUnion(stack< pair<State*, State*> > *NFA_Stack);
	void solveConcatenation(stack< pair<State*, State*> > *NFA_Stack);
	void solveOptional(stack< pair<State*, State*> > *NFA_Stack);
	void solveClosure(stack< pair<State*, State*> > *NFA_Stack);
	void solveCross(stack< pair<State*, State*> > *NFA_Stack);
	void solveComplement(stack< pair<State*, State*> > *NFA_Stack);
	State* getStateByName(string name);
	void removeState(string name);
	bool notAlreadyAdded(char ch);
	
	public:
		NFA();
		~NFA();
		list<char> getInputChars();
		void recognizeInputChars(string regex);
		void generateEpsilonNFA(string regex);
		State* getStartState();
		State* getFinalState();
		void printTransitionTable();
};

#endif
