#include "tokenizer.h"

Tokenizer::Tokenizer(void)
{
	this->numConstants = 0;
	this->numStrings = 0;
	this->numIdenifiers = 0;
	this->numKeywords = 0;
	this->numOperators = 0;
	this->numSpecialSymbols = 0;
	this->numSpecialKeywords = 0;
	this->fileCount = 4;
	this->fileNames = new string[fileCount];
	this->comparisonFiles = new ifstream[fileCount];
	this->fileNames[0] = "OPERATOR";
	this->fileNames[1] = "KEYWORD";
	this->fileNames[2] = "SPECIAL SYMBOL";
	this->fileNames[3] = "SPECIAL KEYWORD";
	this->comparisonFiles[0].open("operators.txt");
	this->comparisonFiles[1].open("keywords.txt");
	this->comparisonFiles[2].open("special_symbols.txt");
	this->comparisonFiles[3].open("special_keywords.txt");
}

Tokenizer::~Tokenizer(void)
{
	cout << endl << endl;
	cout << "========================" << endl;
	cout << left << setfill('-') << setw(20) << "Constants " << right << ": " << numConstants << endl;
	cout << left << setw(20) << "Strings " << right << ": " << numStrings << endl;
	cout << left << setw(20) << "Idenifiers " << right << ": " << numIdenifiers << endl;
	cout << left << setw(20) << "Keywords " << right << ": " << numKeywords << endl;
	cout << left << setw(20) << "Operators " << right << ": " << numOperators << endl;
	cout << left << setw(20) << "Special Symbols " << right << ": " << numSpecialSymbols << endl;
	cout << left << setw(20) << "Special Keywords " << right << ": " << numSpecialKeywords << endl;
	cout << "========================" << endl;
	cout << "Closing Files..." << endl;
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

void Tokenizer::optimizeFile(ifstream *IP_file)
{
	char ch, last_char;

	while(!IP_file->eof())
	{
		IP_file->get(ch);
//		if(((last_char >= 'a' && last_char <= 'z') || (last_char >= 'A' && last_char <= 'Z'))
//		   && (!(ch >= 'a' && ch <= 'z') && !(ch >= 'A' && ch <= 'Z'))
//		   && !(ch >= '0' && ch <= '9')
//		   && ch != ' ')
//		{
//			input_program_optimised.push_back(' ');
//		}
		if(ch != '\t' && ch != '\n')
		{
			input_program_optimised.push_back(ch);
			last_char = ch;
		}
	}
	
	// Print Optimized file
	printOptimizedFile();
}

void Tokenizer::tokenize(ifstream *IP_file)
{
	// Remove Tabs and NewLine Characters
	optimizeFile(IP_file);
	cout << endl << endl;
	
	string temp;
	bool matched = false, constant = true, stringStart = false, IP_File_EOF = false;
	char ch;
	
	list<char>::iterator it = input_program_optimised.begin();
	
	cout << "-------------------------------------" << endl << "TOKEN RECOGNITION:" << endl << "-------------------------------------" << endl;
	// Reading optimized input program from array one character at a time
	while(it != input_program_optimised.end())
	{
		ch = *it;
//		cout << ch;
		
		// Check the start of the string
		if(ch == '"' && !stringStart)
		{
			stringStart = true;
			temp.clear();
		}
		
		// For string detection, e.g., in cout
		else if(ch != '"' && stringStart)
		{
			while((*it) != '"')
			{
//				cout << "INSIDE WHILE..." << endl;
				temp.append(sizeof(*it), *it);
				// For escape characters
				if((*it) == '\\')// && (*(++it)) == '"')
				{
//					temp.append(sizeof(*it), *it);
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
			if(IP_File_EOF)
			{
				break;
			}
			stringStart = false;
			temp.clear();
			temp = ch = *it;
		}
		
//		cout << "CH: " << ch << "\nTEMP: " << temp << endl;

		// Storing the string
		if(((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')
		   || (((temp[0] >= 'a' && temp[0] <= 'z') || (temp[0] >= 'A' && temp[0] <= 'Z') || temp[0] == '_') && ((ch >= '0' && ch <= '9') || (ch == '_')))
		   || (ch >= '0' && ch <= '9'))
		{
			temp.append(sizeof(ch), ch);
		}
		else
		{
//			cout << "TEMP: " << temp << endl;
			// Means a single Special Symbol or operator
			if(!temp.length() && ch != ' ')
			{
				temp = ch;
			}

			if(temp.length())
			{
//				cout << "CH: " << ch << endl;
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
								this->numKeywords++;
								break;
							case 2:
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
//					cout << "CH: " << ch << endl;
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
//		cout << endl << endl << "IT++ END: " << *it << endl;
//		getch();
	}
}

bool Tokenizer::compareStrings(string temp, ifstream *comparison_file, string fileName)
{
	string file_string;
	
	while(!comparison_file->eof())
	{
		*comparison_file >> file_string;
		if(temp == file_string)
		{
//			cout << *temp << " IS RECOGNISED AS " << fileName << endl << endl;
			comparison_file->seekg(0, ios::beg);
			return true;
		}
	}

	comparison_file->seekg(0, ios::beg);
	return false;
}

void Tokenizer::printOptimizedFile()
{
	list<char>::iterator it = input_program_optimised.begin();
	cout << "OPTIMIZED PROGRAM:" << endl << "-------------------------------------" << endl;
	while(it != input_program_optimised.end())
	{
		cout << *it;
		it++;
	}
}
