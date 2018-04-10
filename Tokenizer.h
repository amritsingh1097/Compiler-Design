#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <iostream>
#include <fstream>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <iomanip>
#include "list"
#include "symboltable.h"
using namespace std;

class Tokenizer
{
	ifstream *comparisonFiles;
	string *fileNames;
	int fileCount;
	char wild_char;
	list<char> input_program_optimised;
	int numIdenifiers, numConstants, numStrings, numCharacters, numSpecialSymbols, numOperators, numKeywords, numSpecialKeywords;
	SymbolTable *symbolTable;
	public:
		Tokenizer();
		~Tokenizer();
		void tokenize(ifstream *IP_file);
		void optimizeFile(ifstream *IP_file);
		inline char getWildChar();
		bool isDataType(string temp);
		bool compareStrings(string temp, ifstream *comparison_file, string fileName);
		void printOptimizedFile();
};

#endif
