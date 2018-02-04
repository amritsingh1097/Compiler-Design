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
	list<char> input_program_optimised;
	int numIdenifiers, numConstants, numStrings, numSpecialSymbols, numOperators, numKeywords, numSpecialKeywords;
	public:
		Tokenizer();
		~Tokenizer();
		void tokenize(ifstream *IP_file);
		void optimizeFile(ifstream *IP_file);
		bool compareStrings(string temp, ifstream *comparison_file, string fileName);
		void printOptimizedFile();
};

#endif
