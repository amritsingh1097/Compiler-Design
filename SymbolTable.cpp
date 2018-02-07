#include "symboltable.h"

SymbolTable::SymbolTable()
{
	this->symbolName = 0;
	this->symbolType = 1;
	this->symbolAttribute = 2;
	this->symbolValue = 3;
}

SymbolTable::~SymbolTable()
{

}

void SymbolTable::updateTable(string symbolName, string symbolType)
{
	
}
