#include "stack"
#include "tokenizer.h"

Tokenizer::Tokenizer(void)
{
	this->numConstants = 0;
	this->numStrings = 0;
	this->numCharacters = 0;
	this->numIdenifiers = 0;
	this->numKeywords = 0;
	this->numOperators = 0;
	this->numSpecialSymbols = 0;
	this->numSpecialKeywords = 0;
	this->wild_char = '@';
	this->fileCount = 4;
	this->fileNames = new string[fileCount];
	this->comparisonFiles = new ifstream[fileCount];
	this->fileNames[0] = "OPERATOR";
	this->fileNames[1] = "KEYWORD";
	this->fileNames[2] = "SPECIAL SYMBOL";
	this->fileNames[3] = "SPECIAL KEYWORD";
	this->comparisonFiles[0].open("./Comparison Files/operators.txt");
	this->comparisonFiles[1].open("./Comparison Files/keywords.txt");
	this->comparisonFiles[2].open("./Comparison Files/special_symbols.txt");
	this->comparisonFiles[3].open("./Comparison Files/special_keywords.txt");
	this->symbolTable = new SymbolTable();
}

Tokenizer::~Tokenizer(void)
{
	cout << endl << endl;
	cout << "==========================" << endl;
	cout << left << setfill('-') << setw(20) << "Constants " << right << ": " << numConstants << endl;
	cout << left << setw(20) << "Strings " << right << ": " << numStrings << endl;
	cout << left << setw(20) << "Characters " << right << ": " << numStrings << endl;
	cout << left << setw(20) << "Idenifiers " << right << ": " << numIdenifiers << endl;
	cout << left << setw(20) << "Keywords " << right << ": " << numKeywords << endl;
	cout << left << setw(20) << "Operators " << right << ": " << numOperators << endl;
	cout << left << setw(20) << "Special Symbols " << right << ": " << numSpecialSymbols << endl;
	cout << left << setw(20) << "Special Keywords " << right << ": " << numSpecialKeywords << endl;
	cout << "==========================" << endl;
	
	// Closing Files
	comparisonFiles[0].close();
	comparisonFiles[1].close();
	comparisonFiles[2].close();
	comparisonFiles[3].close();
	cout << "Files Closed." << endl;

	// Freeing up dynamically allocated memory
	delete [] comparisonFiles;
	delete [] fileNames;
	cout << "Memory Deallocated." << endl;
}

inline char Tokenizer::getWildChar()
{
	return this->wild_char;
}

bool Tokenizer::isDataType(string temp)
{
	if(temp == "bool" || temp == "char" || temp == "double" || temp == "float" || temp == "int" || temp == "long" || temp == "short" || temp == "unsigned")
	{
		return true;
	}
	return false;
}

void Tokenizer::optimizeFile(ifstream *IP_file)
{
	char ch, last_char;
	bool preprocessorStart = false;

	while(!IP_file->eof())
	{
		IP_file->get(ch);
		// For preprocessor directives - Preserve newline characters for preprocessor directives
		if(ch == '#')
		{
			preprocessorStart = true;
		}
		if(preprocessorStart)
		{
			input_program_optimised.push_back(ch);
			if(ch == '\n')
			{
				preprocessorStart = false;
			}
			continue;
		}
		// For detecting CHARACTERS so it should not conflict with strings, e.g. 'abc\'
		if(ch == '\'')
		{
			do
			{
				input_program_optimised.push_back(ch);
				last_char = ch;
				IP_file->get(ch);
			} while((ch != '\'' && ch != '\n') || (last_char == '\\' && ch == '\''));
			if(ch == '\n')
			{
				input_program_optimised.push_back(this->wild_char);
			}
		}
		
		// For detecting STRINGS so it should not conflict with strings, e.g. "SOME///STRING"
		if(ch == '"')
		{
			bool condition = true;
			input_program_optimised.push_back(ch);
			do
			{
				last_char = ch;
				IP_file->get(ch);
				if(last_char == '\n' && IP_file->eof())
				{
					condition = false;
				}
				else if(last_char == '\\' && ch == '\n')
				{
					input_program_optimised.pop_back();
					condition = true;
				}
				else if(last_char == ch && ch == '\n' && input_program_optimised.back() != this->wild_char)
				{
					input_program_optimised.push_back(this->wild_char);
					condition = false;
				}
				else if(ch == '\n' && last_char != '\\' && last_char != '\n')
				{
					cout << "CH: " << ch << "\tLAST_CHAR: " << last_char << endl;
					cout << "input_program_optimised.back():: " << input_program_optimised.back() << endl;
					input_program_optimised.push_back(this->wild_char);
					condition = false;
				}
				else if(ch == '"' && last_char != '\\')
				{
					condition = false;
				}
				else if(ch != '\t' && ch != '\n')
				{
					input_program_optimised.push_back(ch);
				}
			} while(condition);
		}
		
		// For removing comments
		if(ch == '/' && last_char != '/' && last_char != '*')
		{
			last_char = ch;
			continue;
		}
		else if(ch == '/' && last_char == '/')
		{
			do
			{
				IP_file->get(ch);
			} while(ch != '\n');
			last_char = '\0';
		}
		else if(ch == '*' && last_char == '/')
		{
			last_char = ch;
			IP_file->get(ch);
			do
			{
				last_char = ch;
				IP_file->get(ch);
			} while(ch != '/' && last_char != '*');
			IP_file->get(ch);
			last_char = '\0';
		}
		
		// For removing newline and tab characters
		if(ch != '\t' && ch != '\n')
		{
			input_program_optimised.push_back(ch);
			last_char = ch;
		}
	}
}

void Tokenizer::tokenize(ifstream *IP_file)
{
	// Remove Tabs and NewLine Characters
	optimizeFile(IP_file);
	
	string temp;
	bool matched = false, constant = true, stringStart = false, charStart = false, IP_File_EOF = false, preprocessorStart = false;
	char ch;
	
	bool isLastCharSemiColon;								// For Symbol Table Creation
	stack<string> Stack;									// For Symbol Table Creation
	
	list<char>::iterator it = input_program_optimised.begin();
	
	cout << "-------------------------------------" << endl << "TOKEN RECOGNITION:" << endl << "-------------------------------------" << endl;
	// Reading optimized input program from array one character at a time
	while(it != input_program_optimised.end())
	{
		ch = *it;

		// Check the start of the character
		if(ch == '\'' && !charStart && !preprocessorStart)
		{
			charStart = true;
			temp.clear();
		}
		// For character detection, e.g. ch = 'character'
		else if(ch != '\'' && charStart && !preprocessorStart)
		{
			while((*it) != '\'' && (*it) != this->wild_char)
			{
				temp.append(sizeof(*it), *it);
				// For escape characters - append char proceeding '\' to temp
				if((*it) == '\\')
				{
					it++;
					temp.append(sizeof(*it), *it);
				}
				it++;
			}
			cout << setw(20) << temp << "\t-->\tCHARACTER" << endl;
			this->numCharacters++;
			charStart = false;
			if((*it) == this->wild_char)
			{
				it++;
			}
			temp.clear();
			ch = *it;
		}
		
		// Check the start of the string
		if(ch == '"' && !stringStart && !preprocessorStart)
		{
			stringStart = true;
			temp.clear();
		}

		// For string detection, e.g., in cout
		else if(ch != '"' && stringStart && !preprocessorStart)
		{
			while((*it) != '"' && (*it) != this->wild_char)
			{
				temp.append(sizeof(*it), *it);
				// For escape characters
				if((*it) == '\\')
				{
					it++;
					temp.append(sizeof(*it), *it);
				}
				it++;
				if(it == input_program_optimised.end())
				{
					IP_File_EOF = true;
					break;
				}
			}
			cout << setw(20) << temp << "\t-->\tSTRING" << endl;
			this->numStrings++;
			if((*it) == this->wild_char)
			{
				it++;
			}
			if(IP_File_EOF || it == input_program_optimised.end())
			{
				IP_File_EOF = true;
				break;
			}
			stringStart = false;
			temp.clear();
			temp = ch = *it;
		}
		
		// For processing Preprocessor Directives
		char temp_char = *(--it);
		it++;
		if(preprocessorStart && (temp_char == '<' || temp_char == '"'))
		{
			char compareWithChar;
			if(temp_char == '<')
			{
				compareWithChar = '>';
			}
			else if(temp_char == '"')
			{
				compareWithChar = '"';
			}
			while(*it != compareWithChar)
			{
				temp.append(sizeof(*it), *it);
				it++;
				if(*it == '\n')
				{
					it++;
					break;
				}
			}
			if(compareStrings(temp, &comparisonFiles[3], fileNames[3]))
			{
				cout << setw(20) << temp << "\t-->\t" << fileNames[3] << endl;
			}
			else
			{
				cout << setw(20) << temp << "\t-->\tIDENTIFIER" << endl;
			}
			preprocessorStart = false;
			temp.clear();
			ch = *it;
		}
		
		// Storing the string
		if(((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' || ch == '$')
		   || (((temp[0] >= 'a' && temp[0] <= 'z') || (temp[0] >= 'A' && temp[0] <= 'Z') || temp[0] == '_') && ((ch >= '0' && ch <= '9') || (ch == '_') || (ch == '$')))
		   || (ch >= '0' && ch <= '9'))
		{
			temp.append(sizeof(ch), ch);
		}
		else
		{
			// Means a single Special Symbol or operator
			if(!temp.length() && ch != ' ' && ch != '\n' && ch != this->wild_char)
			{
				if(ch == '#')
				{
					preprocessorStart = true;
				}
				temp = ch;
			}

			if(temp.length())
			{
				for(int i=0; i<this->fileCount; i++)
				{
					matched = compareStrings(temp, &comparisonFiles[i], fileNames[i]);
					if(matched)
					{
						// Match Found with ith file
						switch(i)
						{
							case 0:
							{
								// For checking compound operators
								while(true)
								{
									it++;
									temp.append(sizeof(*it), *it);
									if(!compareStrings(temp, &comparisonFiles[0], fileNames[0]))
									{
										temp = temp.substr(0, temp.size()-1);
										it--;
										break;
									}
								}
								this->numOperators++;
								break;
							}
							case 1:
								if(isDataType(temp))											// For Symbol Table Creation
								{																// For Symbol Table Creation
									Stack.push(temp);											// For Symbol Table Creation
								}																// For Symbol Table Creation
								this->numKeywords++;
								break;
							case 2:
								if(temp == ";")													// For Symbol Table Creation
								{																// For Symbol Table Creation
									if(Stack.size())
										Stack.pop();
									isLastCharSemiColon = true;
								}
								else if(temp == "{")
								{
									if(Stack.size())
										Stack.pop();
									isLastCharSemiColon = false;
								}
								else { isLastCharSemiColon = false; }
								this->numSpecialSymbols++;
								break;
							case 3:
								this->numSpecialKeywords++;
								break;
						}
						cout << setw(20) << temp << "\t-->\t" << fileNames[i] << endl;
						break;
					}
				}
				// Check for identifying constants
				constant = true;
				for(int i=0; i<temp.length(); i++)
				{
					if(temp[i] < '0' || temp[i] > '9' || temp[i] == '.')
					{
						constant = false;
						break;
					}
				}
				if(constant)
				{
					// Constant
					this->numConstants++;
					cout << setw(20) << temp << "\t-->\tCONSTANT" << endl;
				}
				else if(!matched && !constant)
				{
					// Ientifier
					if(!Stack.empty())									// For Symbol Table Creation
					{													// For Symbol Table Creation
						symbolTable->addSymbol(temp, Stack.top());		// For Symbol Table Creation
					}													// For Symbol Table Creation
					this->numIdenifiers++;
					cout << setw(20) << temp << "\t-->\tIDENTIFIER" << endl;
				}
				
				// If there is an operator after any string of alphabets or digits or underscores(_)
				if(((temp[temp.length()-1] >= 'a' && temp[temp.length()-1] <= 'z')
				    || (temp[temp.length()-1] >= 'A' && temp[temp.length()-1] <= 'Z')
					|| (temp[temp.length()-1] >= '0' && temp[temp.length()-1] <= '9')
					|| (temp[temp.length()-1] == '_'))
				   && (!(ch >= 'a' && ch <= 'z') && !(ch >= 'A' && ch <= 'Z'))
				   && !(ch >= '0' && ch <= '9')
				   && ch != ' ')
				{
					it--;
				}
				temp.clear();
			}
		}
		it++;
	}
	symbolTable->printSymbolTable();
}

bool Tokenizer::compareStrings(string temp, ifstream *comparison_file, string fileName)
{
	string file_string;
	
	while(!comparison_file->eof())
	{
		*comparison_file >> file_string;
		if(temp == file_string)
		{
			comparison_file->seekg(0, ios::beg);
			return true;
		}
	}

	comparison_file->seekg(0, ios::beg);
	return false;
}

void Tokenizer::printOptimizedFile()
{
	int ch = 1;
	list<char>::iterator it = input_program_optimised.begin();
	cout << "OPTIMIZED PROGRAM:" << endl << "-------------------------------------" << endl;
	while(it != input_program_optimised.end())
	{
		cout << *it;
		it++;
	}
}
