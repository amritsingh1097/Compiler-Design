#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include "list"
using namespace std;

class SymbolTable
{
	list<string> symbolName, symbolAttribute, symbolValue;
	public:
		SymbolTable();
		~SymbolTable();
};

#endif
