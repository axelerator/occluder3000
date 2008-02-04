//
// C++ Interface: stats
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef STATS_H
#define STATS_H
#include <map>
#include <string>
#include <iostream>
#include <sstream>

#ifdef NDEBUG
  #define statsset(key,value) ;
  #define statsinc(key) ;
#else
  #define statsset(key,value) Stats::getInstance().set((key), (value))
  #define statsinc(key) Stats::getInstance().incr(key);
#endif

/**
Collects and reports several statistics about the rendering process.
Is a singleton.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Stats {
  friend std::ostream &operator<<(std::ostream &o, const Stats &r);
  public:
    static Stats& getInstance();
    void nextFrame();
    void incr(const std::string& key);
    void set(const std::string& key, const double value);
    double get(const std::string& key) { return avg[key]; }
private:
    Stats();
    ~Stats();
  std::map<std::string, unsigned int> currFrameInt;
  std::map<std::string, double> currFrameDouble;
  std::map<std::string, double> avg;
  
  unsigned int frame;
  static Stats *singleton;
};

std::ostream& operator<<(std::ostream& os, const Stats& s);
#endif
