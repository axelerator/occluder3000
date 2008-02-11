//
// C++ Interface: intersectionsse
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERINTERSECTIONSSE_H
#define OCCLUDERINTERSECTIONSSE_H
#include "sse4.h"

namespace Occluder {

/**
Stores four intersections at once in an SSE suitable SOA format.
In contrast to single intersections an IntersectionSSE is passed
to a primitive/ray intersection test and is potenially updated.
  @author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class IntersectionSSE {
public:
  IntersectionSSE();

  ~IntersectionSSE(){};
  void updateMask(int newHits);

  /**
    Updates u,t,v for the hits where t < this->t for each ray
    @param hitMask mask indicating which rays hit at all
    @param t parametric distance to the hit
    @param u surface coordinate
    @param v surface coordinate
   **/
  void updateHits(const Float4& hitMask, const Float4& t, const Float4& u, const Float4& v);
  const Float4 getHitMask() const ;
  /**
    Calculates the radiance at the points of intersection into the given directions.
    @param directions for which radiance is to be calculated, pointing towards surface
    @param depth steps left until recursion will abort
    @return the calculated radiance
    **/  
  Vec3SSE getRadiance(const Vec3SSE& directions, unsigned int depth) const;
private:

   Vec3SSE position;
   Float4 u,
          v,
          t; /// corresponds to t im ray equation: R = origin t * direction
              /// since only intersections in positive direction are interesting
              /// t < 0.0 indicates the 'empty'- or no-intersection case
//   const Primitive& primitive;
  /**
    indicates which rays hit a primitive
   **/
  Float4 hitMask; 
};

// --------------------- implementation of inlined methods ----------------------

inline IntersectionSSE::IntersectionSSE() :
  t(0.0), hitMask(0.0) {
  
}

inline void IntersectionSSE::updateHits(const Float4& hitMask, const Float4& t, const Float4& u, const Float4& v) {
  const Float4 updateMask( hitMask & ( t < this->t ) );
  this->hitMask = this->hitMask | hitMask;
  
  // update u,v,t for closer hits
  const Float4 noUpdateMask( updateMask.andnot(Float4::BINONE) );
  this->u = ( u | updateMask ) & ( this->u | noUpdateMask );
  this->v = ( v | updateMask ) & ( this->v | noUpdateMask );
  this->t = ( t | updateMask ) & ( this->t | noUpdateMask );
}

inline const Float4 IntersectionSSE::getHitMask() const {
  return hitMask;
}

}

#endif
