//
// C++ Implementation: ray
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ray.h"

Ray::Ray(){}

Ray::Ray(const Vector3D& s, const Vector3D& d, float tmax, float tmin) :
start(s), direction(d), invDirection(1.0/d.value[0], 1.0/d.value[1], 1.0/d.value[2]), tMax(tmax), tMin(tmin){}


Ray::~Ray()
{}


