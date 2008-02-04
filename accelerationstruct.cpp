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
bool AccelerationStruct::trimRaytoBounds(Ray &r) const {

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
bool AccelerationStruct::trimRaytoBounds(RayPacket &raypacket) const {

  if ( !trimRaytoBounds(raypacket.shaft))
    return false;

  for (unsigned int p = 0 ; p < raypacket.getR4Count() ; ++p ){
    trimRaytoBounds(raypacket.r4[p]);
  }

  return true;
}

/**
  @return false if all 4 rays miss the bounds
**/
bool AccelerationStruct::trimRaytoBounds(Ray4 &r4) const {

  __m128 t0 = _mm_setzero_ps();
  __m128 t1 = _mm_set1_ps(UNENDLICH);
  __m128 tmin, tmax, tymin, tymax, tzmin, tzmax;

  __m128 sortedBounds[6];
  
//   assert (
//        ((r4.direction.c[0].v.f[0] < 0) == (r4.direction.c[0].v.f[3] < 0))
//     && ((r4.direction.c[1].v.f[0] < 0) == (r4.direction.c[1].v.f[3] < 0))
//     && ((r4.direction.c[2].v.f[0] < 0) == (r4.direction.c[2].v.f[3] < 0))
//   );
  
  if ( r4.direction.c[0].v.f[0] > 0.0f ) {
    sortedBounds[0] = _mm_set1_ps(bounds[0]);
    sortedBounds[1] = _mm_set1_ps(bounds[1]);
  } else {
    sortedBounds[0] = _mm_set1_ps(bounds[1]);
    sortedBounds[1] = _mm_set1_ps(bounds[0]);
  }

  if ( r4.direction.c[1].v.f[0] > 0.0f ) {
    sortedBounds[2] = _mm_set1_ps(bounds[2]);
    sortedBounds[3] = _mm_set1_ps(bounds[3]);
  } else {
    sortedBounds[2] = _mm_set1_ps(bounds[3]);
    sortedBounds[3] = _mm_set1_ps(bounds[2]);
  }
  
  if ( r4.direction.c[2].v.f[0] > 0.0f ) {
    sortedBounds[4] = _mm_set1_ps(bounds[4]);
    sortedBounds[5] = _mm_set1_ps(bounds[5]);
  } else {
    sortedBounds[4] = _mm_set1_ps(bounds[5]);
    sortedBounds[5] = _mm_set1_ps(bounds[4]);
  }

  tmin =  _mm_mul_ps( _mm_sub_ps( sortedBounds[0], r4.origin.c[0].v.sse ), r4.inv_direction.c[0].v.sse);
  tmax =  _mm_mul_ps( _mm_sub_ps( sortedBounds[1], r4.origin.c[0].v.sse ), r4.inv_direction.c[0].v.sse);
  tymin = _mm_mul_ps( _mm_sub_ps( sortedBounds[2], r4.origin.c[1].v.sse ), r4.inv_direction.c[1].v.sse);
  tymax = _mm_mul_ps( _mm_sub_ps( sortedBounds[3], r4.origin.c[1].v.sse ), r4.inv_direction.c[1].v.sse);

  __m128 hitMask = _mm_and_ps(_mm_cmpgt_ps(tymax, tmin), _mm_cmpgt_ps(tmax, tymin));
  if ( !_mm_movemask_ps(hitMask)) 
    return false;

  tmin = _mm_max_ps(tmin, tymin);
  tmax = _mm_min_ps(tmax, tymax);

  tzmin =  _mm_mul_ps( _mm_sub_ps( sortedBounds[4], r4.origin.c[2].v.sse ), r4.inv_direction.c[2].v.sse);
  tzmax =  _mm_mul_ps( _mm_sub_ps( sortedBounds[5], r4.origin.c[2].v.sse ), r4.inv_direction.c[2].v.sse);

  hitMask =_mm_and_ps(hitMask, _mm_and_ps(_mm_cmpgt_ps(tzmax, tmin), _mm_cmpgt_ps(tmax, tzmin)));
  if ( !_mm_movemask_ps(hitMask)) 
    return false;

  tmin = _mm_max_ps(tmin, tzmin);
  tmax = _mm_min_ps(tmax, tzmax);

  hitMask =_mm_and_ps(hitMask, _mm_and_ps(_mm_cmplt_ps(tmin, t1), _mm_cmpgt_ps(tmax, t0)));
  if ( ! ( _mm_movemask_ps(hitMask))) 
    return false;

  r4.tmin.v.sse = tmin;
  r4.tmax.v.sse = tmax;
  return true;
}


