//
// C++ Interface: simplevector
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SIMPLEVECTOR_H
#define SIMPLEVECTOR_H
#include <cstdlib>
#include <iostream>
#include "assert.h"
/**
	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
template <class T>
class SimpleVector {
  public:
    SimpleVector(unsigned int initSize = 10) : reserved(initSize), idxInChunk(0), chunkCount(1), occupied(0), activeChunk(0) {
      init(initSize);
    }
    ~SimpleVector() {
      freeAll();
    }
    void clear(unsigned int initSize = 10) {
      freeAll();
      reserved = initSize;
      idxInChunk = 0;
      chunkCount = 1;
      occupied = 0;
      activeChunk = 0;
      init(initSize);
    }
    T& get ( unsigned int i ) const { 
      assert ( i < occupied ); 
      unsigned int c = 0;
      while (c < (chunkCount-1) && i >= chunks[c+1].offset)
        ++c;
      return chunks[c].data[i - chunks[c].offset] ; 
       
    }
    T& getNextFree() {
      if ( occupied == reserved ) {
        ++chunkCount;
        chunks = (Chunk*) realloc(chunks, chunkCount * sizeof(Chunk) );
        chunks[chunkCount - 1].data = (T*) malloc( reserved * sizeof(T) );
        chunks[chunkCount - 1].offset = reserved;
        activeChunk = chunks + chunkCount - 1;
        reserved *= 2;
        idxInChunk = 0;
      }
        ++occupied;
      return activeChunk->data[idxInChunk++];
    }
    
    T* getNextFreePair() {
      if ( (occupied + 1) >= reserved ) {
        ++chunkCount;
        chunks = (Chunk*) realloc(chunks, chunkCount * sizeof(Chunk) );
        chunks[chunkCount - 1].data = (T*) malloc( reserved * sizeof(T) );
        chunks[chunkCount - 1].offset = occupied;
        activeChunk = chunks + chunkCount - 1;
        reserved *= 2;
        idxInChunk = 0;
      }
      occupied += 2;
      idxInChunk += 2;
      return activeChunk->data + idxInChunk - 2;
    }    
    T& operator [] (unsigned int i)  { return get[i]; }
    
    unsigned int size() const {assert( occupied == activeChunk->offset + idxInChunk); return occupied;}
  private:
    void freeAll() {
      for (unsigned int i = 0; i < chunkCount; ++i)
        free(chunks[i].data);
      free(chunks);    
    }
    
    void init(int unsigned initSize) {
      assert(initSize > 0);
      chunks = (Chunk*) malloc( chunkCount * sizeof(Chunk) );
      chunks[0].offset = 0;
      chunks[0].data = (T*) malloc( initSize * sizeof(T) );
      activeChunk = chunks;     
    }
  
    unsigned int reserved;
    unsigned int idxInChunk;
    unsigned int chunkCount;
    unsigned int occupied;

    typedef struct {
      unsigned int offset;
      T* data;
    } Chunk;

    Chunk *activeChunk;// The chunk the last element was added to
    Chunk *chunks;
};

/**
  Holds max 256 elements of T.
  caches size.
**/
template <class T>
class SmallStaticArray {
  public:
    SmallStaticArray(unsigned char size):size(size){
      data = (T*) malloc( size * sizeof(T) );
    }
    ~SmallStaticArray() {
      free(data);
    }
  unsigned char size;
  T* data;
};

#endif
