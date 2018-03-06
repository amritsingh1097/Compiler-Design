#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include "tokenizer.h"
#include "nfa.h"
using namespace std;

string regex[] = {"(a|b)*",
				  "(a|b)*.a.(a|b)*.a.(a|b)*",
				  "b*.a.b*.a.b*|b*.a.b*",
				  "(a.b|a)*.(a.a|b)",
				  "(a*.b|b*.a)*.a",
				  "a*|(a.b|a)*",
				  "(a*.a.b|b.a)*.a*",
	   		   	  "(a.b|a*)*.b",
				  "(a|b)*abb(a|b)*"};

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
	int numInputChars = 2;
	NFA *nfa = new NFA(numInputChars, new char[2]{'a', 'b'});
	nfa->generateNFA(regex[0]);
	cout << endl << "TRANSITION TABLE:" << endl << endl;
	nfa->printTransitionTable();
}

int main()
{
	// To run the tokenizer program, uncomment the following line
//	runTokenizer();
	runGenerateNFA();
	getch();
	return 0;
}

