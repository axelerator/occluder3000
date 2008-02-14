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
#include <iostream>
#include <assert.h>
#include "scene.h"
#include "raysegmentsse.h"

using namespace Occluder;

Vec3SSE IntersectionSSE::getRadiance(const RaySegmentSSE& ray, unsigned int depth) const {
  Vec3SSE res(0.0f);
  const int hitMaski = hitMask.getMask();
  if ( allSamePrimitive ) { 
    res = primitivePointers[0]->getShader().getRadiance(ray.getDirection(), *this, depth);
  } else if ( !isEmpty() ) {
//     return Vec3SSE(Vec3(0.5, 0.0, 0.3));
    for ( unsigned int r = 0; r < 4; ++r ) {
      if ( hitMaski & ( 1 << r ) ) {
        res.setVec(r, getSingleIntersection(r).getRadiance(ray.getDirection().get(r), depth));
      }
    }
  }
  return res;
}

/*inline*/ void IntersectionSSE::updateHits(const Float4& hitMask, const Float4& t, 
          const Float4& u, const Float4& v, unsigned int primIdx) {
  this->hitMask = this->hitMask | hitMask;
  const Float4 updateMask( hitMask & ( t < this->t ) );
  
  // update u,v,t for closer hits
  const Float4 noUpdateMask( updateMask.andnot(Float4::BINONE) );
  this->u = ( u & updateMask ) | ( this->u & noUpdateMask );
  this->v = ( v & updateMask ) | ( this->v & noUpdateMask );
  this->t = ( t & updateMask ) | ( this->t & noUpdateMask );

  Float4 prim4(primIdx);
  this->primitive = ( prim4 & updateMask ) | ( this->primitive & noUpdateMask );
  const int maski = updateMask.getMask();
  if ( maski == 15 )
    allSamePrimitive = true;
  else if ( maski > 0) 
    allSamePrimitive = false;

}

void IntersectionSSE::resolvePrimitiveIds(const Scene& scene, const RaySegmentSSE& rays) {
  position = rays.getOrigin() + ( rays.getDirection() * t );
  if ( allSamePrimitive ) {
    primitivePointers[0] = 
    primitivePointers[1] = 
    primitivePointers[2] = 
    primitivePointers[3] = &( scene.getPrimitive( primitive.v.i[0] ) );
    normals = Vec3SSE( primitivePointers[0]->getNormal() );
  } else {
    const int maskI = hitMask.getMask();
    for ( unsigned int i = 0; i < 4; ++i) {
      if ( ( 1 << i ) & maskI )
      primitivePointers[i] = &( scene.getPrimitive( primitive.v.i[i] ) );
    } 
  }
}
