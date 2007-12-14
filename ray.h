//
// C++ Interface: ray
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RAY_H
#define RAY_H
#include "vector3d.h"
/**
	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Ray
{
  public:
    Ray();
    Ray(const Vector3D& s, const Vector3D& d);
    ~Ray();
    inline void setDirection(const Vector3D& direction) { this->direction = direction;  invDirection.value[0] = 1.0/direction.value[0]; invDirection.value[1] =  1.0/direction.value[1]; invDirection.value[2] = 1.0/direction.value[2] ; hitSomething = false; hitDistance = UNENDLICH; }
    inline void setStart(const Vector3D& start) {  this->start = start;    }
    inline const Vector3D& getStart() const { return start; }
    inline const Vector3D& getDirection() const { return direction; }
    inline const Vector3D& getInvDirection() const { return invDirection; }
    inline const bool didHitSomething() const { return hitSomething; }
  private:

  Vector3D start;
  Vector3D direction;
  Vector3D invDirection;

  // hit info
  bool hitSomething;
  float hitDistance;
};

#endif
