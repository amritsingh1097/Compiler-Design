#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

class SymbolTable
{
	int symbolName, symbolType, symbolAttribute, symbolValue;
	string **Symbol_Table;
	public:
		SymbolTable();
		~SymbolTable();
		void updateTable(string name, string type);
};

#endif
