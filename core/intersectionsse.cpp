//
// C++ Implementation: intersectionsse
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "intersectionsse.h"

using namespace Occluder;

Vec3SSE IntersectionSSE::getRadiance(const Vec3SSE& directions, unsigned int depth) const {
  Vec3SSE res(1.0f);
  res.c[0] = res.c[0] & hitMask; 
  res.c[1] = res.c[1] & hitMask; 
  res.c[2] = res.c[2] & hitMask; 
  return res;
}
