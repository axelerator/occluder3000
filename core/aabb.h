//
// C++ Interface: aabb
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AABB_H
#define AABB_H

#include "vec3.h"


namespace Occluder {
/**
An axis-aligned bounding box contains info the minimaand maxima of the contained primitiv.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class AABB {
public:
    AABB(const Vec3& mininma, const Vec3& maxinma);
    AABB();
    ~AABB();
    /**
      refits (if necessary) the AABB in the way that the given vertex
      will be contained in it.
     **/
    void update(const Vec3& v);

private:
  Vec3 minima;
  Vec3 maxima;

};
}
//  ------------- implementation of inlined methods ---------------
using namespace Occluder;

inline AABB::AABB(const Vec3& mininma, const Vec3& maxinma):
minima(minima), maxima(maxima) {}

inline AABB::AABB():
minima(-UNENDLICH), maxima(UNENDLICH)
{}

inline AABB::~AABB() {
}

#endif
