//
// C++ Implementation: stats
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "stats.h"


Stats *Stats::singleton = 0;

Stats::Stats():frame(1) {
}


Stats::~Stats() {
}

Stats& Stats::getInstance() {
  if ( !singleton )
    singleton = new Stats();
  return *singleton;
}

void Stats::incr(const std::string& key) {
  ++(currFrameInt[key]);
}

void Stats::set(const std::string& key, const double value) {
  currFrameDouble[key] = value;
}

void Stats::nextFrame() {
  std::map<std::string, unsigned int>::iterator iter = currFrameInt.begin();
  for (; iter != currFrameInt.end(); ++iter) {
    std::pair<const std::string, unsigned int> &currVal = *iter;
    avg[currVal.first] += (currVal.second - avg[currVal.first]) / frame;
    currVal.second = 0;
  }

  std::map<std::string, double>::iterator iterf = currFrameDouble.begin();
  for (; iterf != currFrameDouble.end(); ++iterf) {
    std::pair<const std::string, double> &currVal = *iterf;
    avg[currVal.first] += (currVal.second - avg[currVal.first]) / frame;
    currVal.second = 0;
  }
  ++frame;
}

std::ostream& operator << (std::ostream& os, const Stats& s) {
  
  os << "Stats:\n";
  std::map<std::string, double>::const_iterator iter = s.avg.begin();
  for (; iter != s.avg.end(); ++iter) {
    os << "\t" << (*iter).first << ":\t\t\t" <<  (*iter).second << "\n";
  }
  return os;
}



