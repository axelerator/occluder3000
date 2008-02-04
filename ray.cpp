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

Ray::Ray(const Vector3D& s, const Vector3D& d, float tmax, float tmin, const Triangle* tri) :
start(s), direction(d), invDirection(1.0/d.value[0], 1.0/d.value[1], 1.0/d.value[2]), tMax(tmax), tMin(tmin), 
ignore(tri){}

Ray::~Ray()
{}


Vector3D Ray::refractRay(const Vector3D& e, const Vector3D& n, double nFrom, double nTo) {
  float ne = e * n;
  float reflection = 1.0 - pow((nFrom/nTo), 2.0) * (1.0 - pow(ne, 2.0));

  if (reflection < 0.0) {
          Vector3D vpar( ne * n );
          Vector3D reflDir(e - ( 2 * vpar ));
          reflDir.normalize();  
          return reflDir;
  }

  return Vector3D(((e - (n * ne)) * (nFrom/nTo) - (n * sqrt(reflection))).normal());
}
