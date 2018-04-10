#ifndef DFA_H
#define DFA_H

#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include "list"
#include "vector"
#include "stack"
#include "queue"
#include "state.h"
#include "nfa.h"
using namespace std;

class DFA
{
//	char *transitionTable;
	int numInputChars;
	char epsilon;
	list<char> inputChars;
	State *startState, *finalState;
	int stateCounter;
	list< list<State*> > epsilonStatesSetList;
	list<State*> states;
	
	// Functions
	bool areSame(list<State*> source, list<State*> target);
	bool notAlreadyAdded(list<State*> states, State *state);
	bool statesAlreadyAdded(char transitionChar, list<State*> closureStates, list< pair< char, list<State*> > > listClosed);
	bool statesAlreadyAdded(list< pair< char, list<State*> > > listOpen, list< pair< char, list<State*> > > listClosed);
	list<State*> findEpsilonClosure(State *state);
	list<State*> findInputSymbolClosure(list<State*> epsilonClosureStates, char symbol);
	list<State*> removeDuplicateStates(list<State*> statesList);
	State* getNextState(list< pair< list<State*>, State*> > symbolClosureNewStates, list<State*> symbolClosureStates);
	public:
		DFA(list<char> inputChars);
		~DFA();
		void convertIntoDFA(NFA *nfa);
		void printTransitionTable();
		void printClosedStates(list< pair< char, list<State*> > > symbolClosureStatesQueue_closed);
		void printClosureStates(list<State*> states);
};

#endif
