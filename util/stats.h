//
// C++ Interface: stats
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
#include <sys/time.h>

namespace Occluder {
class Timer {
  public:
    Timer() : start(gettime()) {}
    double elapsed() const {
      return gettime() - start;
    }
  
    static double gettime() {
      struct timeval t;
      gettimeofday ( &t,0 );
      return t.tv_sec+t.tv_usec/1000000.0;
    }
  private:
    const double start;

};

}

