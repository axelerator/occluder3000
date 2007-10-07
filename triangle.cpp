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
Triangle::Triangle(const Vector3D& v1, const Vector3D& v2, const Vector3D& v3):
 u(v2-v1), v(v3-v1), nu(0.0, 0.0, 0.0), nv(0.0, 0.0, 0.0) {
  p[0] = v1;
  p[1] = v2;
  p[2] = v3;
  Vector3D normal((u%v).normal());
  n[0] = normal;
  n[1] = normal;
  n[2] = normal;  
}

Triangle::Triangle(const Vector3D& v1, const Vector3D& v2, const Vector3D& v3, const Vector3D& vn1, const Vector3D& vn2, const Vector3D& vn3):
 u(v2-v1), v(v3-v1), nu(vn2-vn1), nv(vn3-vn1) {
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

//#define TEST_CULL 1


int
intersect_triangle(const fliess orig[3], const fliess dir[3],
                   const fliess vert0[3], const fliess vert1[3], const fliess vert2[3],
                   fliess *t, fliess *u, fliess *v, fliess* edge1, fliess* edge2) {
   fliess tvec[3], pvec[3], qvec[3];
   fliess det,inv_det;

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
   return 1;
}


void Triangle::intersection(const Ray& r, IntersectionResult& ir) const {
/*  Vector3D pvec = r.getDirection() % v;
  fliess det = u * pvec;
  if ( Fliess::abs(det) < EPSILON )
    return Vector3D::undefined();
  fliess inv_det = 1.0 / det;
  Vector3D tvec = r.getStart() - p[0];
  fliess up = (tvec * pvec) * inv_det;
  if (up < 0.0 || up > 1.0)
    return Vector3D::undefined();
  Vector3D qvec = tvec % u;
  fliess vp = (r.getDirection() * qvec) * inv_det;
  if ( vp < 0.0 || up+vp > 1.0 )
    return Vector3D::undefined();
  
  return p[0] + up*u +vp *v;
*/

if ( intersect_triangle(r.getStart().value, r.getDirection().value ,
                  this->p[0].value,this->p[1].value, this->p[2].value,
                  &(ir.t), &(ir.u), &(ir.v), ir.e1.value, ir.e2.value)) {
// if ( intersect_triangle(r.getStart().value, r.getDirection().value ,
//                   this->p[2].value,this->p[1].value, this->p[0].value,
//                   &(ir.t), &(ir.u), &(ir.v), ir.e1.value, ir.e2.value)) {
  ir.setIntersection(true);
  ir.orig =  this->p[0];
  }
else
  ir.setIntersection(false);
}

Vector3D Triangle::getNormalAt(const IntersectionResult& ir) const {
  return Vector3D(n[0] + (nu * ir.u) + (nv * ir.v));
}

const Vector3D& Triangle::getPoint(unsigned int i) const{
  return p[i];
}

Triangle::~Triangle()
{}

