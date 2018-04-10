#include "nfa.h"
#include "tokenizer.h"

NFA::NFA()
{
	this->originalRegex = "";
	this->postfixRegex = "";
	this->epsilon = 238;
	this->numInputChars = 0;
	this->stateCounter = 0;
}

list<char> NFA::getInputChars()
{
	return this->inputChars;
}

bool NFA::isRegexValid(string regex)
{
	list<char> tempStack;
	for(int i=0; i<regex.length(); i++)
	{
		if(regex[i] == '(')
		{
			tempStack.push_back(regex[i]);
		}
		else if(regex[i] == ')' && tempStack.size())
		{
			tempStack.pop_back();
		}
		else if(regex[i] == ')' && !tempStack.size())
		{
			return false;
		}
	}
	if(tempStack.size())
	{
		return false;
	}
	return true;
}

bool NFA::isOperator(char ch)
{
	return (ch == '.' || ch == '|' || ch == '?' || ch == '+' || ch == '*' || ch == '^');
}

int NFA::precedence(char op)
{
	switch(op)
	{
		case '(':	return 1;
		case '|':	return 2;
		case '.':	return 3;
		case '?':	return 4;
		case '+':	return 4;
		case '*':	return 4;
		case '^':	return 5;
		default :	return 0;
	}
}

void NFA::convertIntoPostfix(string regex)
{
	stack<char> tempStack;
	this->postfixRegex = "";
	for(int i=0; i<regex.length(); i++)
	{
		if(regex[i] == '(')
		{
			tempStack.push(regex[i]);
		}
		else if(regex[i] == ')')
		{
			while(tempStack.top() != '(')
			{
				this->postfixRegex += tempStack.top();
				tempStack.pop();
			}
			tempStack.pop();   // Pop '('
		}
		else if(isOperator(regex[i]) && (tempStack.empty() || precedence(regex[i]) >= precedence(tempStack.top())))
		{
			tempStack.push(regex[i]);
		}
		else if(isOperator(regex[i]) && (precedence(regex[i]) < precedence(tempStack.top())))
		{
			while(!tempStack.empty() && (precedence(tempStack.top()) > precedence(regex[i])))
			{
				this->postfixRegex += tempStack.top();
				tempStack.pop();
			}
			tempStack.push(regex[i]);
		}
		else
		{
			this->postfixRegex += regex[i];
		}
	}

	// If stack is not empty after the string has been exausted
	while(!tempStack.empty())
	{
		this->postfixRegex += tempStack.top();
		tempStack.pop();
	}
}

string NFA::transformRegex(string regex)
{
	string tempRegex = regex;
	regex.clear();
	char lastChar;
	bool isLastCharOp = false;
	
	for(int i=0; i<tempRegex.length(); i++)
	{
		if((!isOperator(tempRegex[i]) && tempRegex[i] != '(' && tempRegex[i] != ')') || (tempRegex[i] == '*' || tempRegex[i] == '+'))
		{
			regex += tempRegex[i];
			if((i+1 < tempRegex.length()) && !isOperator(tempRegex[i+1]) && tempRegex[i+1] != ')')
			{
				regex += ".";
			}
		}
		else if(tempRegex[i] == ')')
		{
			regex += tempRegex[i];
			if((i+1 < tempRegex.length()) && !isOperator(tempRegex[i+1]))
			{
				regex += ".";
			}
		}
		else
		{
			regex += tempRegex[i];
		}
	}
	return regex;
}

bool NFA::notAlreadyAdded(char ch)
{
	list<char>::iterator it = inputChars.begin();
	for(it; it != inputChars.end(); it++)
	{
		if(ch == (*it))	return false;
	}
	return true;
}

void NFA::recognizeInputChars(string regex)
{
	inputChars.push_back(this->epsilon);
	for(int i=0; i < regex.length(); i++)
	{
		if(!isOperator(regex[i]) && regex[i] != '(' && regex[i] != ')' && notAlreadyAdded(regex[i]))
		{
			inputChars.push_back(regex[i]);
		}
	}
	numInputChars = inputChars.size();
}

void NFA::generateEpsilonNFA(string regex)
{
	if(!isRegexValid(regex))
	{
		return;
	}
	recognizeInputChars(regex);
	this->originalRegex = regex;
	cout << "Regular Expression: " << regex << endl;
	regex = transformRegex(regex);
	convertIntoPostfix(regex);

	stack< pair<State*, State*> > NFA_Stack;
	for(int i=0; i<postfixRegex.length(); i++)
	{
		if(!isOperator(postfixRegex[i]))
		{
			pair<State*, State*> newNFA;
			newNFA.first = new State(this->stateCounter++);
			newNFA.second = new State(this->stateCounter++);
			newNFA.first->addNextState(postfixRegex[i], newNFA.second);

			newNFA.first->setStartState(true);
			newNFA.second->setFinalState(true);
			this->startState = newNFA.first;
			this->finalState = newNFA.second;
			states.push_back(newNFA.first);
			states.push_back(newNFA.second);

			NFA_Stack.push(make_pair(this->startState, this->finalState));
		}
		if(postfixRegex[i] == '|')
		{
			solveUnion(&NFA_Stack);
		}
		if(postfixRegex[i] == '*')
		{
			solveClosure(&NFA_Stack);
		}
		if(postfixRegex[i] == '+')
		{
			solveCross(&NFA_Stack);
		}
		if(postfixRegex[i] == '.')
		{
			solveConcatenation(&NFA_Stack);
		}
	}
}

void NFA::solveUnion(stack< pair<State*, State*> > *NFA_Stack)
{
	pair<State*, State*> oldNFA1 = NFA_Stack->top();
	NFA_Stack->pop();
	oldNFA1.first->setStartState(false);
	oldNFA1.second->setFinalState(false);
	pair<State*, State*> oldNFA2 = NFA_Stack->top();
	NFA_Stack->pop();
	oldNFA2.first->setStartState(false);
	oldNFA2.second->setFinalState(false);
	
	pair<State*, State*> newNFA;
	newNFA.first = new State(this->stateCounter++);
	newNFA.second = new State(this->stateCounter++);

	newNFA.first->addNextState(this->epsilon, oldNFA2.first);
	newNFA.first->addNextState(this->epsilon, oldNFA1.first);

	oldNFA1.second->addNextState(this->epsilon, newNFA.second);
	oldNFA2.second->addNextState(this->epsilon, newNFA.second);
	
	newNFA.first->setStartState(true);
	newNFA.second->setFinalState(true);
	states.push_back(newNFA.first);
	states.push_back(newNFA.second);

	NFA_Stack->push(newNFA);
	
	this->startState = newNFA.first;
	this->finalState = newNFA.second;
}

void NFA::solveConcatenation(stack< pair<State*, State*> > *NFA_Stack)
{
	pair<State*, State*> oldNFA1 = NFA_Stack->top();
	NFA_Stack->pop();
	oldNFA1.first->setStartState(false);
	oldNFA1.second->setFinalState(false);

	pair<State*, State*> oldNFA2 = NFA_Stack->top();
	NFA_Stack->pop();
	oldNFA2.first->setStartState(false);
	oldNFA2.second->setFinalState(false);

	char transitionChar = oldNFA1.first->getTransitionChar();
	oldNFA2.second->addNextState(transitionChar, oldNFA1.first->getNextTransitionStates(transitionChar));
	
	// Remove first state of first NFA because final state of second NFA is merged with first state of first NFA
	this->removeState(oldNFA1.first->getStateName());

	oldNFA2.first->setStartState(true);
	oldNFA1.second->setFinalState(true);
	NFA_Stack->push(make_pair(oldNFA2.first, oldNFA1.second));

	this->startState = oldNFA2.first;
	this->finalState = oldNFA1.second;
}

void NFA::solveClosure(stack< pair<State*, State*> > *NFA_Stack)
{
	pair<State*, State*> oldNFA = NFA_Stack->top();
	NFA_Stack->pop();
	oldNFA.first->setStartState(false);
	oldNFA.second->setFinalState(false);
	
	pair<State*, State*> newNFA;
	newNFA.first = new State(this->stateCounter++);
	newNFA.second = new State(this->stateCounter++);
	
	newNFA.first->addNextState(this->epsilon, oldNFA.first);
	newNFA.first->addNextState(this->epsilon, newNFA.second);
	oldNFA.second->addNextState(this->epsilon, oldNFA.first);
	oldNFA.second->addNextState(this->epsilon, newNFA.second);

	newNFA.first->setStartState(true);
	newNFA.second->setFinalState(true);
	states.push_back(newNFA.first);
	states.push_back(newNFA.second);

	NFA_Stack->push(newNFA);

	this->startState = newNFA.first;
	this->finalState = newNFA.second;
}

void NFA::solveCross(stack< pair<State*, State*> > *NFA_Stack)
{
	pair<State*, State*> oldNFA = NFA_Stack->top();
	NFA_Stack->pop();
	oldNFA.first->setStartState(false);
	oldNFA.second->setFinalState(false);

	pair<State*, State*> newNFA;
	newNFA.first = new State(this->stateCounter++);
	newNFA.second = new State(this->stateCounter++);

	newNFA.first->addNextState(this->epsilon, oldNFA.first);
	oldNFA.second->addNextState(this->epsilon, oldNFA.first);
	oldNFA.second->addNextState(this->epsilon, newNFA.second);

	newNFA.first->setStartState(true);
	newNFA.second->setFinalState(true);
	states.push_back(newNFA.first);
	states.push_back(newNFA.second);
	
	NFA_Stack->push(newNFA);

	this->startState = newNFA.first;
	this->finalState = newNFA.second;
}

State* NFA::getStateByName(string name)
{
	list<State*>::iterator it = states.begin();
	for(it; it != states.end(); it++)
	{
		if((*it)->getStateName() == name)
		{
			return (*it);
		}
	}
}

void NFA::removeState(string name)
{
	list<State*>::iterator it = states.begin();
	for(it; it != states.end(); it++)
	{
		if((*it)->getStateName() == name)
		{
			delete (*it);
			states.erase(it);
			return;
		}
	}
}

State* NFA::getStartState()
{
	return startState;
}

State* NFA::getFinalState()
{
	return finalState;
}

void NFA::printTransitionTable()
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
		if((*statesIt)->isStartState())
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
