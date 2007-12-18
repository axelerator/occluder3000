//
// C++ Implementation: triangle
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "triangle.h"
#include "ray.h"
#include "radianceray.h"
Triangle::Triangle(const Vector3D& v1, const Vector3D& v2, const Vector3D& v3):
 u(v2-v1), v(v3-v1), nu(0.0, 0.0, 0.0), nv(0.0, 0.0, 0.0), center(v1 + 0.33*u + 0.33*v) {
  p[0] = v1;
  p[1] = v2;
  p[2] = v3;
  Vector3D normal((u%v).normal());
  n[0] = normal;
  n[1] = normal;
  n[2] = normal;  
}

Triangle::Triangle(const Vector3D& v1, const Vector3D& v2, const Vector3D& v3, const Vector3D& vn1, const Vector3D& vn2, const Vector3D& vn3):
 u(v2-v1), v(v3-v1), nu(vn2-vn1), nv(vn3-vn1), center(v1 + 0.33*u + 0.33*v) {
  p[0] = v1;
  p[1] = v2;
  p[2] = v3;
  n[0] = vn1;
  n[1] = vn2;
  n[2] = vn3;
  n[0].normalize();
  n[1].normalize();
  n[2].normalize();
}

#define CROSS(dest,v1,v2) \
          dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
          dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
          dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
#define SUB(dest,v1,v2) \
          dest[0]=v1[0]-v2[0]; \
          dest[1]=v1[1]-v2[1]; \
          dest[2]=v1[2]-v2[2]; 

// #define TEST_CULL 1

bool Triangle::intersect(const Ray& r) const {
float t,u,v;

   float inv_det;

   Vector3D pvec(r.getDirection() % this->v);

   float det = this->u * pvec;

   if (det > -EPSILON && det < EPSILON)
     return 0;
   inv_det = 1.0 / det;

   Vector3D tvec = r.getStart() - p[0].value;

   u = (tvec * pvec) * inv_det;
   if (u < 0.0 || u > 1.0)
     return 0;

   Vector3D qvec = tvec % this->u;
   v = (r.getDirection() * qvec) * inv_det;
   if (v < 0.0 || u + v > 1.0)
     return 0;

   t = (this->v * qvec) * inv_det;
   return ( t > r.getMin() && t < r.getMax() );

}

bool Triangle::intersect(RadianceRay& r) const {
float t,u,v;

   float inv_det;

   Vector3D pvec(r.getDirection() % this->v);

   float det = this->u * pvec;

   if (det > -EPSILON && det < EPSILON)
     return 0;
   inv_det = 1.0 / det;

   Vector3D tvec = r.getStart() - p[0].value;

   u = (tvec * pvec) * inv_det;
   if (u < 0.0 || u > 1.0)
     return 0;

   Vector3D qvec = tvec % this->u;
   v = (r.getDirection() * qvec) * inv_det;
   if (v < 0.0 || u + v > 1.0)
     return 0;

   t = (this->v * qvec) * inv_det;
   if ( t <= r.getMin() || t > r.getMax() )
    return false;

  Intersection current(this, this->p[0] + (u * this->u) + (v * this->v), r.getStart());
  if ( current < r.getClosestIntersection() ) {
    current.e1 = this->u;
    current.e2 = this->v;
    current.u = u;
    current.v = v;
    r.setClosestIntersection(current);
  }
  return true;
}

Vector3D Triangle::getNormalAt(const IntersectionResult& ir) const {
  return Vector3D(n[0] + (nu * ir.u) + (nv * ir.v));
}

Vector3D Triangle::getNormalAt(const Intersection& ir) const {
  return Vector3D(n[0] + (nu * ir.u) + (nv * ir.v));
}

const Vector3D& Triangle::getPoint(unsigned int i) const{
  return p[i];
}

Triangle::~Triangle()
{}

