#include "vector3d.h"
#include "ray.h"
#include "box.h"

/*
 * Ray-box intersection using IEEE numerical properties to ensure that the
 * test is both robust and efficient, as described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 *      * slightly altered to find poin of intersection *
 */
void Box::intersect(const Ray &r, float t0, float t1,BoxRayIntersect& result ) const {
  float tmin, tmax, tymin, tymax, tzmin, tzmax;
  const Vector3D& inv_direction = r.getInvDirection();
  int sign[3];
  sign[0] = (inv_direction.value[0] < 0);
  sign[1] = (inv_direction.value[1] < 0);
  sign[2] = (inv_direction.value[2] < 0);

  tmin = (parameters[sign[0]].value[0] - r.getStart().value[0]) * inv_direction.value[0];
  tmax = (parameters[1-sign[0]].value[0] - r.getStart().value[0]) * inv_direction.value[0];
  tymin = (parameters[sign[1]].value[1] - r.getStart().value[1]) * inv_direction.value[1];
  tymax = (parameters[1-sign[1]].value[1] - r.getStart().value[1]) * inv_direction.value[1];
  if ( (tmin > tymax) || (tymin > tmax) ) {
    result.hit = false;
    return;
  }
  if (tymin > tmin)
    tmin = tymin;
  if (tymax < tmax)
    tmax = tymax;
  tzmin = (parameters[sign[2]].value[2] - r.getStart().value[2]) * inv_direction.value[2];
  tzmax = (parameters[1-sign[2]].value[2] - r.getStart().value[2]) * inv_direction.value[2];
  if ( (tmin > tzmax) || (tzmin > tmax) ) { 
    result.hit = false;
    return;
  }
  if (tzmin > tmin)
    tmin = tzmin;
  if (tzmax < tmax)
    tmax = tzmax;
  if ( (tmin < t1) && (tmax > t0) ) {
    result.hit = true;
    result.poi = r.getStart() + (tmin * r.getDirection());
  } else    
    result.hit = false;
}
