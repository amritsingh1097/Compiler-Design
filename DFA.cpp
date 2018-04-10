// NOTE - This NFA to DFA conversion isn't working for all regular expressions.
// There is still some modifications to be done to make it work for all the regular expressions.

#include "dfa.h"
#include "tokenizer.h"
#include "state.h"
#include "nfa.h"

DFA::DFA(list<char> inputChars)
{
	this->epsilon = 238;
	this->numInputChars = inputChars.size();
	this->inputChars = inputChars;
	this->stateCounter = 0;
}

bool DFA::areSame(list<State*> sourceList, list<State*> targetList)
{
	list<State*>::iterator sourceIt = sourceList.begin();
	list<State*>::iterator targetIt = targetList.begin();
	bool flag = false;
	
	for(sourceIt; sourceIt != sourceList.end(); sourceIt++)
	{
		for(targetIt; targetIt != targetList.end(); targetIt++)
		{
			if((*sourceIt)->getStateID() == (*targetIt)->getStateID())
			{
				flag = true;
				break;
			}
		}
		if(!flag)
		{
			return false;
		}
		flag = false;
	}
	return true;
}

bool DFA::notAlreadyAdded(list<State*> states, State *state)
{
	list<State*>::iterator it = states.begin();
	for(it; it != states.end(); it++)
	{
		if((*it)->getStateID() == state->getStateID())
		{
			return false;
		}
	}
	return true;
}

bool DFA::statesAlreadyAdded(list< pair< char, list<State*> > > listOpen, list< pair< char, list<State*> > > listClosed)
{
	list<char>::iterator charIt = inputChars.begin();
	charIt++;
	list< pair< char, list<State*> > >::iterator openIt = listOpen.begin();
	list< pair< char, list<State*> > >::iterator closedIt = listClosed.begin();
	
	while((*openIt).first != (*charIt))
	{
		openIt++;
	}
	for(openIt = listOpen.begin(); openIt != listOpen.end(); openIt++)
	{
		for(closedIt; closedIt != listClosed.end(); closedIt++)
		{
			if(((*openIt).first == (*closedIt).first) && areSame((*openIt).second, (*closedIt).second))
			{
				return true;
			}
		}
	}
	return false;
}

bool DFA::statesAlreadyAdded(char transitionChar, list<State*> closureStates, list< pair< char, list<State*> > > listClosed)
{
	list< pair< char, list<State*> > >::iterator closedIt = listClosed.begin();
	
	for(closedIt; closedIt != listClosed.end(); closedIt++)
	{
		if(((*closedIt).first == transitionChar) && areSame(closureStates, (*closedIt).second))
		{
			return true;
		}
	}
	return false;
}

list<State*> DFA::findEpsilonClosure(State *state)
{
	list<State*> epsilonClosureStates;
	State *tempState = state;
	epsilonClosureStates.push_back(state);
	
	list<State*>::iterator it = epsilonClosureStates.begin();
	for(it; it != epsilonClosureStates.end(); it++)
	{
		epsilonClosureStates.splice(epsilonClosureStates.end(), (*it)->getEpsilonTransitionStates(this->epsilon));
	}
	return epsilonClosureStates;
}

list<State*> DFA::findInputSymbolClosure(list<State*> epsilonClosureStates, char symbol)
{
	list<State*> symbolClosureStates;
	list<State*>::iterator it = epsilonClosureStates.begin();
	State *tempState;
	
	for(it; it != epsilonClosureStates.end(); it++)
	{
		tempState = *it;
		State *newState = tempState->getSymbolTransitionState(symbol);
		if(newState != (State*)NULL)// && notAlreadyAdded(symbolClosureStates, newState))
		{
			symbolClosureStates.push_back(newState);
		}
	}
	return symbolClosureStates;
}

list<State*> DFA::removeDuplicateStates(list<State*> statesList)
{
	list<State*> tempList;
	list<State*>::iterator it = statesList.begin();
	for(it; it != statesList.end(); it++)
	{
		if(notAlreadyAdded(tempList, (*it)))
		{
			tempList.push_back( (*it) );
		}
	}
	return tempList;
}

State* DFA::getNextState(list< pair< list<State*>, State*> > symbolClosureNewStates, list<State*> symbolClosureStates)
{
	list< pair< list<State*>, State*> >::iterator it = symbolClosureNewStates.begin();
	for(it; it != symbolClosureNewStates.end(); it++)
	{
		if( areSame((*it).first, symbolClosureStates) )
		{
			return (*it).second;
		}
	}
}

void DFA::convertIntoDFA(NFA *nfa)
{
	list< pair< char, list<State*> > > symbolClosureStatesQueue_open, symbolClosureStatesQueue_closed;
	list< list<State*> > epsilonClosureStates_closed;
	
	// For start state - Finding epsilon closure and then symbol closure states for the start state.
	list<State*> symbolClosureStates;
	symbolClosureStates.push_back(nfa->getStartState());
	
	list< list<State*> > epsilonClosureStates;
	list<State*> tempEpsilonClosureStates;
	list<State*>::iterator symbolClosureIt = symbolClosureStates.begin();
	for(symbolClosureIt; symbolClosureIt != symbolClosureStates.end(); symbolClosureIt++)
	{
		tempEpsilonClosureStates = findEpsilonClosure((*symbolClosureIt));
	}
	
	epsilonClosureStates.push_back(tempEpsilonClosureStates);
	epsilonClosureStates_closed.push_back(tempEpsilonClosureStates);
	this->epsilonStatesSetList.push_back(tempEpsilonClosureStates);
	
	State *newState = new State(this->stateCounter++);
	State *currState = newState;
	newState->setStartState(true);
	newState->setFinalState(false);
	this->states.push_back(newState);
	this->startState = newState;
	
	list<char>::iterator charIt = inputChars.begin();
	charIt++;
	for(charIt; charIt != inputChars.end(); charIt++)
	{
		symbolClosureStates.clear();
		symbolClosureStates = findInputSymbolClosure(tempEpsilonClosureStates, (*charIt));
		symbolClosureStatesQueue_open.push_back( make_pair( (*charIt), symbolClosureStates ) );
	}
	
	list<State*> newStatesList;
	newStatesList.push_back(newState);
	list< pair< list<State*>, State*> > symbolClosureNewStates;		// (9,1) -> q1

	list< list<State*> >::iterator epsilonClosureIt = epsilonClosureStates.begin();
	
	// Loop for intermediate states
	do
	{
		currState = newStatesList.front();
		newStatesList.pop_front();
		
		list<State*> tempEpsilonClosureStates;
		list< pair< char, list<State*> > >::iterator openIt = symbolClosureStatesQueue_open.begin();
		epsilonClosureStates.clear();
		
		charIt = inputChars.begin();
		charIt++;
		for(charIt; charIt != inputChars.end(); charIt++)
		{
			char transitionChar = symbolClosureStatesQueue_open.front().first;
			symbolClosureStates.clear();
			symbolClosureStates = symbolClosureStatesQueue_open.front().second;
			
			if(statesAlreadyAdded(transitionChar, symbolClosureStates, symbolClosureStatesQueue_closed))
			{
				symbolClosureStatesQueue_closed.push_back( symbolClosureStatesQueue_open.front() );
				symbolClosureStatesQueue_open.pop_front();
				currState->addNextState( transitionChar, getNextState(symbolClosureNewStates, symbolClosureStates) );
				continue;
			}
			
			symbolClosureStatesQueue_closed.push_back( symbolClosureStatesQueue_open.front() );
			symbolClosureStatesQueue_open.pop_front();
			
			for(symbolClosureIt = symbolClosureStates.begin(); symbolClosureIt != symbolClosureStates.end(); symbolClosureIt++)
			{
				tempEpsilonClosureStates.splice(tempEpsilonClosureStates.end(), findEpsilonClosure((*symbolClosureIt)));
			}
			tempEpsilonClosureStates = removeDuplicateStates(tempEpsilonClosureStates);
			epsilonClosureStates.push_back( tempEpsilonClosureStates );
			epsilonClosureStates_closed.push_back(tempEpsilonClosureStates);
			tempEpsilonClosureStates.clear();
			
			newState = new State(this->stateCounter++);
			this->states.push_back(newState);
			currState->addNextState(transitionChar, newState);
			newStatesList.push_back(newState);
			
			symbolClosureNewStates.push_back( make_pair(symbolClosureStates, newState) );
		}
		
		this->epsilonStatesSetList.insert(epsilonStatesSetList.end(), epsilonClosureStates.begin(), epsilonClosureStates.end());
		
		for(epsilonClosureIt = epsilonClosureStates.begin(); epsilonClosureIt != epsilonClosureStates.end(); epsilonClosureIt++)
		{
			charIt = inputChars.begin();
			charIt++;
			for(charIt; charIt != inputChars.end(); charIt++)
			{
				symbolClosureStates.clear();
				symbolClosureStates = findInputSymbolClosure((*epsilonClosureIt), (*charIt));
				symbolClosureStatesQueue_open.push_back( make_pair( (*charIt), symbolClosureStates ) );
			}
		}
	} while( symbolClosureStatesQueue_open.size() );
	
	// Setting final state
	list< list<State*> >::iterator epIt = epsilonStatesSetList.begin();
	list<State*>::iterator statesIt = states.begin();
	for(epIt; epIt != epsilonStatesSetList.end(); epIt++, statesIt++)
	{
		if( !notAlreadyAdded( (*epIt), nfa->getFinalState() ) )
		{
			(*statesIt)->setFinalState(true);
		}
	}
	
	this->inputChars.remove(238);
	printTransitionTable();
}

void DFA::printClosedStates(list< pair< char, list<State*> > > symbolClosureStatesQueue_closed)
{
	list< pair< char, list<State*> > >::iterator stateIt = symbolClosureStatesQueue_closed.begin();
	for(stateIt; stateIt != symbolClosureStatesQueue_closed.end(); stateIt++)
	{
		printClosureStates((*stateIt).second);
		cout << endl;
	}
}

void DFA::printTransitionTable()
{
	cout << setw(10) << left << "\tStates\\IP";
	list<char>::iterator charIt = inputChars.begin();
	for(charIt; charIt != inputChars.end(); charIt++)
	{
		cout << "\t" << setw(10) << left << (*charIt);
	}
	cout << endl;
	list<State*>::iterator statesIt = states.begin();
	for(statesIt; statesIt != states.end(); statesIt++)
	{
		if( (*statesIt)->isStartState() && (*statesIt)->isFinalState() )
		{
			cout << " O>";
		}
		else if((*statesIt)->isStartState())
		{
			cout << " -->";
		}
		else if((*statesIt)->isFinalState())
		{
			cout << "  O";
		}
		cout << "\t" <<  setw(10) << left << (*statesIt)->getStateName();
		for(charIt = inputChars.begin(); charIt != inputChars.end(); charIt++)
		{
			cout << "\t" << setw(10) << left << (*statesIt)->getStateTransition((*charIt));
		}
		cout << endl;
	}
	cout << endl << "Total States: " << states.size() << endl;
}

void DFA::printClosureStates(list<State*> states)
{
	list<State*>::iterator it = states.begin();
	for(it; it != states.end(); it++)
	{
		cout << "State: " << (*it)->getStateName() << "\tState ID: " << (*it)->getStateID() << endl;
	}
}
