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
#include "accelerationstruct.h"
#include "intersection.h"
#include "rgbvalue.h"
#include "scene.h"
#include "ray.h"
/**
A ray that gathers radiance when cast into a scene.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class RadianceRay : public Ray{
  public:
    RadianceRay(const Scene& scene):scene(scene){}
    RadianceRay(const Vector3D& s, const Vector3D& d,const Scene& scene, float tmax = UNENDLICH, float tmin = 0.0f):Ray(s, d, tmax, tmin), scene(scene)  {}

    ~RadianceRay();
    const bool didHitSomething() const { return closestIntersection.triangle != 0 ; }
    Intersection& getClosestIntersection()  { return closestIntersection; }
    void setClosestIntersection(const Intersection& newInters) { closestIntersection = newInters; }
    void shade(RGBvalue& result, unsigned int depth);
  private:


    // hit info
    Intersection closestIntersection;
    const Scene &scene;

};

#endif
