//
// C++ Implementation: raysegment
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "raysegment.h"
using namespace Occluder;

RaySegment::RaySegment(const Vec3& origin, const Vec3& direction, float tmin, float tmax):
origin(origin), direction(direction), tmin(tmin), tmax(tmax) {}


RaySegment::~RaySegment() {
}

bool RaySegment::trim(const AABB& aabb) {
  return false;
}
