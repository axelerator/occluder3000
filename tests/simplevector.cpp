#include <iostream>
#include "../simplevector.h"

#define asrt(expr) if (!expr) { \
                        std::cout << " -- Test in line " << __FILE__ << ":" << __LINE__ << " failed." << std::endl;\
                        ++failed;\
                        return false;\
                     } else\
                        ++succeed;

unsigned int failed = 0, succeed = 0, testcount = 0, testsucc = 0;

void execute( bool (*f)()) {
  if ( f() ) {
    ++testsucc; 
    std::cout << "Test:" << ++testcount << " successfully completed." << std::endl;
  }
  else
    std::cout << ++testcount << " failed." << std::endl;
}

bool test() {
  SimpleVector<unsigned int> tv;
  for ( int i = 0; i < 10; ++i)
    tv.getNextFree() = i;
  asrt(tv.size() == 10);
  return true;
}

bool test1() {
  SimpleVector<unsigned int> tv(20);
  for ( int i = 0; i < 300; ++i)
    tv.getNextFree() = i;
  asrt(tv.size() == 300);
  asrt(tv.get(0) == 0);
  asrt(tv.get(159) == 159);
  asrt(tv.get(200) == 200);
  return true;
}

bool test2() {
  SimpleVector<unsigned int> tv(4);
  unsigned int *ui = 0;
  int c = 0;
  for ( unsigned int i = 0; i < 5; ++i) {
    ui = tv.getNextFreePair();
    ui[0] = c++;
    ui[1] = c++;
  }
  asrt(tv.get(0) == 0);
  asrt(tv.get(9) == 9);

  return true;
}

bool test3() {
  SimpleVector<unsigned int> tv(4);
  unsigned int *ui = 0;
  int c = 0;
  tv.getNextFree() = c++;
  for ( unsigned int i = 0; i < 5; ++i) {
    ui = tv.getNextFreePair();
    ui[0] = c++;
    ui[1] = c++;
  }
  asrt(tv.get(0) == 0);
  asrt(tv.get(9) == 9);

  return true;
}

int main ( int argc, char *argv[] ) {

int result = 0;

execute(test);
execute(test1);
execute(test2);
execute(test3);
std::cout << "\n\n === Result ==\nTotal tests: " << testcount <<"\nFailed: " << testcount - testsucc << "\nSuceeded:" << testsucc << std::endl;
}
