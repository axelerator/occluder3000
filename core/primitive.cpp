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

Primitive::Primitive(unsigned int p0, unsigned int p1, unsigned int p2, const Scene& scene, const std::string& shaderName, unsigned int index):
p0(p0), p1(p1), p2(p2),
u(scene.getVertex(p1) - scene.getVertex(p0) ),
v(scene.getVertex(p2) - scene.getVertex(p0) ),
normal( (u % v).normal() ),
center(scene.getVertex(p0) + 0.33 * u + 0.33 * v),
scene(scene),
shader(scene.getShader(shaderName)),
index(index) {}


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

Float4 Primitive::intersect(const RaySegmentSSE& rays) const {
   const Vec3SSE p04(scene.getVertex(p0));
   const Vec3SSE tvec(rays.getOrigin() - p04);
   const Vec3SSE qvec(tvec % this->u);
   const Vec3SSE v4(this->v);
   const Vec3SSE u4(this->u);
   const Vec3SSE pvec(rays.getDirection() % v4);
   const Float4  det(u4 * pvec);
   
  Float4 hitMask((det < Float4::EPSILON4_NEG) | (Float4::EPSILON4 < det)) ;
  if ( hitMask == 0 )
    return hitMask;
    
  const Float4 inv_det( Float4::ONE / det );
  const Float4 u( (tvec * pvec) * inv_det );

  hitMask &= ( ( u > Float4::zero() ) & ( u < Float4::ONE ) );
  if ( hitMask == 0 )
    return hitMask;

  const Float4 v( ( rays.getDirection() * qvec ) * inv_det );
  hitMask &= (  (v > Float4::zero()) & ( (u + v) < Float4::ONE ) );
  if ( hitMask == 0 )
    return hitMask;

  const Float4 t((v4 * qvec) * inv_det);
  hitMask &= ( ( t > rays.getTMin() ) &  ( rays.getTMax() > t ) );
  if ( hitMask == 0 )
    return hitMask;

  return hitMask;
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

  result.updateHits( hitMask, t, u, v, index);
}

Vec3 Primitive::getSurfacePoint(float u, float t) const {
  return scene.getVertex(p0) + this->u *u + this->v * v;
}

const AABB Primitive::getAABB() const {
  const Vec3& p0 = scene.getVertex(this->p0);
  const Vec3& p1 = scene.getVertex(this->p1);
  const Vec3& p2 = scene.getVertex(this->p2);
  const Vec3 min(fminf(fminf(p0[0], p1[0]), p2[0]),
                fminf(fminf(p0[1], p1[1]), p2[1]),
                fminf(fminf(p0[2], p1[2]), p2[2]));
  const Vec3 max(fmaxf(fmaxf(p0[0], p1[0]), p2[0]),
                fmaxf(fmaxf(p0[1], p1[1]), p2[1]),
                fmaxf(fmaxf(p0[2], p1[2]), p2[2]));
  return AABB(min, max);
}

const Vec3& Primitive::getVertex(unsigned int v) const {
  switch ( v ) {
    case 0: return scene.getVertex(p0);
    case 1: return scene.getVertex(p1);
    case 2: return scene.getVertex(p2);
    default: assert( "Invalid vertex number" == 0);
  }
return scene.getVertex(p0);
}
