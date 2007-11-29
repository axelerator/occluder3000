//
// C++ Interface: radianceray
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RADIANCERAY_H
#define RADIANCERAY_H
#include "vector3d.h"
// #include "triangle.h"
#include "intersection.h"


/**
A ray that gathers radiance when cast into a scene.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class RadianceRay {
  public:
    RadianceRay(const Vector3D& s, const Vector3D& d):start(s), direction(d), invDirection(1.0/d.value[0], 1.0/d.value[1], 1.0/d.value[2]){};

    ~RadianceRay();
    void setDirection(const Vector3D& direction) { this->direction = direction;  invDirection.value[0] = 1.0/direction.value[0]; invDirection.value[1] =  1.0/direction.value[1]; invDirection.value[2] = 1.0/direction.value[2]; }
    void setStart(const Vector3D& start) {  this->start = start;    }
    const Vector3D& getStart() const { return start; }
    const Vector3D& getDirection() const { return direction; }
    const Vector3D& getInvDirection() const { return invDirection; }
    const bool didHitSomething() const { return closestIntersection.triangle != 0 ; }
    Intersection& getClosestIntersection()  { return closestIntersection; }
    void setClosestIntersection(const Intersection& newInters) { closestIntersection = newInters; }
    
    
  private:
    Vector3D start;
    Vector3D direction;
    Vector3D invDirection;
  
    // hit info
    Intersection closestIntersection;
};

#endif
