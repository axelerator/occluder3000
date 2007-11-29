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
    void setDirection(const Vector3D& direction) { this->direction = direction;  invDirection.value[0] = 1.0/direction.value[0]; invDirection.value[1] =  1.0/direction.value[1]; invDirection.value[2] = 1.0/direction.value[2] ; hitSomething = false; hitDistance = UNENDLICH; }
    void setStart(const Vector3D& start) {  this->start = start;    }
    const Vector3D& getStart() const { return start; }
    const Vector3D& getDirection() const { return direction; }
    const Vector3D& getInvDirection() const { return invDirection; }
    const bool didHitSomething() const { return hitSomething; }
  private:

  Vector3D start;
  Vector3D direction;
  Vector3D invDirection;

  // hit info
  bool hitSomething;
  fliess hitDistance;
};

#endif
