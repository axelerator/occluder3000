//
// C++ Implementation: primitivelist
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "primitivelist.h"
#include "scene.h"
#include "raysegmentsse.h"
#include "intersectionsse.h"
using namespace Occluder;

PrimitiveList::PrimitiveList( const Scene& scene )
 : AccelerationStructure(scene)
{
}


PrimitiveList::~PrimitiveList()
{
}


bool PrimitiveList::hasIntersection(const RaySegment& ray)  const{
  const size_t primCount = scene.getPrimitiveCount();
  for (size_t i = 0; i < primCount; ++i) {
    const Primitive& prim = scene.getPrimitive(i);
    if ( prim.intersects( ray ) ) 
      return true;
  }
return false;
}

bool PrimitiveList::hasIntersection(const RaySegmentIgnore& ray)  const{
  const size_t primCount = scene.getPrimitiveCount();
  for (size_t i = 0; i < primCount; ++i) {
    const Primitive& prim = scene.getPrimitive(i);
    if ( prim.intersects( ray ) ) 
      return true;
  }
return false;
}

const Intersection PrimitiveList::getFirstIntersection(const RaySegment& ray) const {
  Intersection closest(Intersection::getEmpty());
  const size_t primCount = scene.getPrimitiveCount();
  for (size_t i = 0; i < primCount; ++i) 
    closest += scene.getPrimitive( i ).getIntersection( ray );

  return closest;
}

void PrimitiveList::getAllIntersections(const RaySegment& ray, List< const Intersection >& results) const {
}

void PrimitiveList::determineFirstIntersection(const RaySegmentSSE& ray, IntersectionSSE& result) const  {
  const size_t primCount = scene.getPrimitiveCount();
  for (size_t i = 0; i < primCount; ++i) {
    const Primitive& prim = scene.getPrimitive( i );
    prim.intersect( ray, result );
  }
}

Float4 PrimitiveList:: haveIntersections(const RaySegmentSSE& ray) const {
  const size_t primCount = scene.getPrimitiveCount();
  Float4 result(Float4::zero());
  for (size_t i = 0; i < primCount; ++i) {
    const Primitive& prim = scene.getPrimitive( i );
    result |= prim.intersect( ray );
    if ( result == 15 )
      return result;
  }
 return result;
}
