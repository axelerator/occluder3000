//
// C++ Implementation: aabb
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "aabb.h"
using namespace Occluder;

void AABB::update(const Vec3& v) {
  for (size_t c = 0; c < 3; ++c) {
    minima[c] = (v[c] < minima[c]) ? v[c] : minima[c] ;
    maxima[c] = (v[c] > maxima[c]) ? v[c] : maxima[c];
  }
}
