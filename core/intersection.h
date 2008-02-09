//
// C++ Interface: intersection
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "vec3.h"
#include "primitive.h"


namespace Occluder {
class Triangle;

/**
Contains information about a intersection of a ray with a geometric primitiv.
It is able to represent the 'empty intersection' - meaning no intersection was found.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Intersection {
public:
    Intersection(const Vec3& position, float u, float v, float t, const Primitive& primitive);

    ~Intersection();
    Intersection& operator=(const Intersection& op);

    /**
      Calculates the radiance at this intersection into a given direction
      @param the direction for which radiance is to be calculated, pointing towards surface
      @param the calculated radiance
     **/
    Vec3 getRadiance(const Vec3& direction) const;
    /**
       @return wether this is a valid or the 'empty' intersection
     **/
    bool isEmpty() const;

    static const Intersection& getEmpty();

    bool operator<(const Intersection& op);

    /**
      @return the primtive the intersection occured with
     **/
    const Primitive& getPrimitive() const;

    /**
      @return the spacial loaction where the intersection occured
     **/
    const Vec3& getLocation() const;

    /**
      @return the surfacenormal of the primitive at the location where the 
              the intersection occured.
     **/
    const Vec3& getNormal() const;

private:
  const Vec3 position;
  const float u,
              v,
              t; /// corresponds to t im ray equation: R = origin t * direction
                 /// since only intersections in positive direction are interesting
                 /// t < 0.0 indicates the 'empty'- or no-intersection case
  const Primitive& primitive;

  static const Intersection empty; /// singleton instance for the empty hit
};


// ----------------------- implementations of inlined methods ------------------------------

inline Intersection& Intersection::operator=(const Intersection& op) {
  memcpy(this, &op, sizeof(Intersection));
  return *this;
}


inline bool Intersection::isEmpty() const {
  return (t < 0.0f);
}

inline const Intersection& Intersection::getEmpty() {
  return empty;
}

inline bool Intersection::operator<(const Intersection& op) {
  return (op.isEmpty() || ( t >= 0.0f ) && ( t < op.t ) );
}

inline const Primitive& Intersection::getPrimitive() const {
  return primitive;
}

inline const Vec3& Intersection::getLocation() const {
  return position;
}


}
#endif
