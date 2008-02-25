//
// C++ Implementation: kdtreesimple
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "kdtreesimple.h"
#include <assert.h>
#include "scene.h"
#include "kdnodebloated.h"

using namespace Occluder;

KdTreeSimple::KdTreeSimple(const Scene& scene, unsigned int minPrimPerLeaf, unsigned int maxDepth):
  AccelerationStructure(scene),
  primitiveBBs(0),
  root(0),
  primCount(0),
  minPrimPerLeaf(minPrimPerLeaf),
  maxDepth(maxDepth)
 {
}


KdTreeSimple::~KdTreeSimple() {
    if ( primitiveBBs )
        delete primitiveBBs;
    delete root;
}


bool KdTreeSimple::hasIntersection(const RaySegment& ray) const {
  return false;
}

const Intersection KdTreeSimple::getFirstIntersection(const RaySegment& ray) const {
  const RaySegment trimmed(ray.trim(scene.getAABB()));
  return traverseRecursive( *root, trimmed );
}


Intersection KdTreeSimple::traverseRecursive( const KdNodeBloated& node, const RaySegment& r) const {

  Intersection closest(Intersection::getEmpty());
  // 1. ------------------ Leaf case  ------------------------
  if ( node.isLeaf() ) {    
    const List<unsigned int>& primitives = node.getPrimitives();
    List<unsigned int>::const_iterator iter;
    for (iter = primitives.begin(); iter != primitives.end(); ++iter ) {
      closest += scene.getPrimitive(*iter).getIntersection( r );
    }
    return closest;
  }
  // 2. ------------------ inner node case  ------------------------
  const unsigned int far = (r.getDirection()[node.getAxis()] > 0.0f) ? 1 : 0;
  const unsigned int near = 1 - far;

  const float distToSplit = (node.getSplitpos() - r.getOrigin()[node.getAxis()] ) * r.getInvDirection()[node.getAxis()];

  if ( distToSplit < r.getTMin()) { // near voxel does not need to be traversed
      closest = traverseRecursive(node.getChild(far), r.resize(distToSplit, r.getTMax()));
  } else if ( r.getTMax() < distToSplit ) { // far voxel has not to be visited
    closest = traverseRecursive( node.getChild(near), r.resize(r.getTMin(),  distToSplit));
  } else { // both nodes have to be visited
    closest = traverseRecursive( node.getChild(near), r.resize(r.getTMin(),  distToSplit));
    if ( closest.isEmpty() )
      closest = traverseRecursive( node.getChild(far), r.resize(distToSplit, r.getTMax()));
  }
  return closest;
}

void KdTreeSimple::construct() {
    // malloc not new because we want no contructor calls here
    primitiveBBs = (AABB*)malloc(scene.getPrimitiveCount() * sizeof(AABB));
    primCount = scene.getPrimitiveCount();
    List<unsigned int> primitives;
    for ( unsigned int i = 0; i < primCount; ++i) {
        primitiveBBs[i] = scene.getPrimitive(i).getAABB();
        primitives.push_back(i);
    }
    root = subdivide(primitives, scene.getAABB(), maxDepth);
    root->analyze();
}

KdNodeBloated *KdTreeSimple::subdivide(const List<unsigned int>& primitives, const AABB& aabb, unsigned int depth) {
  if (  ( primitives.size() <= minPrimPerLeaf ) || ( depth == 0 ) ) {
    return new KdNodeBloated(primitives); // create leaf
  }

  // determine longest axis of bounding box
  const Vec3 aabbWidth( aabb.getWidths() );
  unsigned char axis = 0;
  axis = ( aabbWidth[1] > aabbWidth[0] )    ? 1 : 0;
  axis = ( aabbWidth[2] > aabbWidth[axis] ) ? 2 : axis;

  // determine splitposition on half of longest axis
  const float splitPos = aabb.getMin(axis) + ( aabbWidth[axis] * 0.5f );

  List<unsigned int> leftPrims;
  List<unsigned int> rightPrims;

  for (unsigned int i = 0; i < primCount; ++i) {
    if ( primitiveBBs[i].getMin(axis) < splitPos )
      leftPrims.push_back(i);
    if ( primitiveBBs[i].getMax(axis) >= splitPos )
      rightPrims.push_back(i);
  }
  assert(leftPrims.size() + rightPrims.size() >= primitives.size());
  const AABB leftBox = aabb.getHalfBox(axis, splitPos, true);
  const AABB rightBox = aabb.getHalfBox(axis, splitPos, false);

  return new KdNodeBloated( subdivide ( leftPrims,  leftBox,  depth - 1),
                            subdivide ( rightPrims, rightBox, depth - 1),
                            axis, splitPos);

}

Float4 KdTreeSimple::haveIntersections(const RaySegmentSSE& ray) const {
  return Float4::zero();
}

void KdTreeSimple::determineFirstIntersection(const RaySegmentSSE& ray, IntersectionSSE& result) const {
}

void KdTreeSimple::getAllIntersections(const RaySegment& ray, List< const Intersection >& results) const {
}

