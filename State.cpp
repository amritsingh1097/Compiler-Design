#include "state.h"

State::State(int stateCounter)
{
	stateID = stateCounter;
	stateName = "q" + to_string(stateID);
	isStart = false;
	isFinal = false;
}

void State::setStartState(bool flag) { this->isStart = flag; }

void State::setFinalState(bool flag) { this->isFinal = flag; }

bool State::isStartState() { return this->isStart; }

bool State::isFinalState() { return this->isFinal; }

string State::getStateName() { return this->stateName; }

string State::getStateTransition(char IP_Char)
{
	list< pair<char, State*> >::iterator it = nextState.begin();
	bool statesFound = false;
	string states;
	for(it; it != nextState.end(); it++)
	{
		if(states.length())
		{
			states += ", ";
		}
		if((*it).first == IP_Char)
		{
			statesFound = true;
			states += (*it).second->getStateName();
		}
	}
	if(statesFound)
	{
		return states;
	}
	return "-";
}

void State::addNextState(char inputChar, State *nextStatePtr)
{
	this->nextState.push_back(make_pair(inputChar, nextStatePtr));
}
