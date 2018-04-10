#include "state.h"

State::State(int stateCounter)
{
	stateID = stateCounter;
	stateName = "q" + to_string(stateID);
	isStart = false;
	isFinal = false;
}

void State::setStartState(bool flag)	{	this->isStart = flag;		}

void State::setFinalState(bool flag)	{	this->isFinal = flag;		}

bool State::isStartState()				{	return this->isStart;		}

bool State::isFinalState()				{	return this->isFinal;		}

int State::getStateID()					{	return this->stateID;		}

string State::getStateName()			{	return this->stateName;		}

char State::getTransitionChar()
{
	list< pair<char, State*> >::iterator it = nextState.begin();
	return (*it).first;
}

list<State*> State::getNextTransitionStates(char IP_char)
{
	list< pair<char, State*> >::iterator it = nextState.begin();
	list<State*> tempNextStates;
	for(it; it != nextState.end(); it++)
	{
		if((*it).first == IP_char)
		{
			tempNextStates.push_back((*it).second);
		}
	}
	return tempNextStates;
}

list<State*> State::getEpsilonTransitionStates(char epsilon)
{
	list< pair<char, State*> >::iterator it = nextState.begin();
	list<State*> tempNextStates;
	for(it; it != nextState.end(); it++)
	{
		if((*it).first == epsilon)
		{
			tempNextStates.push_back((*it).second);
		}
	}
	return tempNextStates;
}

State* State::getSymbolTransitionState(char IP_char)
{
	list< pair<char, State*> >::iterator it = nextState.begin();
	for(it; it != nextState.end(); it++)
	{
		if((*it).first == IP_char)
		{
			return (*it).second;
		}
	}
	return (State*)NULL;
}

string State::getStateTransition(char IP_Char)
{
	list< pair<char, State*> >::iterator it = nextState.begin();
	bool statesFound = false;
	string states;
	for(it; it != nextState.end(); it++)
	{
		if((*it).first == IP_Char)
		{
			if(states.length())
			{
				states += ", ";
			}
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

void State::addNextState(char inputChar, list<State*> nextStates)
{
	list<State*>::iterator it = nextStates.begin();
	for(it; it != nextStates.end(); it++)
	{
		this->nextState.push_back(make_pair(inputChar, (*it)));
	}
}
