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
		string getStateName();
		string getStateTransition(char IP_Char);
		void addNextState(char inputChar, State *nextStatePtr);
};

#endif
