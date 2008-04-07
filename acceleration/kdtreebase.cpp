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
#include <fstream>

#include "scene.h"
#include "kdnode.h"
#define NODE_SIZE (sizeof(KdNode) / sizeof(unsigned int))


using namespace Occluder;

const float KdTreeBase::C_trav = 0.1f;
const float KdTreeBase::C_intersect = 1.0f;

KdTreeBase::KdTreeBase(const Scene& scene):
  AccelerationStructure(scene), memBlock(0), primitiveBoxes(0)
 {
}


KdTreeBase::~KdTreeBase() {
  if ( memBlock ) 
    free (memBlock);
  if ( primitiveBoxes ) 
    free(primitiveBoxes);
  
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
  unsigned int i;
  cachePrimitiveAABBs();
  unsigned int size = (2 * primCount - 1) * sizeof(KdNode) // space for treenodes
                    + (unsigned int)(1.3f * primCount) * sizeof(unsigned int)    ;// space for object references
  totalSpace = size;
//     size = 5 * sizeof(KdNode)  + 1.5f * primCount * sizeof(unsigned int);
  memBlock = (unsigned int*)malloc(size);
  for (i = 0; i < primCount; ++i) {
    memBlock[i] = i;
  }
  subdivide(memBlock, primCount, scene.getAABB(), size / sizeof(unsigned int));
  const KdNode *root = (const KdNode *)memBlock;
  root->analyze();
//   writeToDisk();
}

void KdTreeBase::subdivide( unsigned int *memBlock, unsigned int primitiveCount, const AABB nodeBox, unsigned int size) {
  assert( size >= ( primitiveCount + 2 ) );
  
  if ( primitiveCount == 0) {
    KdNode::makeLeaf(memBlock, primitiveCount);
    return;
  }

  const KdTreeBase::SplitCandidate split = determineSplitpos(nodeBox, memBlock, primitiveCount);
  if ( split.axis == 3 ) {
      // no good split candidate found
    memBlock[primitiveCount] = memBlock[0];
    memBlock[primitiveCount+1] = memBlock[1];
    KdNode::makeLeaf(memBlock, primitiveCount);
    return;
  }

  unsigned int *fallBack = (unsigned int*) malloc(size * sizeof(unsigned int));
  memcpy(fallBack, memBlock, sizeof(unsigned int) * primitiveCount);

  unsigned int leftAndRightStart = size;
  unsigned int rightOnlyStart = size;
  unsigned int lastUnclassified = primitiveCount - 1;

  unsigned int l = 0;

  for ( unsigned int i = 0; i < primitiveCount; ++i) {
//     Overlap overlap2 = classify(memBlock[l], split, nodeBox);
    Overlap overlap = classifyVague(memBlock[l], split, nodeBox);
//     if ( overlap != overlap2)
//       overlap = classify(memBlock[l], split, nodeBox);
    if ( overlap == LEFT_ONLY) { // completely in left node
      ++l;
    } else  if ( overlap == RIGHT_ONLY ) { // completely in right node
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
  KdNode::makeInnernode(memBlock, split.axis, split.pos, rightStart << 2);
  subdivide(memBlock + leftStart, leftPrimitives, nodeBox.getHalfBox(split.axis, split.pos, true),  rightStart - NODE_SIZE );
  subdivide(memBlock + rightStart, rightPrimitives, nodeBox.getHalfBox(split.axis, split.pos, false),  size - rightStart );
}

void KdTreeBase::cachePrimitiveAABBs() {
  const unsigned int primCount = scene.getPrimitiveCount();
   primitiveBoxes = (AABB *)malloc(primCount * sizeof(AABB));
  for ( unsigned int i = 0; i < primCount; ++i) {
    primitiveBoxes[i] =  scene.getPrimitive(i).getAABB();
  }
}

KdTreeBase::SplitCandidate KdTreeBase::determineSplitpos(const AABB& v, const unsigned int *primitves, const unsigned int primitveCount) {
  // determine longest axis of bounding box
  const Vec3 aabbWidth( v.getWidths() );
  unsigned char axis = 0;
  axis = ( aabbWidth[1] > aabbWidth[0] )    ? 1 : 0;
  axis = ( aabbWidth[2] > aabbWidth[axis] ) ? 2 : axis;

  // determine splitposition on half of longest axis
  const SplitCandidate result = { v.getMin(axis) + ( aabbWidth[axis] * 0.5f ), axis, false };
  return result;
}

void KdTreeBase::writeToDisk() const {
    std::ofstream myFile ("kdtree.bin", std::ios::out | std::ios::binary);
    myFile.write ((const char*)memBlock, totalSpace);
    myFile.close();
}

KdTreeBase::Overlap KdTreeBase::classify(unsigned int primitiveID, const KdTreeBase::SplitCandidate split, const AABB &aabb) {
  const Primitive& primitive = scene.getPrimitive(primitiveID);
  const AABB primAABB = getAABBForPrimitiv(primitiveID);
  if ((primAABB.getMin(split.axis) == primAABB.getMax(split.axis)) && (primAABB.getMin(split.axis) == split.pos))
    return (split.putInPlaneLeft) ? LEFT_ONLY : RIGHT_ONLY;

  if ( primAABB.getMax(split.axis) < split.pos)  // completely in left node
    return LEFT_ONLY;
  else  if ( primAABB.getMin(split.axis) > split.pos)  // completely in right node
    return RIGHT_ONLY;
  

  const unsigned otherAxis[2] = {(split.axis + 1) % 3, (split.axis + 2) % 3};
  // find vertex that's alone on one side of split plane
  unsigned char left = 0, right = 0;
  unsigned char lefttri[2], righttri[2];
  for (unsigned int p = 0; p < 3; ++p)
    if ( primitive[p][split.axis] < split.pos ) {
      lefttri[left] = p;
      ++left;
    } else {
      righttri[right] = p;
      ++right;
    }

  const bool aloneOnleft = (left<right);
  const Vec3& alone =  primitive[ aloneOnleft?lefttri[0]:righttri[0]];
  const Vec3& other0 = primitive[!aloneOnleft?lefttri[0]:righttri[0]];
  const Vec3& other1 = primitive[!aloneOnleft?lefttri[1]:righttri[1]];

  const Vec3 a0(other0 - alone);
  const Vec3 a1(other1 - alone);

  float d;
  if ( aloneOnleft )
    d = split.pos - alone[split.axis];
  else
    d = alone[split.axis] - split.pos;
  
  float ratio0 = fabsf(d / ( other0[split.axis] - alone[split.axis] ));
  float ratio1 = fabsf(d / ( other1[split.axis] - alone[split.axis] ));

  const Vec3 s1(alone + (ratio0 * a0));
  const Vec3 s2(alone + (ratio1 * a1));

  Vec3 s1s2min(s1);
  Vec3 s1s2max(s2);

  for ( unsigned int c = 0; c < 2; ++c)
    if (s1[otherAxis[c]] > s2[otherAxis[c]]) {
      s1s2min[otherAxis[c]] = s2[otherAxis[c]];
      s1s2max[otherAxis[c]] = s1[otherAxis[c]];
    }

  bool fullOut = false;
  bool above;
  unsigned int c = 0;
  for ( ; !fullOut && (c < 2); ++c) {
    if ( s1s2max[otherAxis[c]] < aabb.getMin(otherAxis[c]) ) {
      fullOut = true;
      above = false;
      break;
    }
    if ( s1s2min[otherAxis[c]] > aabb.getMax(otherAxis[c]) ) {
      fullOut = true;
      above = true;
      break;
    }
  }

  if ( fullOut ) {
    return ((( (a0[otherAxis[c]] > 0.0f) ^ above ) ^ aloneOnleft ) ? LEFT_ONLY : RIGHT_ONLY);
  } else {
    Vec3 s12(s2 - s1);
    Vec3 q;
    q[split.axis] = 1.0f;
    q[otherAxis[0]] = q[otherAxis[1]] = 0.0;
    Vec3 normal(s12 % q);
  
    bool b0 = (normal[otherAxis[0]] > 0.0);
    bool b1 = (normal[otherAxis[1]] > 0.0);
    Vec3 B0(split.pos),B1(split.pos);
  
    if ( b0 ) {
      B0[otherAxis[0]] = aabb.getMax(otherAxis[0]);
      B1[otherAxis[0]] = aabb.getMin(otherAxis[0]);
    } else {
      B0[otherAxis[0]] = aabb.getMin(otherAxis[0]);
      B1[otherAxis[0]] = aabb.getMax(otherAxis[0]);
    }
    if ( b1 ) {
      B0[otherAxis[1]] = aabb.getMax(otherAxis[1]);
      B1[otherAxis[1]] = aabb.getMin(otherAxis[1]);
    } else {
      B0[otherAxis[1]] = aabb.getMin(otherAxis[1]);
      B1[otherAxis[1]] = aabb.getMax(otherAxis[1]);
    }

    float m1 = ( s1 - B0 ) * normal;
    float m2 = ( s1 - B1 ) * normal;

    if ( !(0x80000000 & (unsigned int&)(m1) ^ ( 0x80000000 & (unsigned int&)(m2)))) {
      float q1 = ( alone - B0 ) * normal;
      return  (aloneOnleft ^ !(0x80000000 & (unsigned int&)(m1) ^ ( 0x80000000 & (unsigned int&)(q1)))) ? LEFT_ONLY : RIGHT_ONLY;
    } else 
      return BOTH;
   }

}

KdTreeBase::Overlap KdTreeBase::classifyVague(unsigned int primitiveID, const KdTreeBase::SplitCandidate split, const AABB &aabb) {
  const AABB primAABB = getAABBForPrimitiv(primitiveID);
  if ( primAABB.getMax(split.axis) <= split.pos)  // completely in left node
    return LEFT_ONLY;
  else  if ( primAABB.getMin(split.axis) >= split.pos)  // completely in right node
    return RIGHT_ONLY;
  return BOTH;
}

