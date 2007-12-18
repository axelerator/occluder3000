//
// C++ Implementation: accelerationstruct
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "accelerationstruct.h"


AccelerationStruct::AccelerationStruct(const Scene& scene) : scene ( scene )  {}


AccelerationStruct::~AccelerationStruct() {}

void AccelerationStruct::setBounds(float* newBounds) {
  memcpy(this->bounds, newBounds, 6 * sizeof(float));
  this->boundsSet = true;
}

void AccelerationStruct::addTriangle ( const Triangle& t ) {
    triangles.push_back ( t );
}


/**
* @return false if ray misses scene
* Ray-box intersection using IEEE numerical properties to ensure that the
* test is both robust and efficient, as described in:
*
*      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
*      "An Efficient and Robust Ray-Box Intersection Algorithm"
*      Journal of graphics tools, 10(1):49-54, 2005
*
*      * slightly altered to find point of intersection *
*/
bool AccelerationStruct::trimRaytoBounds(Ray &r) {

  float t0 = 0.0;
  float t1 = UNENDLICH;
  float tmin, tmax, tymin, tymax, tzmin, tzmax;
  Vector3D parameters[2] = {Vector3D ( bounds[0], bounds[2], bounds[4] ),
                            Vector3D ( bounds[1], bounds[3], bounds[5] ) };
  const Vector3D &inv_direction = r.getInvDirection();
  int sign[3];
  sign[0] = ( inv_direction.value[0] < 0 );
  sign[1] = ( inv_direction.value[1] < 0 );
  sign[2] = ( inv_direction.value[2] < 0 );

  tmin = ( parameters[sign[0]].value[0] - r.getStart().value[0] ) * inv_direction.value[0];
  tmax = ( parameters[1-sign[0]].value[0] - r.getStart().value[0] ) * inv_direction.value[0];
  tymin = ( parameters[sign[1]].value[1] - r.getStart().value[1] ) * inv_direction.value[1];
  tymax = ( parameters[1-sign[1]].value[1] - r.getStart().value[1] ) * inv_direction.value[1];
  if ( ( tmin > tymax ) || ( tymin > tmax ) ) {
    return false;
  }
  if ( tymin > tmin )
    tmin = tymin;
  if ( tymax < tmax )
    tmax = tymax;
  tzmin = ( parameters[sign[2]].value[2] - r.getStart().value[2] ) * inv_direction.value[2];
  tzmax = ( parameters[1-sign[2]].value[2] - r.getStart().value[2] ) * inv_direction.value[2];
  if ( ( tmin > tzmax ) || ( tzmin > tmax ) ) {
    return false;
  }
  if ( tzmin > tmin )
    tmin = tzmin;
  if ( tzmax < tmax )
    tmax = tzmax;
  if ( ( tmin >= t1 ) || ( tmax <= t0 ) ) 
    return false;
  r.setMin(fmaxf(tmin, 0.0));
  r.setMax(tmax);
  return true;
}

