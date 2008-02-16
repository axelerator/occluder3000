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
origin(origin), direction(direction), invDirection( 1.0f / direction[0], 1.0f / direction[1], 1.0f / direction[2]), tmin(tmin), tmax(tmax) {}


RaySegment::~RaySegment() {
}

bool RaySegment::trim(const AABB& aabb) {
  float t0 = 0.0;
  float t1 = UNENDLICH;
  float tmin, tmax, tymin, tymax, tzmin, tzmax;
  Vec3 parameters[2] = {aabb.getMin(), aabb.getMax() };
  int sign[3];
  sign[0] = ( invDirection[0] < 0 );
  sign[1] = ( invDirection[1] < 0 );
  sign[2] = ( invDirection[2] < 0 );

  tmin = ( parameters[sign[0]][0] - this->getOrigin()[0] ) * invDirection[0];
  tmax = ( parameters[1-sign[0]][0] - this->getOrigin()[0] ) * invDirection[0];
  tymin = ( parameters[sign[1]][1] - this->getOrigin()[1] ) * invDirection[1];
  tymax = ( parameters[1-sign[1]][1] - this->getOrigin()[1] ) * invDirection[1];
  if ( ( tmin > tymax ) || ( tymin > tmax ) ) {
    return false;
  }
  if ( tymin > tmin )
    tmin = tymin;
  if ( tymax < tmax )
    tmax = tymax;
  tzmin = ( parameters[sign[2]][2] - this->getOrigin()[2] ) * invDirection[2];
  tzmax = ( parameters[1-sign[2]][2] - this->getOrigin()[2] ) * invDirection[2];
  if ( ( tmin > tzmax ) || ( tzmin > tmax ) ) {
    return false;
  }
  if ( tzmin > tmin )
    tmin = tzmin;
  if ( tzmax < tmax )
    tmax = tzmax;
  if ( ( tmin >= t1 ) || ( tmax <= t0 ) ) 
    return false;
  this->setTMin(fmaxf(tmin, this->getTMin()));
  this->setTMax(fminf(tmax, this->getTMax()));
  return true;
}
