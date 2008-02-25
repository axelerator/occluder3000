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
#include <iostream>

#include "scene.h"
#include "kdnode.h"
#define NODE_SIZE (sizeof(KdNode) / sizeof(unsigned int))


using namespace Occluder;

KdTreeBase::KdTreeBase(const Scene& scene):
  AccelerationStructure(scene), memBlock(0)
 {
}


KdTreeBase::~KdTreeBase() {
  if ( memBlock )
    free (memBlock);
}


bool KdTreeBase::hasIntersection(const RaySegment& ray) const {
  return false;
}

const Intersection KdTreeBase::getFirstIntersection(const RaySegment& ray) const {
  const RaySegment trimmed(ray.trim(scene.getAABB()));
  return traverseRecursive( *((KdNode*)memBlock), trimmed );
}


Intersection KdTreeBase::traverseRecursive( const KdNode& node, const RaySegment& r) const {

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

  if ( distToSplit < r.getTMin()) { // near voxel does not need to be traversed
      closest = traverseRecursive(node.getChild(far), r.resize(distToSplit, r.getTMax()));
  } else if ( r.getTMax() < distToSplit ) { // far voxel has not to be visited
    closest = traverseRecursive( node.getChild(near), r.resize( r.getTMin(),  distToSplit));
  } else { // both nodes have to be visited
    closest+= traverseRecursive( node.getChild(near), r.resize( r.getTMin(),  distToSplit ));
    if ( closest.isEmpty() )
      closest = traverseRecursive( node.getChild(far), r.resize( distToSplit, r.getTMax() ));
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
  const unsigned int primCount = scene.getPrimitiveCount();
  unsigned int size = (2 * primCount - 1) * sizeof(KdNode) // space for treenodes
                    + primCount * sizeof(unsigned int)    ;// space for object references
//     size = 5 * sizeof(KdNode)  + 1.5f * primCount * sizeof(unsigned int);
  memBlock = (unsigned int*)malloc(size);
  for (unsigned int i = 0; i < primCount; ++i) {
    memBlock[i] = i;
  }
  subdivide(memBlock, primCount, scene.getAABB(), size / sizeof(unsigned int));
  const KdNode *root = (const KdNode *)memBlock;
  root->analyze();
}

void KdTreeBase::subdivide( unsigned int *memBlock, unsigned int primitiveCount, const AABB nodeBox, unsigned int size) {
  std::cout << "fart";
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
    const AABB primAABB = scene.getPrimitive(memBlock[l]).getAABB();
    if ( primAABB.getMax(axis) < splitPos) { // completely in left node
      ++l;
    } else  if ( primAABB.getMin(axis) >= splitPos) { // completely in right node
      --leftAndRightStart;
      --rightOnlyStart;
      memBlock[leftAndRightStart] = memBlock[rightOnlyStart];
      memBlock[rightOnlyStart] = memBlock[l];
      memBlock[l] = memBlock[lastUnclassified];
      --lastUnclassified;
    } else {// overlaps both nodes -> reference replication
      if ( (leftAndRightStart - lastUnclassified) < ( 3 * NODE_SIZE + (rightOnlyStart - leftAndRightStart)) ) {
         // not enough memory for 1 inner node & 2 leaves -> create leaf
        memcpy(memBlock + NODE_SIZE, fallBack, primitiveCount * sizeof(unsigned int));
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
  const unsigned int leftOnly = l;// number of objects in left only block

  const unsigned int leftAndRight = (rightOnlyStart - leftAndRightStart);
  // not enough memory for 1 inner node & 2 leaves & replicated references -> create leaf
  // TODO: move this test completely into upper loop
  if ( (leftAndRightStart - l) < ( 3 * NODE_SIZE + leftAndRight) ) {
    memcpy(memBlock + NODE_SIZE, fallBack, primitiveCount * sizeof(unsigned int));
    KdNode::makeLeaf(memBlock, primitiveCount);
    free(fallBack);
    return;
  }
  free(fallBack);
  

  const unsigned int leftPrimitives = leftOnly + leftAndRight;
  const unsigned int rightPrimitives = size - leftAndRightStart;

  float p = (float)leftPrimitives / (leftPrimitives + rightPrimitives);
  memBlock[l] = memBlock[0];
  memBlock[l + 1] = memBlock[1];
  l += NODE_SIZE;
  // |NODE|__left only__|_______________________|__left&right__|__right only__|
  
  const unsigned int leftStart = NODE_SIZE;
  // copy 'left and right' block to end of 'left only' block
  memmove( memBlock + leftStart + leftOnly, memBlock + leftAndRightStart, leftAndRight * sizeof( unsigned int));
  // |NODE|__left only__|__left&right__|________|__left&right__|__right only__|

  //   for left node ends after: this node + left only + replicated references
  const unsigned int leftEnd    = leftStart + leftOnly + leftAndRight + NODE_SIZE;

  const unsigned int free       = leftAndRightStart - leftEnd - NODE_SIZE;

  // the amount of mem scheduled for the left child
  const unsigned int leftFree   = (unsigned int)(p * free);
  const unsigned int rightStart = leftEnd + leftFree;
  // shift content of ride node left to grant free mem for its children
  memmove( memBlock + rightStart, memBlock + leftAndRightStart, rightPrimitives * sizeof(unsigned int));
  // p = i.e. 0.5
  // |NODE|__left only__|__left&right__|____|__left&right__|__right only__|____|
  KdNode::makeInnernode(memBlock, axis, splitPos, rightStart << 2);
  std::cout << "\nleft:";
  for ( unsigned int i = 0; i < leftPrimitives; ++i)
    std::cout << memBlock[NODE_SIZE + i] << ",";
  subdivide(memBlock + leftStart, leftPrimitives, nodeBox.getHalfBox(axis, splitPos, true),  rightStart - NODE_SIZE );
  std::cout << "\nright:";
  for ( unsigned int i = 0; i < rightPrimitives; ++i)
    std::cout << memBlock[rightStart + i] << ",";
  subdivide(memBlock + rightStart, rightPrimitives, nodeBox.getHalfBox(axis, splitPos, false),  size - rightStart );
  std::cout << "\n";
}
