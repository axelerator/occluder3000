//
// C++ Implementation: bvhsimple
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <iostream>
#include <assert.h>
#include "bvhsimple.h"
#include "scene.h"
using namespace Occluder;

BvhSimple::BvhSimple(const Scene& scene): AccelerationStructure(scene), root(0) {
}


BvhSimple::~BvhSimple() {
  delete root;
}


bool BvhSimple::hasIntersection(const RaySegment& ray) const {
  return false;
}

const Intersection BvhSimple::getFirstIntersection(const RaySegment& ray) const {
  return root->getFirstIntersection(ray);
}

void BvhSimple::construct() {
  List<unsigned int> primitives;
  for (unsigned int i = 0; i < scene.getPrimitiveCount(); ++i) 
      primitives.push_back(i);
  root = subdivide(primitives);
  root->update();
  analyze();
}

BvhNodeSimple *BvhSimple::subdivide(const List<unsigned int>& primitives) {
  assert(primitives.size() > 0);
  if (primitives.size() == 1) {
    return new BvhSimpleLeaf(scene.getPrimitive(primitives[0]));
  }

  List<unsigned int>::const_iterator iter = primitives.begin();
  const unsigned int median = primitives.size() / 2;
  for (unsigned int i = 0; i < median; ++i)
    ++iter;

  const List<unsigned int> left(primitives.begin(), iter);
  const List<unsigned int> right(iter, primitives.end());
  return new BvhSimpleInner(subdivide(left), subdivide(right));
}

void BvhSimple::analyze() {
  std::cout << "Tree depth: " << root->treeDepth() << "\n";
  std::cout << "Node count: " << root->leafCount() << "\n";
}
