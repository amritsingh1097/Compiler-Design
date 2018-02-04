#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include "tokenizer.h"
using namespace std;

int main()
{
	Tokenizer *tokenizer = new Tokenizer();

	ifstream input_program;
	input_program.open("./Source Programs/addition.cpp");
	tokenizer->tokenize(&input_program);

	tokenizer->~Tokenizer();
	input_program.close();
	getch();
	return 0;
}

