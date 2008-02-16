//
// C++ Interface: raysegment
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RAYSEGMENT_H
#define RAYSEGMENT_H

#include "aabb.h"
namespace Occluder {
/**
Represents a distinct part of a ray.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class RaySegment {
public:
    RaySegment(const Vec3& origin, const Vec3& direction, float tmin = 0.0, float tmax = INFINITY);
    ~RaySegment();

    /**
        The ray will be trimmed to the minima/maxima of the given
        axis aligned bounding box.
        @param The aabb the raysegment will fit in after calling
        @return false if the ray misses the box completely
    **/
    bool trim(const AABB& aabb);

    const Vec3& getDirection() const ;
    const Vec3& getInvDirection() const;
    const Vec3& getOrigin() const ;
    float getTMin() const;
    float getTMax() const;

    void setTMin(float newmin);
    void setTMax(float newmax);
    void setOrigin(const Vec3& newOrigin);

  private:
    Vec3 origin;
    Vec3 direction;
    Vec3 invDirection;
    float tmin,tmax;

};

// ---------------------- implementations of inlined methodes ------------------------

inline const Vec3& RaySegment::getDirection() const {
  return direction;
}

inline const Vec3& RaySegment::getOrigin() const {
  return origin;
}

inline float RaySegment::getTMin() const {
  return tmin;
}

inline float RaySegment::getTMax() const {
  return tmax;
}

inline void RaySegment::setOrigin(const Vec3& newOrigin) {
  origin = newOrigin;
}

inline void RaySegment::setTMin(float newmin) {
  tmin = newmin;
}

inline void RaySegment::setTMax(float newmax) {
  tmax = newmax;
}

inline const Vec3& RaySegment::getInvDirection() const {
  return invDirection;
}

}
#endif
