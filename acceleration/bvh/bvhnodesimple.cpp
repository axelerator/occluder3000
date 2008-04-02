//
// C++ Implementation: bvhnodesimple
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bvhnodesimple.h"
#include "primitive.h"

using namespace Occluder;


// Implementation of methods of BvhNodeSimple ---------------------------------

BvhNodeSimple::BvhNodeSimple() {}

// Implementation of methods of BvhSimpleLeaf ---------------------------------

BvhSimpleLeaf::BvhSimpleLeaf(const Primitive& primitive):
primitive(primitive){}

const AABB BvhSimpleLeaf::update() {
  return primitive.getAABB();
}

// info methods
unsigned int BvhSimpleLeaf::leafCount() const {
  return 1;
}

unsigned int BvhSimpleLeaf::treeDepth() const {
  return 1;
}

const Intersection BvhSimpleLeaf::getFirstIntersection(const RaySegment& ray) const {
  return primitive.getIntersection(ray);
}

bool BvhSimpleLeaf::hasIntersection(const RaySegment& ray) const {
  return primitive.intersects(ray);
}

// Implementation of methods of BvhSimpleInner --------------------------------

BvhSimpleInner::BvhSimpleInner(BvhNodeSimple *left, BvhNodeSimple *right):
left(left), right(right) {}

BvhSimpleInner::~BvhSimpleInner() {
  delete left;
  delete right;
}

const AABB BvhSimpleInner::update() {
  aabb = left->update() + right->update();
  return aabb;
}

// info methods
unsigned int BvhSimpleInner::leafCount() const {
  return left->leafCount() + right->leafCount();
}


unsigned int BvhSimpleInner::treeDepth() const {
  const unsigned int l = left->treeDepth();
  const unsigned int r = left->treeDepth();
  return 1 + ((l > r) ? l : r);
}

const Intersection BvhSimpleInner::getFirstIntersection(const RaySegment& ray) const {
  if ( ray.intersects(aabb) ) {
    Intersection i = left->getFirstIntersection(ray);
    i += right->getFirstIntersection(ray);
    return i;
  }
  return Intersection::getEmpty();
}

bool BvhSimpleInner::hasIntersection(const RaySegment& ray) const {
  return left->hasIntersection(ray) || right->hasIntersection(ray);
}
