#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include "list"
using namespace std;

class State
{
	string stateName;
	int stateID;
	bool isStart, isFinal;
	list< pair<char, State*> > nextState;
	public:
		State(int stateCounter);
		void setStartState(bool flag);
		void setFinalState(bool flag);
		bool isStartState();
		bool isFinalState();
		int getStateID();
		string getStateName();
		char getTransitionChar();
		list<State*> getNextTransitionStates(char IP_char);
		list<State*> getEpsilonTransitionStates(char epsilon);
		State* getSymbolTransitionState(char IP_char);
		string getStateTransition(char IP_Char);
		void addNextState(char inputChar, State *nextStatePtr);
		void addNextState(char inputChar, list<State*> nextStates);
};

#endif
