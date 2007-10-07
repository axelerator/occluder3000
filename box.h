#ifndef _BOX_H_
#define _BOX_H_

#include "vector3d.h"
#include "ray.h"

class BoxRayIntersect {
  public:
  BoxRayIntersect(Vector3D poi) : hit(true), poi(poi) {}
  BoxRayIntersect() : hit(false),poi(0.0) {}
  bool hit;
  Vector3D poi;
  
};


/*
 * Axis-aligned bounding box class, for use with the optimized ray-box
 * intersection test described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 */

class Box {
  public:
    Box() { }
    Box(const Vector3D &min, const Vector3D &max) {
      parameters[0] = min;
      parameters[1] = max;
    }
    // (t0, t1) is the interval for valid hits
    void intersect(const Ray &, float t0, float t1, BoxRayIntersect& result) const;

    // corners
    Vector3D parameters[2];
};

#endif // _BOX_H_
