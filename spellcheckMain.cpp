/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Author: Ariel Habshush
 
Project Name: DSA II- Programming Assignment 1 - SpellChecking

Project Description:
The objective of this program is to implement a spell checker using a hash table class to store a dictionry.

Date: Sept. 24, 2013
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include "hash.h"

using namespace std;


// Load the words from a specified input file dictionary.
// Parameters:
//	ht--a hashTable object where dictionary words will be stored.
//	validChar--a logical array whose elements are true, if their 
//		   corresponding ASCII character is a valid word character.
void loadDictionary(hashTable &ht, std::vector<bool> validChar);

// Perform a spellchek on a user-specified input file.
// Write errors to an output file.
void spellCheck(hashTable &dict, std::vector<bool> validChar);

// Create a logical array that stores valid characters. Valid characters are letters, 
// digits, dashes, and apostrophes. These correspond to Ascii values: 39, 45, 48-57, 65-90, and 97-122.
// The array is indexed by the character's ASCII value.
void fillValidCharArray(std::vector<bool> &anArray);

int main()
{
  // Create a logical array containing valid word characters. 
  // It can store extended ASCII which has 256 characters.
  std::vector<bool> validCharArray(256, false);
  fillValidCharArray(validCharArray);
  
  // Load the Dictionary
  hashTable theDictionary;
  loadDictionary(theDictionary, validCharArray);
  
  // Spell Check the document.
  spellCheck(theDictionary, validCharArray);

  return 0;
}

void loadDictionary(hashTable &ht, std::vector<bool> validChar) {
  string filename;
  cout << "Enter name of dictionary: ";
  cin >> filename;
  
  // Keep track of how long it takes to read the dictionary.
  clock_t t1 = clock();

  ifstream input(filename.c_str());
  if (!input) 
  {
    cerr << "Error: could not open " << filename << endl;
    exit(1);
  }

  // Loop through each line of the dictionary and load each word into the hash table.
  while ( !input.eof() ) 
  {
    string word;
    getline(input, word);
    bool validWord = true;
    
    // If word is longer than 20 characters it is not valid, and don't store in hash table.
    // Otherwise, convert uppercase letters to lowercase and also check if word contains invalid characters.
    if ( word.size() > 20 )
      validWord = false;    
    else 
    {    
      std::locale loc;
      for (string::size_type i = 0; i < word.length(); ++i) 
      {
	word[i] = std::tolower(word[i], loc);
	if ( word[i] < 0 || !validChar[ word[i] ] )
	  validWord = false;
      }
    }
    
    // If the word is valid, store in the hash table.
    if (validWord)
    {
      int insertStatus = ht.insert(word);
      if(insertStatus == 2)
      {
	cerr << "Error: could not open " << filename << endl;
	exit(1);
      }
	
    }
  }

  input.close();
  
  clock_t t2 = clock();
  double timeDiff = ((double) (t2 - t1)) / CLOCKS_PER_SEC;

  cout << "Total time (in seconds) to load dictionary: " << timeDiff << endl;
}

void spellCheck(hashTable &dict, std::vector<bool> validChar) {
  // Prompt user for document to be spellchecked and the output file.
  string infilename;
  cout << "Enter name of input file: ";
  cin >> infilename;
  
  string outfilename;
  cout << "Enter name of output file: ";
  cin >> outfilename;
  
  ifstream input(infilename.c_str());
  if (!input) 
  {
    cerr << "Error: could not open " << infilename << endl;
    exit(1);
  }
  
  ofstream output(outfilename.c_str());
  if (!output) {
    cerr << "Error: could not open " << outfilename << endl;
    exit(1);
  }
  
 
  // Loop through each line of the document that is to be spelled checked.
  // Parse line into individual words by searching the line until reaching an invalid character.
  // Each word will be stored in a struct that contains 3 elements:
  //	1) The word itself.
  //	2) Information about the word (if it is valid, invalid, too long, or unknown).
  //	3) If the word contains a digit.
    
  enum wordStatus {VALID, INVALID, TOO_LONG, UNKNOWN};
  struct sWord
  {
    std::string word;
    wordStatus info;
    bool hasDigit;
    
    sWord( const string &str = string(), wordStatus sts = UNKNOWN, bool dig = false)
      : word(str), info(sts), hasDigit(dig) {}
  };
  
  // Loop through each line in file and keep track of how long it takes to spellcheck the document.
  int lineNumber = 0;
  clock_t t1 = clock();
  
  while ( !input.eof() ) 
  {
    lineNumber++;
    string line;
    getline(input, line);
    
    // Loop through each character in the current line:
    // Initialization
    string::size_type i = 0; // Keep track of the first character of the word.
    string::size_type j = 0; // Keep track of the last character of the word.
    std::locale loc;
    sWord Word;
    Word.hasDigit = false;
    
    // Start loop
    while( j < line.length() ) 
    {
      line[j] = std::tolower(line[j], loc);
      // If the current character is valid...
      if( (line[j] > 0) && (validChar[ line[j] ])  )
      {
	// Check if the current character is a digit.
	if ( !Word.hasDigit && (line[j] >= 48) && (line[j] <= 57) )
	  Word.hasDigit = true;
	
	// If we reached the end of the line, we look up the final word of the line.
	if( j >= line.length() - 1) 
	{
	  Word.word = line.substr(i, j-i+1);
	  // Check if word is not more than 20 characters long.
	  if( (Word.word).size() <= 20 )
	  {
	    // Words containing digits are automatically valid.
	    if (  Word.hasDigit )
	      Word.info = VALID;
	    else if( dict.contains(Word.word) )
	      Word.info = VALID;
	    else
	    {
	      Word.info = INVALID;
	      output << "Unknown word at line "<< lineNumber << ": " << Word.word << endl;
	    }
	  }
	  else
	  {
	    Word.info = TOO_LONG;
	    output << "Long word at line "<< lineNumber << ", starts: " << (Word.word).substr(0, 20) << endl;
	  }
	  Word.hasDigit = false;
	}
	
      }
      
      // If the current character is not valid, we reached a word delimeter.
      else 
      {
	// Test condition for cases that have multiple invalid characters in a row.
	if(i != j)
	{
	  Word.word = line.substr(i, j-i);
	  if( (Word.word).size() <= 20 )
	  {
	    // Words containing digits are automatically valid.
	    if (  Word.hasDigit  )
	      Word.info = VALID;
	    else if( dict.contains(Word.word) )
	      Word.info = VALID;
	    else
	    {
	      Word.info = INVALID;
	      output << "Unknown word at line "<< lineNumber << ": " << Word.word << endl;
	    }
	  }
	  else
	  {
	    Word.info = TOO_LONG;
	    output << "Long word at line "<< lineNumber << ", starts: " << (Word.word).substr(0, 20) << endl;
	  }
	  Word.hasDigit = false;
	  i = j+1; // Reset the start position of the next word.
	}
	else
	  i++;
      }
      j++;
    } // end looping through line characters.
  } // end looping through lines.
  
  output.close();
  input.close();
  
  clock_t t2 = clock();
  double timeDiff = ((double) (t2 - t1)) / CLOCKS_PER_SEC; 
  cout << "Total time (in seconds) to check document: " << timeDiff << endl;
}

void fillValidCharArray(std::vector<bool> &anArray) {
  anArray[39] = true; // single quotations
  anArray[45] = true; // hyphen (dash)
  for (int i = 48; i <= 57; i++) // digits 0-9
    anArray[i] = true;
  for (int i = 65; i <= 90; i++) // capital letters
    anArray[i] = true;
  for (int i = 97; i <= 122; i++) // lower letters.
    anArray[i] = true;
}
