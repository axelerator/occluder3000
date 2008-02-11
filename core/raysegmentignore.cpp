//
// C++ Implementation: raysegmentignore
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "raysegmentignore.h"

using namespace Occluder;

RaySegmentIgnore::RaySegmentIgnore(const Vec3& origin, const Vec3& direction, const Primitive& ignore, float tmin, float tmax)
: RaySegment(origin, direction, tmin, tmax), 
  ignore(ignore) {
}


RaySegmentIgnore::~RaySegmentIgnore() {
}

