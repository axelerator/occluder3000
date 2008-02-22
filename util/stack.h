#ifndef OCCLUDERSTACK_H
#define OCCLUDERSTACK_H
#include <assert.h>


#define MSIZE 32
namespace Occluder {

template <class T>
class Stack {
  public:
  Stack(unsigned int maxDepth = 32):
    maxDepth(maxDepth),
    currentSize(0) {
    stack = (T*) malloc ( maxDepth * sizeof(T) );
  }

  ~Stack() {
    free(stack);
  }

  void push(const T& t) {
    assert(currentSize < maxDepth );
    stack[currentSize] = t;
    ++currentSize;
  }

  bool isEmpty() {
    return currentSize == 0;
  }

  T pop() {
    assert( !isEmpty() );
    currentSize--;
    return stack[currentSize];
  }

  void clear() {
    currentSize = 0;
  }

  private:
    const unsigned int maxDepth;
    unsigned int currentSize;
    T *stack;
};

}
#endif
