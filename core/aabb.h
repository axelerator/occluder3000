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
class Primitive;
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

    /**
      @param axis The axis along the minimum should be returned
      @return minimum value of the aabb along the given axis
     **/
    float getMin(unsigned int axis) const ;

    /**
      @param axis The axis along the maximum should be returned
      @return maximum value of the aabb along the given axis
     **/
    float getMax(unsigned int axis) const ;

    /**
      @return wether the given primitive overlaps this aabb
     **/
    bool intersects(const Primitive& prim) const;

    /**
      @return the minima of the AABB
     **/
    const Vec3& getMin() const;

    /**
      @return the maxima of the AABB
     **/
    const Vec3& getMax() const;

    /**
      @param f the box will be scaled with this factor
     **/
    void scale(float f);

    /**
      @return a vector that contains the width of the aabb in
              each dimension.
     **/
    const Vec3 getWidths() const;

    /**
       @param axis of the dimension in which the box is to be split
       @param splitPos position where the box is to be split
       @param left true if the half lesser then splitval is requested
       @return one half of this aabb
     **/
    AABB getHalfBox(unsigned char axis, float splitPos, bool left) const;
private:
  Vec3 minima;
  Vec3 maxima;

};

}
//  ------------- implementation of inlined methods ---------------
using namespace Occluder;

inline AABB::AABB(const Vec3& minima, const Vec3& maxima):
minima(minima), maxima(maxima) {}

inline AABB::AABB():
minima(UNENDLICH), maxima(-UNENDLICH)
{}

inline AABB::~AABB() {
}

inline float AABB::getMin(unsigned int axis) const  {
  return minima[axis];
}

inline float AABB::getMax(unsigned int axis) const {
  return maxima[axis];
}

inline const Vec3& AABB::getMin() const {
  return minima;
}

inline  const Vec3& AABB::getMax() const {
  return maxima;
}

inline void AABB::scale(float f) {
  maxima *= f;
  minima *= f;
}

inline const Vec3 AABB::getWidths() const {
  return Vec3(maxima[0] - minima[0], maxima[1] - minima[1], maxima[2] - minima[2]);
}
#endif
