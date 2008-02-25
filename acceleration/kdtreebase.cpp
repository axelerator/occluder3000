//
// C++ Implementation: kdtreebase
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "kdtreebase.h"
#include <assert.h>
#include "scene.h"
#include "kdnode.h"
#define NODE_SIZE (sizeof(KdNode) / sizeof(unsigned int))


using namespace Occluder;

KdTreeBase::KdTreeBase(const Scene& scene):
  AccelerationStructure(scene)
 {
}


KdTreeBase::~KdTreeBase() {
}


bool KdTreeBase::hasIntersection(const RaySegment& ray) const {
  return false;
}

const Intersection KdTreeBase::getFirstIntersection(const RaySegment& ray) const {
  const RaySegment trimmed(ray.trim(scene.getAABB()));
  return traverseRecursive( *((KdNode*)memBlock), trimmed, trimmed.getTMin(), trimmed.getTMax() );
}


Intersection KdTreeBase::traverseRecursive( const KdNode& node, const RaySegment& r, float tmin, float tmax) const {

  Intersection closest(Intersection::getEmpty());
  // 1. ------------------ Leaf case  ------------------------
  if ( node.isLeaf() ) {
    const KdNode::IndexList primitives = node.getPrimitiveList();
    for (unsigned int i = 0; i < primitives.length ; ++i ) {
      closest += scene.getPrimitive(primitives.start[i]).getIntersection( r );
    }
    return closest;
  }
  // 2. ------------------ inner node case  ------------------------
  const unsigned int far = (r.getDirection()[node.getAxis()] > 0.0f) ? 1 : 0;
  const unsigned int near = 1 - far;

  const float distToSplit = (node.getSplitPos() - r.getOrigin()[node.getAxis()] ) * r.getInvDirection()[node.getAxis()];

  if ( tmin > distToSplit) { // near voxel does not need to be traversed
      closest = traverseRecursive(node.getChild(far), r, distToSplit, tmax);
  } else if ( distToSplit > tmax) { // far voxel has not to be visited
    closest = traverseRecursive( node.getChild(near), r, tmin,  distToSplit);
  } else { // both nodes have to be visited
    closest = traverseRecursive( node.getChild(near), r, tmin,  distToSplit);
    if ( !closest.isEmpty() )
      closest = traverseRecursive( node.getChild(far), r, distToSplit, tmax);
  }
  return closest;
}


Float4 KdTreeBase::haveIntersections(const RaySegmentSSE& ray) const {
  return Float4::zero();
}

void KdTreeBase::determineFirstIntersection(const RaySegmentSSE& ray, IntersectionSSE& result) const {
}

void KdTreeBase::getAllIntersections(const RaySegment& ray, List< const Intersection >& results) const {
}


void KdTreeBase::construct() {

//   unsigned int size = sizeof(KdNode); // space for treenodes
  const unsigned int primCount = scene.getPrimitiveCount();
// 
//   memBlock = (unsigned int*)malloc(sizeof(KdNode));
//   KdNode::makeLeaf(memBlock, 0);
//   const KdNode *root = (const KdNode *)memBlock;
//   root->analyze();
//   return;

  unsigned int size = (2 * primCount - 1) * sizeof(KdNode) // space for treenodes
                    + primCount * sizeof(unsigned int)    ;// space for object references
  memBlock = (unsigned int*)malloc(size);
  for (unsigned int i = 0; i < primCount; ++i) {
    memBlock[i] = i;
  }
  subdivide(memBlock, primCount, scene.getAABB(), size / sizeof(unsigned int));
  const KdNode *root = (const KdNode *)memBlock;
  root->analyze();
}

void KdTreeBase::subdivide( unsigned int *memBlock, unsigned int primitiveCount, const AABB nodeBox, unsigned int size) {

  assert( size >= ( primitiveCount + 2 ) );
  unsigned int *fallBack = (unsigned int*) malloc(size * sizeof(unsigned int));
  memcpy(fallBack, memBlock, sizeof(unsigned int) * primitiveCount);
  
  // determine longest axis of bounding box
  const Vec3 aabbWidth( nodeBox.getWidths() );
  unsigned char axis = 0;
  axis = ( aabbWidth[1] > aabbWidth[0] )    ? 1 : 0;
  axis = ( aabbWidth[2] > aabbWidth[axis] ) ? 2 : axis;

  // determine splitposition on half of longest axis
  const float splitPos = nodeBox.getMin(axis) + ( aabbWidth[axis] * 0.5f );


  unsigned int leftAndRightStart = size;
  unsigned int rightOnlyStart = size;
  unsigned int lastUnclassified = primitiveCount - 1;

  unsigned int l = 0;
  for ( unsigned int i = 0; i < primitiveCount; ++i) {
    const AABB primAABB = scene.getPrimitive(memBlock[i]).getAABB();
    if ( primAABB.getMax(axis) < splitPos) { // completely in left node
      ++l;
    } else  if ( primAABB.getMin(axis) >= splitPos) { // completely in right node
      --leftAndRightStart;
      memBlock[leftAndRightStart] = memBlock[rightOnlyStart];
      --rightOnlyStart;
      memBlock[rightOnlyStart] = memBlock[l];
      memBlock[l] = memBlock[lastUnclassified];
      --lastUnclassified;
    } else {// overlaps both nodes -> reference replication
      if ( (leftAndRightStart - lastUnclassified) < ( 3 * NODE_SIZE ) ) {
         // not enough memory for 1 inner node & 2 leaves -> create leaf
        memcpy(fallBack, memBlock + NODE_SIZE, primitiveCount * sizeof(unsigned int));
        KdNode::makeLeaf(memBlock, primitiveCount);
        free(fallBack);
        return;
      }
      --leftAndRightStart;
      memBlock[leftAndRightStart] = memBlock[l];
      memBlock[l] = memBlock[lastUnclassified];
      --lastUnclassified;
    }
  }
  // |____left only__|___________________________|__left&right__|__right only__|

  const unsigned int leftAndRightLength = (rightOnlyStart - leftAndRightStart);
  // not enough memory for 1 inner node & 2 leaves & replicated references -> create leaf
  // TODO: move this test completely into upper loop
  if ( (leftAndRightStart - l) < ( 3 * NODE_SIZE + leftAndRightLength) ) {
    memcpy(memBlock + NODE_SIZE, fallBack, primitiveCount * sizeof(unsigned int));
    KdNode::makeLeaf(memBlock, primitiveCount);
    free(fallBack);
    return;
  }
  free(fallBack);

  float p = 0.5;
  memcpy( memBlock + l, memBlock, sizeof(KdNode) );
  l += NODE_SIZE;
  // |NODE|__left only__|_______________________|__left&right__|__right only__|

  // copy 'left and right' block to end of 'left only' block
  memcpy( memBlock + l, memBlock + leftAndRightStart, leftAndRightLength * sizeof( unsigned int));
  // |NODE|__left only__|__left&right__|________|__left&right__|__right only__|

  // shift content of ride node left to grant free mem for its children
  const unsigned int leftEnd    = l + leftAndRightLength;
  const unsigned int free       = leftAndRightStart - ( leftEnd );
  const unsigned int leftFree   = (unsigned int)(p * free);
  const unsigned int rightStart = leftEnd + leftFree;
  const unsigned int rightPrimitives = size - leftAndRightStart;
  memcpy( memBlock + rightStart, memBlock + leftAndRightStart, rightPrimitives);
  // p = i.e. 0.5
  // |NODE|__left only__|__left&right__|____|__left&right__|__right only__|____|
  KdNode::makeInnernode(memBlock, axis, splitPos, rightStart << 2);
  subdivide(memBlock + NODE_SIZE, leftEnd - NODE_SIZE, nodeBox.getHalfBox(axis, splitPos, true),  rightStart - NODE_SIZE );
  subdivide(memBlock + rightStart, rightPrimitives, nodeBox.getHalfBox(axis, splitPos, false),  size - rightStart );
}
