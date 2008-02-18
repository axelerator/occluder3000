//
// C++ Implementation: intersection
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "intersection.h"

#include "primitive.h"


using namespace Occluder;
const Intersection Intersection::empty(Vec3(), 0.0f, 0.0f, -1.0f, *((Primitive*)0));


Intersection::Intersection(const Vec3& position, float u,  float v, float t, const Primitive& primitive) :
position(position), u(u), v(v), t(t), primitive(primitive){}


Intersection::~Intersection() {
}

Vec3 Intersection::getRadiance(const Vec3& direction, unsigned int depth) const {
  return primitive.getShader().getRadiance(direction, *this, depth);
}

/**
   TODO: a good candidate for inlining, but impossible due to cyclic dependency
 **/
const Vec3& Intersection::getNormal() const {
  return primitive.getNormal();
}

Intersection &Intersection::operator +=(const Intersection& op) {
  if ( op < *this)
    memcpy(this, &op, sizeof(Intersection));
  return *this;
}