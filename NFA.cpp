#include "nfa.h"
#include "tokenizer.h"

NFA::NFA(int numInputChars, char *inputChars)
{
	this->originalRegex = "";
	this->postfixRegex = "";
	this->epsilon = '$';
	this->numInputChars = numInputChars+1;
	this->inputChars = new char[this->numInputChars];
	this->inputChars[0] = this->epsilon;
	for(int i=1; i<this->numInputChars; i++)
	{
		this->inputChars[i] = inputChars[i-1];
	}
	this->stateCounter = 0;
}

NFA::~NFA()
{
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


void NFA::generateNFA(string regex)
{
	if(!isRegexValid(regex))
	{
		return;
	}
	this->originalRegex = regex;
	cout << "Regular Expression: " << regex << endl;
	regex = transformRegex(regex);
	convertIntoPostfix(regex);

	stack< pair<State*, State*> > NFA_Stack;
	for(int i=0; i<postfixRegex.length(); i++)
	{
		if(!isOperator(postfixRegex[i]))
		{
			pair<State*, State*> newState;
			newState.first = new State(this->stateCounter++);
			newState.second = new State(this->stateCounter++);
			newState.first->addNextState(postfixRegex[i], newState.second);

			newState.first->setStartState(true);
			newState.second->setFinalState(true);
			this->startState = newState.first;
			this->finalState = newState.second;
			states.push_back(newState.first);
			states.push_back(newState.second);

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
	if(NFA_Stack.size() == 1)
	{
		NFA_Stack.pop();
	}
}

void NFA::solveUnion(stack< pair<State*, State*> > *NFA_Stack)
{
	pair<State*, State*> oldState1 = NFA_Stack->top();
	NFA_Stack->pop();
	oldState1.first->setStartState(false);
	oldState1.second->setFinalState(false);
	pair<State*, State*> oldState2 = NFA_Stack->top();
	NFA_Stack->pop();
	oldState2.first->setStartState(false);
	oldState2.second->setFinalState(false);
	
	pair<State*, State*> newState;
	newState.first = new State(this->stateCounter++);
	newState.second = new State(this->stateCounter++);

	newState.first->addNextState(this->epsilon, oldState1.first);
	newState.first->addNextState(this->epsilon, oldState2.first);

	oldState1.second->addNextState(this->epsilon, newState.second);
	oldState2.second->addNextState(this->epsilon, newState.second);
	
	newState.first->setStartState(true);
	newState.second->setFinalState(true);
	states.push_back(newState.first);
	states.push_back(newState.second);

	NFA_Stack->push(newState);
	
	this->startState = newState.first;
	this->finalState = newState.second;
}

void NFA::solveConcatenation(stack< pair<State*, State*> > *NFA_Stack)
{
	pair<State*, State*> oldState1 = NFA_Stack->top();
	NFA_Stack->pop();
	oldState1.first->setStartState(false);
	oldState1.second->setFinalState(false);
	pair<State*, State*> oldState2 = NFA_Stack->top();
	NFA_Stack->pop();
	oldState2.first->setStartState(false);
	oldState2.second->setFinalState(false);
	
	oldState2.second->addNextState(this->epsilon, oldState1.first);

	oldState2.first->setStartState(true);
	oldState1.second->setFinalState(true);
	NFA_Stack->push(make_pair(oldState2.first, oldState1.second));

	this->startState = oldState2.first;
	this->finalState = oldState1.second;
}

void NFA::solveClosure(stack< pair<State*, State*> > *NFA_Stack)
{
	pair<State*, State*> oldState = NFA_Stack->top();
	NFA_Stack->pop();
	oldState.first->setStartState(false);
	oldState.second->setFinalState(false);
	
	pair<State*, State*> newState;
	newState.first = new State(this->stateCounter++);
	newState.second = new State(this->stateCounter++);
	
	newState.first->addNextState(this->epsilon, oldState.first);
	newState.first->addNextState(this->epsilon, newState.second);
	oldState.second->addNextState(this->epsilon, newState.first);
	oldState.second->addNextState(this->epsilon, newState.second);

	newState.first->setStartState(true);
	newState.second->setFinalState(true);
	states.push_back(newState.first);
	states.push_back(newState.second);

	NFA_Stack->push(newState);

	this->startState = newState.first;
	this->finalState = newState.second;
}

void NFA::solveCross(stack< pair<State*, State*> > *NFA_Stack)
{
	pair<State*, State*> oldState = NFA_Stack->top();
	NFA_Stack->pop();
	oldState.first->setStartState(false);
	oldState.second->setFinalState(false);

	pair<State*, State*> newState;
	newState.first = new State(this->stateCounter++);
	newState.second = new State(this->stateCounter++);

	newState.first->addNextState(this->epsilon, oldState.first);
	oldState.second->addNextState(this->epsilon, newState.first);
	oldState.second->addNextState(this->epsilon, newState.second);

	newState.first->setStartState(true);
	newState.second->setFinalState(true);
	states.push_back(newState.first);
	states.push_back(newState.second);
	
	NFA_Stack->push(newState);

	this->startState = newState.first;
	this->finalState = newState.second;
}

void NFA::printTransitionTable()
{
	cout << setw(10) << left << "\tStates\\IP";
	for(int i=0; i<numInputChars; i++)
	{
		cout << "\t" << setw(10) << left << inputChars[i];
	}
	cout << endl;
	list<State*>::iterator it = states.begin();
	for(it; it != states.end(); it++)
	{
		if((*it)->isStartState())
		{
			cout << " -->";
		}
		else if((*it)->isFinalState())
		{
			cout << "  O";
		}
		cout << "\t" <<  setw(10) << left << (*it)->getStateName();
		for(int i=0; i<numInputChars; i++)
		{
			cout << "\t" << setw(10) << left << (*it)->getStateTransition(inputChars[i]);
		}
		cout << endl;
	}
}
