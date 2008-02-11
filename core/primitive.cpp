//
// C++ Implementation: primitiv
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "primitive.h"
#include "scene.h"
#include "intersection.h"
#include "raysegmentignore.h"
#include "raysegmentsse.h"
#include "intersectionsse.h"

using namespace Occluder;

Primitive::Primitive(unsigned int p0, unsigned int p1, unsigned int p2, const Scene& scene, const std::string& shaderName):
p0(p0), p1(p1), p2(p2), u(scene.getVertex(p1) - scene.getVertex(p0) ),
v(scene.getVertex(p2) - scene.getVertex(p0) ), normal( (u % v).normal() ), scene(scene), shader(scene.getShader(shaderName)) {}


Primitive::~Primitive() {
}

bool Primitive::intersects(const RaySegment& r) const {
   const Vec3 pvec(r.getDirection() % this->v);
   const float det = this->u * pvec;

   if (det > -EPSILON && det < EPSILON)
     return false;
   const float inv_det = 1.0 / det;

   const Vec3 tvec = r.getOrigin() - scene.getVertex(p0);

   const float u = (tvec * pvec) * inv_det;
   if (u < 0.0 || u > 1.0)
     return false;

   const Vec3 qvec = tvec % this->u;
   const float v = (r.getDirection() * qvec) * inv_det;
   if (v < 0.0 || u + v > 1.0)
     return false;

   const float t = (this->v * qvec) * inv_det;
   return ( (t > r.getTMin()) && (t <= r.getTMax()) );
}

bool Primitive::intersects(const RaySegmentIgnore& r) const {
   const Vec3 pvec(r.getDirection() % this->v);
   const float det = this->u * pvec;

   if (det > -EPSILON && det < EPSILON)
     return false;
   const float inv_det = 1.0 / det;

   const Vec3 tvec = r.getOrigin() - scene.getVertex(p0);

   const float u = (tvec * pvec) * inv_det;
   if (u < 0.0 || u > 1.0)
     return false;

   const Vec3 qvec = tvec % this->u;
   const float v = (r.getDirection() * qvec) * inv_det;
   if (v < 0.0 || u + v > 1.0)
     return false;

   const float t = (this->v * qvec) * inv_det;
   return ( (t > r.getTMin()) && (t <= r.getTMax()) && ( this != &(r.getIgnoredPrimitve())));
}

const Intersection Primitive::getIntersection( const RaySegment& r) const {
   const Vec3 pvec(r.getDirection() % this->v);
   const float det = this->u * pvec;

   if (det > -EPSILON && det < EPSILON)
     return Intersection::getEmpty();
   const float inv_det = 1.0 / det;

   const Vec3 tvec = r.getOrigin() - scene.getVertex(p0);

   const float u = (tvec * pvec) * inv_det;
   if (u < 0.0 || u > 1.0)
     return Intersection::getEmpty();

   const Vec3 qvec = tvec % this->u;
   const float v = (r.getDirection() * qvec) * inv_det;
   if (v < 0.0 || u + v > 1.0)
     return Intersection::getEmpty();

   const float t = (this->v * qvec) * inv_det;
   if (!( (t > r.getTMin()) && (t <= r.getTMax()) ))
    return Intersection::getEmpty();

   return Intersection(r.getOrigin() + t * r.getDirection(), u, v, t, *this);
}

void Primitive::intersect(const RaySegmentSSE& rays, IntersectionSSE& result) const {
   const Vec3SSE p04(scene.getVertex(p0));
   const Vec3SSE tvec(rays.getOrigin() - p04);
   const Vec3SSE qvec(tvec % this->u);
   const Vec3SSE v4(this->v);
   const Vec3SSE u4(this->u);
   const Vec3SSE pvec(rays.getDirection() % v4);
   const Float4  det(u4 * pvec);
   
  Float4 hitMask((det < Float4::EPSILON4_NEG) | (Float4::EPSILON4 < det)) ;
  if ( hitMask == 0 )
    return /*false*/;
    
  const Float4 inv_det( Float4::ONE / det );
  const Float4 u( (tvec * pvec) * inv_det );

  hitMask &= ( ( u > Float4::zero() ) & ( u < Float4::ONE ) );
  if ( hitMask == 0 )
    return /*false*/;  

  const Float4 v( ( rays.getDirection() * qvec ) * inv_det );
  hitMask &= (  (v > Float4::zero()) & ( (u + v) < Float4::ONE ) );
  if ( hitMask == 0 )
    return;

  const Float4 t((v4 * qvec) * inv_det);
  hitMask &= ( ( t > rays.getTMin() ) &  ( rays.getTMax() > t ) );
  if ( hitMask == 0 )
    return;

  result.updateHits( hitMask, t, u, v);
//   
//   Intfloat triidx;
//   triidx.i = idx;
//   __m128 idxf = _mm_set1_ps(triidx.f);
//   rp.hitTriangle = _mm_or_ps(_mm_and_ps(idxf, hitMask) , _mm_and_ps(rp.hitTriangle.v.sse, inv_closerhits)); 
//   return /*true*/;
}
