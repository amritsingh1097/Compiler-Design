#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <iomanip>
#include "list"
using namespace std;

struct SymbolAttributes
{
	string symbolName, symbolType;
	int symbolSize;
};

class SymbolTable
{
	list< pair<string, string> > DataTypeSizes;
	list<struct SymbolAttributes> Symbol_Table;
	
	// Functions
	short getSize(string dataType);
	public:
		void addSymbol(string symbolName, string symbolType);
		void printSymbolTable();
};

#endif
