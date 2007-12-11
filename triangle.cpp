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


int
intersect_triangle(const float orig[3], const float dir[3],
                   const float vert0[3], const float vert1[3], const float vert2[3],
                   float *t, float *u, float *v, float* edge1, float* edge2) {
   float tvec[3], pvec[3], qvec[3];
   float det,inv_det;

   /* find vectors for two edges sharing vert0 */
   SUB(edge1, vert1, vert0);
   SUB(edge2, vert2, vert0);

   /* begin calculating determinant - also used to calculate U parameter */
   CROSS(pvec, dir, edge2);

   /* if determinant is near zero, ray lies in plane of triangle */
   det = DOT(edge1, pvec);

#ifdef TEST_CULL           /* define TEST_CULL if culling is desired */
   if (det < EPSILON)
      return 0;

   /* calculate distance from vert0 to ray origin */
   SUB(tvec, orig, vert0);

   /* calculate U parameter and test bounds */
   *u = DOT(tvec, pvec);
   if (*u < 0.0 || *u > det)
      return 0;

   /* prepare to test V parameter */
   CROSS(qvec, tvec, edge1);

    /* calculate V parameter and test bounds */
   *v = DOT(dir, qvec);
   if (*v < 0.0 || *u + *v > det)
      return 0;

   /* calculate t, scale parameters, ray intersects triangle */
   *t = DOT(edge2, qvec);
   inv_det = 1.0 / det;
   *t *= inv_det;
   *u *= inv_det;
   *v *= inv_det;
#else                    /* the non-culling branch */
   if (det > -EPSILON && det < EPSILON)
     return 0;
   inv_det = 1.0 / det;

   /* calculate distance from vert0 to ray origin */
   SUB(tvec, orig, vert0);

   /* calculate U parameter and test bounds */
   *u = DOT(tvec, pvec) * inv_det;
   if (*u < 0.0 || *u > 1.0)
     return 0;

   /* prepare to test V parameter */
   CROSS(qvec, tvec, edge1);

   /* calculate V parameter and test bounds */
   *v = DOT(dir, qvec) * inv_det;
   if (*v < 0.0 || *u + *v > 1.0)
     return 0;

   /* calculate t, ray intersects triangle */
   *t = DOT(edge2, qvec) * inv_det;
#endif
   return *t > 0.0;
}


bool Triangle::intersect(const Ray& r, IntersectionResult& ir) const {
if ( intersect_triangle(r.getStart().value, r.getDirection().value ,
                  this->p[0].value,this->p[1].value, this->p[2].value,
                  &(ir.t), &(ir.u), &(ir.v), ir.e1.value, ir.e2.value)) {
  ir.orig =  this->p[0];
  return true;
} else
  return false;
}

bool Triangle::intersect(RadianceRay& r) const {
float t,u,v;
Vector3D e1,e2;
if ( intersect_triangle(r.getStart().value, r.getDirection().value ,
                  this->p[0].value,this->p[1].value, this->p[2].value,
                  &t, &u, &v, e1.value, e2.value)) {
  Intersection current(this, this->p[0] + (u * e1) + (v * e2), r.getStart());
  if ( current < r.getClosestIntersection() ) {
    current.e1 = e1;
    current.e2 = e2;
    current.u = u;
    current.v = v;
    r.setClosestIntersection(current);
  }


  return true;
} else
  return false;
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

