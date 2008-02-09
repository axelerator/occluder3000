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
#include "raysegment.h"
#include "intersection.h"

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
