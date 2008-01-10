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
#include "raypacket.h"

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
  r.setMin(fmaxf(tmin, r.getMin()));
  r.setMax(fminf(tmax, r.getMax()));
  return true;
}

/**
  All rays in packet are expected to have directions with
  same sign in all components
  @return false if whole packet misses scene
**/
bool AccelerationStruct::trimRaytoBounds(RayPacket &raypacket) {

  float t0 = 0.0;
  float t1 = UNENDLICH;
  float tmin, tmax, tymin, tymax, tzmin, tzmax;
  int sign[3]; // for all rays in packet the same
  sign[0] = ( raypacket.getInvDirection(0).value[0] < 0 );
  sign[1] = ( raypacket.getInvDirection(0).value[1] < 0 );
  sign[2] = ( raypacket.getInvDirection(0).value[2] < 0 );
  Vector3D parameters[2] = {Vector3D ( bounds[0], bounds[2], bounds[4] ),
                            Vector3D ( bounds[1], bounds[3], bounds[5] ) };
  const Vector3D& origin = raypacket.getOrigin();
  unsigned int anyHit = 0;
  for ( unsigned int i = 0; i < raypacket.getRayCount(); ++i) {
    const Vector3D &inv_direction = raypacket.getInvDirection(i);
    
    tmin = ( parameters[sign[0]].value[0] - origin.value[0] ) * inv_direction.value[0];
    tmax = ( parameters[1-sign[0]].value[0] - origin.value[0] ) * inv_direction.value[0];
    tymin = ( parameters[sign[1]].value[1] - origin.value[1] ) * inv_direction.value[1];
    tymax = ( parameters[1-sign[1]].value[1] - origin.value[1] ) * inv_direction.value[1];
    if ( ( tmin > tymax ) || ( tymin > tmax ) ) {
      raypacket.setMiss(i, true);
      continue;
    }
    if ( tymin > tmin )
      tmin = tymin;
    if ( tymax < tmax )
      tmax = tymax;
    tzmin = ( parameters[sign[2]].value[2] - origin.value[2] ) * inv_direction.value[2];
    tzmax = ( parameters[1-sign[2]].value[2] - origin.value[2] ) * inv_direction.value[2];
    if ( ( tmin > tzmax ) || ( tzmin > tmax ) ) {
      raypacket.setMiss(i, true);
      continue;
    }
    if ( tzmin > tmin )
      tmin = tzmin;
    if ( tzmax < tmax )
      tmax = tzmax;
    if ( ( tmin >= t1 ) || ( tmax <= t0 ) )  {
      raypacket.setMiss(i, true);
      continue;
    }
    raypacket.setMin(i, tmin);
    raypacket.setMax(i, tmax);
    raypacket.setMiss(i, false);
    ++anyHit;
  }
  return anyHit;
}
