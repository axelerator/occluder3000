//
// C++ Interface: raysegmentsse
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERRAYSEGMENTSSE_H
#define OCCLUDERRAYSEGMENTSSE_H
#include "sse4.h"

namespace Occluder {
class AABB;
/**
Represents a quadruple of rays. Data is stored in an Structure-of-Arrays manner, suitable for processing with SSE instructions.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class RaySegmentSSE{
public:
    /**
      @param origin common origin fo all rays
      @param d0 direction of the first ray
      @param u offset of the second rays direction to the first ones
      @param v offset of the second rays direction to the third ones
     **/ 
    RaySegmentSSE(const Vec3& origin, const Vec3SSE& directions, const Float4 tmax = Float4(UNENDLICH));
    const Vec3SSE& getOrigin() const;
    const Vec3SSE& getDirection() const;
    const Float4& getTMin() const;
    const Float4& getTMax() const;
    /**
        The rays will be trimmed to the minima/maxima of the given
        axis aligned bounding box.
        @param The aabb the raysegment will fit in after calling
        @return false if all ray miss the box completely
    **/
    bool trim(const AABB& aabb);

    ~RaySegmentSSE() {}
private:
    /**
      Stores the origin of the rays. Is further assumed
      to be for all four rays the same.
     **/
    const Vec3SSE origin;

    /**
      Contains directions for all four rays in the set.
     **/
    const Vec3SSE direction;

    Float4 tmin;
    Float4 tmax;

};


// ----------------------------- implementation of inlined methods -------------------------
inline RaySegmentSSE::RaySegmentSSE(const Vec3& origin, const Vec3SSE& directions, const Float4 tmax):
  origin(origin), 
  direction(directions),
  tmin(0.0f),
  tmax(tmax) {
}


inline const Vec3SSE& RaySegmentSSE::getOrigin() const {
  return origin;
}

inline const Vec3SSE& RaySegmentSSE::getDirection() const {
  return direction;
}

inline const Float4& RaySegmentSSE::getTMin() const {
  return tmin;
}

inline const Float4& RaySegmentSSE::getTMax() const {
  return tmax;
}

}

#endif
