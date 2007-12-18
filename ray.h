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
    Ray(const Vector3D& s, const Vector3D& d, float tmax = UNENDLICH, float tmin = 0.0f);
    ~Ray();
    inline void setDirection(const Vector3D& direction) { this->direction = direction;  invDirection.value[0] = 1.0f/direction.value[0]; invDirection.value[1] =  1.0f/direction.value[1]; invDirection.value[2] = 1.0f/direction.value[2];}
    inline void setStart(const Vector3D& start) {  this->start = start;    }
    inline const Vector3D& getStart() const { return start; }
    inline const Vector3D& getDirection() const { return direction; }
    inline const Vector3D& getInvDirection() const { return invDirection; }
    void setMax(float max) { tMax = max; }
    float getMax() const { return tMax; }    
    void setMin(float min) { tMin = min; }
    float getMin() const { return tMin; }  
  protected:

  Vector3D start;
  Vector3D direction;
  Vector3D invDirection;
  float tMax;
  float tMin;
};

#endif
