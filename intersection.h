//
// C++ Interface: intersection
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef INTERSECTION_H
#define INTERSECTION_H
#include "triangle.h"
#include "vector3d.h"
/**
	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Intersection {
  public:
    
   Intersection(const Triangle *tri, const Vector3D& intersectionPoint, const Vector3D& rayStart): 
    triangle(tri), intersectionPoint(intersectionPoint), intersectionDistance((intersectionPoint - rayStart).length()) {};
   Intersection() : triangle(0), intersectionPoint(0.0, 0.0, 0.0), intersectionDistance(INFINITY){  }
   Intersection(const Intersection& i) : triangle(i.triangle), intersectionPoint(i.intersectionPoint), intersectionDistance(i.intersectionDistance){  }
   Intersection& operator=(const Intersection& i) {
                                                  memcpy(this, &i, sizeof(Intersection)); 
//                                                      triangle = i.triangle;
//                                                      intersectionPoint = i.intersectionPoint;
//                                                      intersectionDistance = i.intersectionDistance;
//                                                      u = i.u; v= i.v;
//                                                      e1 = i.e1; e2 = i.e2;
                                                     return (*this);}
    bool operator<(const Intersection& i) const {return i.triangle == 0 || i.intersectionDistance > this->intersectionDistance; }

    const Triangle *triangle;
    Vector3D intersectionPoint;
    float intersectionDistance;
    float u,v;
    Vector3D e1, e2;
};

#endif
