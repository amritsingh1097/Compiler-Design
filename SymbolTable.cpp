#include "symboltable.h"

short SymbolTable::getSize(string dataType)
{
	if(dataType == "bool")			return sizeof(bool);
	if(dataType == "char")			return sizeof(char);
	if(dataType == "double")		return sizeof(double);
	if(dataType == "float")			return sizeof(float);
	if(dataType == "int")			return sizeof(int);
	if(dataType == "long")			return sizeof(long);
	if(dataType == "short")			return sizeof(short);
	if(dataType == "unsigned")		return sizeof(unsigned);
}

void SymbolTable::addSymbol(string symbolName, string symbolType)
{
	struct SymbolAttributes symbolAttrib;
	symbolAttrib.symbolName = symbolName;
	symbolAttrib.symbolType = symbolType;
	symbolAttrib.symbolSize = getSize(symbolType);
	this->Symbol_Table.push_back(symbolAttrib);
}

void SymbolTable::printSymbolTable()
{
	list<struct SymbolAttributes>::iterator it = Symbol_Table.begin();
	cout << endl << "-------------------------------------" << endl << "SYMBOL TABLE:" << endl << "-------------------------------------" << endl;
	cout << setw(20) << right << "Symbol Name" << "\t" << setw(20) << left << "Symbol Type" << setw(20) << left << "Symbol Size" << endl;
	for(it; it != Symbol_Table.end(); it++)
	{
		cout << setw(20) << right << (*it).symbolName << "\t" << setw(20) << left << (*it).symbolType << setw(20) << left << (*it).symbolSize << endl;
	}
}
