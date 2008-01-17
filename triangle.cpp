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
#include "raypacket.h"
#include "radianceray.h"
Triangle::Triangle(const Vector3D& v1, const Vector3D& v2, const Vector3D& v3, const PhongMaterial& mat): normal((u%v).normal()), u(v2-v1), v(v3-v1), nu(0.0, 0.0, 0.0), nv(0.0, 0.0, 0.0), 
center(v1 + 0.33*u + 0.33*v), mat(mat) {
  p[0] = v1;
  p[1] = v2;
  p[2] = v3;
  n[0] = normal;
  n[1] = normal;
  n[2] = normal;
}

Triangle::Triangle(const Vector3D& v1, const Vector3D& v2, const Vector3D& v3, const Vector3D& vn1, const Vector3D& vn2, const Vector3D& vn3, const PhongMaterial& mat):normal((u%v).normal()),
 u(v2-v1), v(v3-v1), nu(vn2-vn1), nv(vn3-vn1), center(v1 + 0.33*u + 0.33*v), mat(mat) {
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
   return ( (r.getIgnored() != this) && (t > r.getMin()) && (t < r.getMax()) );
}




bool Triangle::intersect(RadianceRay& r) const {
float t,u,v;

   float inv_det;

   Vector3D pvec(r.getDirection() % this->v);

   float det = this->u * pvec;

   if (det > -EPSILON && det < EPSILON)
     return 0;
   inv_det = 1.0 / det;

   Vector3D tvec(r.getStart() - p[0].value);

   u = (tvec * pvec) * inv_det;
   if (u < 0.0 || u > 1.0)
     return 0;

   Vector3D qvec( tvec % this->u );
   v = (r.getDirection() * qvec) * inv_det;
   if (v < 0.0 || u + v > 1.0)
     return 0;

   t = (this->v * qvec) * inv_det;
   if ( t <= r.getMin() || t > r.getMax() )
    return false;

  Intersection current(this, this->p[0] + (u * this->u) + (v * this->v), r.getStart());
  if ( (r.getIgnored() != this) && (current < r.getClosestIntersection()) ) {
    current.e1 = this->u;
    current.e2 = this->v;
    current.u = u;
    current.v = v;
    r.setClosestIntersection(current);
  }
  return true;
}

void Triangle::intersect(RayPacket& rp, unsigned int idx) const {
//   float t,u,v;
//    Vector3D tvec(rp.origin - p[0].value);
//    Vector3D qvec(tvec % this->u);
// 
//    float inv_det;
    for( unsigned int i = 0; i < rp.getR4Count() ; ++i ){
       intersect( rp.r4[i], idx);
       }
       

/*      Vector3D pvec(rp.getDirection(i) % this->v);
    
      float det = this->u * pvec;
    
      if (det > -EPSILON && det < EPSILON)
        continue;
      inv_det = 1.0 / det;
    
    
      u = (tvec * pvec) * inv_det;
      if (u < 0.0 || u > 1.0)
        continue;
    
      v = (rp.getDirection(i) * qvec) * inv_det;
      if (v < 0.0 || u + v > 1.0)
        continue;
    
      t = (this->v * qvec) * inv_det;
      if ( t <= rp.getMin(i) || t > rp.getMax(i) )
        continue;
    
      Intersection current(this, this->p[0] + (u * this->u) + (v * this->v), rp.getOrigin());
      if ( (current < rp.getClosestIntersection(i)) ) {
        current.e1 = this->u;
        current.e2 = this->v;
        current.u = u;
        current.v = v;      
        rp.getClosestIntersection(i) = current;
      }*/
//     }
}

bool Triangle::intersect(Ray4& rp, unsigned int idx) const {

   const SSEVec3D p04(p[0]);
   const SSEVec3D tvec(rp.origin - p04);
   const SSEVec3D qvec(tvec % this->u);
   const SSEVec3D v4(this->v);
   const SSEVec3D u4(this->u);
   const SSEVec3D pvec(rp.direction % v4);
   const SSE4 det(u4 * pvec);
   
  __m128 hitMask = _mm_or_ps(_mm_cmplt_ps(det.v.sse, EPSILON4INV ), _mm_cmplt_ps(EPSILON4, det.v.sse ));
  if ( ! ( _mm_movemask_ps(hitMask) ))
    return false;
    
  const SSE4 inv_det( _mm_div_ps(ONE , det.v.sse) );
  const SSE4 u( _mm_mul_ps( tvec * pvec , inv_det.v.sse ) );

  hitMask = _mm_and_ps(hitMask, _mm_and_ps(_mm_cmpgt_ps(u.v.sse, _mm_setzero_ps() ), _mm_cmplt_ps(u.v.sse, ONE )));
  if ( ! ( _mm_movemask_ps(hitMask) ))
    return false;  

   __m128 v = _mm_mul_ps( rp.direction * qvec, inv_det.v.sse );
  hitMask = _mm_and_ps(hitMask, _mm_and_ps(_mm_cmpgt_ps(v, _mm_setzero_ps() ), 
  _mm_cmplt_ps(_mm_add_ps(u.v.sse, v), ONE )));
  if ( ! ( _mm_movemask_ps(hitMask) ))
    return false;  
  
  const __m128 t = _mm_mul_ps( (v4 * qvec), inv_det.v.sse);
  hitMask = _mm_and_ps(hitMask, _mm_and_ps(_mm_cmpgt_ps(t, rp.tmin.v.sse ), _mm_cmpgt_ps(rp.tmax.v.sse, t )));
  if ( ! ( _mm_movemask_ps(hitMask) ))
    return false;  
     
  hitMask = _mm_and_ps(_mm_cmplt_ps(t, rp.t.v.sse), hitMask);
  
  rp.mask |= _mm_movemask_ps(hitMask);
  
  // update u,v,t for closer hits
  static __m128 one;
  memset(&one, 0xff, sizeof(__m128));  
  const __m128 inv_closerhits = _mm_andnot_ps(hitMask, one);
  rp.u = _mm_or_ps(_mm_and_ps(u.v.sse, hitMask), _mm_and_ps(rp.u.v.sse, inv_closerhits));
  rp.v = _mm_or_ps(_mm_and_ps(v, hitMask), _mm_and_ps(rp.v.v.sse, inv_closerhits));
  rp.t = _mm_or_ps(_mm_and_ps(t, hitMask), _mm_and_ps(rp.t.v.sse, inv_closerhits));
  
  Intfloat triidx;
  triidx.i = idx;
  __m128 idxf = _mm_set1_ps(triidx.f);
  rp.hitTriangle = _mm_or_ps(_mm_and_ps(idxf, hitMask) , _mm_and_ps(rp.hitTriangle.v.sse, inv_closerhits)); 
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

Vector3D& Triangle::getPoint(unsigned int i) {
  return p[i];
}

Triangle::~Triangle()
{}


