#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include "tokenizer.h"
#include "nfa.h"
#include "dfa.h"
using namespace std;

const string sample_regex[] = {	"(a|b)*",
	  		 					"(a|b)*abb",
								"(a|b)*a(a|b)*a(a|b)*",
								"b*ab*ab*|b*ab*",
								"(ab|a)*(aa|b)",
								"(a*b|b*a)*a",
								"a*|(ab|a)*",
								"(a*ab|ba)*.a*",
								"(ab|a*)*b",
								"(a|b)*abb(a|b)*" };

void runTokenizer()
{
	Tokenizer *tokenizer = new Tokenizer();

	ifstream input_program;
	input_program.open("./Source Programs/addition.cpp");
	tokenizer->tokenize(&input_program);

	delete tokenizer;
	input_program.close();
}

void runGenerateNFA()
{
	NFA *nfa = new NFA();
	nfa->generateEpsilonNFA(sample_regex[1]);
	cout << endl << "-----------------------" << endl;
	cout << "\tNFA" << endl;
	cout << "-----------------------" << endl;
	nfa->printTransitionTable();
	
	DFA *dfa = new DFA(nfa->getInputChars());
	cout << endl << "-----------------------" << endl;
	cout << "\tDFA" << endl;
	cout << "-----------------------" << endl;
	dfa->convertIntoDFA(nfa);
}

int main()
{
	// To run the tokenizer, uncomment the following line.
	//runTokenizer();
	
	// To generate NFA for regular expression, uncomment following line.
	//runGenerateNFA();
	cout << "\n\nPress key to exit...";
	getch();
	return 0;
}

