#include "hash.h"
#include <vector>
#include <string>

// The constructor initializes the hash table.
// Uses getPrime to choose a prime number at least as large as
// the specified size for the initial size of the hash table.
hashTable::hashTable(int size) 
  : data( getPrime(size) ), capacity( getPrime(size) ), filled(0)
{
  for( int i = 0; i < data.size(); i++)
  {
    data[i].isOccupied = false;
    data[i].isDeleted  = false;
  }
}

// Insert the specified key into the hash table.
// If an optional pointer is provided,
// associate that pointer with the key.
// Returns 0 on success,
// 1 if key already exists in hash table,
// 2 if rehash fails.
int hashTable::insert(const std::string &key, void *pv)
{
  int currentPos = hashTable::findPos(key);
  if( data[currentPos].isOccupied )
    return 1;
  
  hashTable::hashItem x;
  x.key = key;
  x.isOccupied = true;
//   x.isDeleted = false; // I'm not sure if this should be set when doing an insert.
  x.pv = pv;
  data[currentPos] = x;
  
  // If the current amount of elements stored in the hash table
  // is greater than or equal to half the table size, rehash all the elements
  // into a hash table that is double in capacity.
  if( ++filled >= data.size()/2 )
  {
    bool success = rehash();
    if (success) return 0;
    else return 2;
  }
  else return 0;
}

// Check if the specified key is in the hash table.
// If so, return true; otherwise, return false.
bool hashTable::contains(const std::string &key)
{
  return( data[findPos(key)].isOccupied );
}

// The hash function.
int hashTable::hash(const std::string &key)
{
  int hashVal = 0;
  for( int i = 0; i < key.length(); i++)
    hashVal = 37*hashVal + key[i];
    
  hashVal %= data.size();
  if(hashVal < 0)
    hashVal += data.size();
  
  return hashVal;
}

// Get the size of the table.
int hashTable::getSize() {return capacity;}

// Find position in array where key will be stored. 
// Linear Probing is used as a collisions resolution strategy.
int hashTable::findPos(const std::string &key)
{
  int currentPos = hashTable::hash( key );
  
  while( (data[currentPos].isOccupied == true || data[currentPos].isDeleted == true) && 
	  data[currentPos].key != key)
  {
    currentPos++;
    if( currentPos >= data.size() )
      currentPos = currentPos - data.size();
  }
  return currentPos;
}

// The rehash function; makes the hash table bigger.
// Returns true on success, false if memory allocation fails.
bool hashTable::rehash()
{
  std::vector<hashItem> oldData = data;
  
  // Create new double-sized empty table
  data.resize( getPrime( 2* oldData.size() ) );
  for(int j = 0; j < data.size(); j++)
  {
    data[j].isOccupied = false;
    data[j].isDeleted  = false;
  }
  hashTable::capacity = data.size();
  
  // Copy over old table.
  hashTable::filled = 0;
  for( int i = 0; i < oldData.size(); i++ )
  {
    if( oldData[i].isOccupied )
    {
      insert( oldData[i].key );
      hashTable::filled++;
    }
  }
    
  return 1;
}

// Return a prime number at least as large as size.
// Uses a precomputed sequence of selected prime numbers.
unsigned int hashTable::getPrime(int size)
{
  int primes[] = {100003, 203971, 408011, 817049, 1634819, 3271277};
  int i = 0;
  while(true)
  {
    if( primes[i] >= size) return primes[i];
    else i++;
  }
}
