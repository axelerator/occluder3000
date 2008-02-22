//
// C++ Implementation: bihcompact
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <assert.h>
#include "bihcompact.h"
#include "scene.h"


using namespace Occluder;

BihCompact::BihCompact(const Scene& scene, unsigned int primPerLeaf): 
  AccelerationStructure(scene),
  primIds(0),
  minPrimPerLeaf(primPerLeaf),
  nodes(0),
  nextFree(0)

{}

bool BihCompact::hasIntersection(const RaySegment& ray) const {
  return traverseRecursiveShadow( nodes[0], ray, ray.getTMin(), ray.getTMax() );
}

bool BihCompact::traverseRecursiveShadow(const BihNodeCompact& node, const RaySegment& r, float tmin, float tmax) const {
  Intersection closest(Intersection::getEmpty());
  if ( node.isLeaf() ) {
    for (unsigned int i = node.first(); i <= node.last(); ++i ) {
      if ( scene.getPrimitive(primIds[i]).intersects( r ) )
        return true;
    }
    return false;
  }
  const unsigned int far = (r.getDirection()[node.getAxis()] > 0.0f) ? 1 : 0;
  const unsigned int near = 1 - far;

  const float distToNear = (node.getExtremum(near) - r.getOrigin()[node.getAxis()] ) * r.getInvDirection()[node.getAxis()];
  const float distToFar  = (node.getExtremum(far)  - r.getOrigin()[node.getAxis()] ) * r.getInvDirection()[node.getAxis()];

  if ( tmin > distToNear) { // near voxel ist not needed to be traverses 
    if ( distToFar < tmax ) { // far voxel has to be traversed
      return traverseRecursiveShadow(node.getChild(far), r, fmaxf ( tmin, distToFar ), tmax);
    } // else: ray passes throug empty space between node planes
  } else if ( distToFar > tmax) { // far voxel has not to be visited
    return traverseRecursiveShadow( node.getChild(near), r, tmin,  fminf ( tmax, distToNear ));
  } else { // both nodes have to be visited
    return    traverseRecursiveShadow( node.getChild(near), r, tmin,  fminf ( tmax, distToNear ))
           || traverseRecursiveShadow( node.getChild(far), r, fmaxf ( tmin, distToFar ), tmax);
  }
  return false;
}

const Intersection BihCompact::getFirstIntersection(const RaySegment& ray) const {
  const RaySegment trimmed(ray.trim(scene.getAABB()));
  return traverseRecursive( nodes[0], trimmed, trimmed.getTMin(), trimmed.getTMax() );
}

Intersection BihCompact::traverseRecursive(const BihNodeCompact& node, const RaySegment& r, float tmin, float tmax) const {
  Intersection closest(Intersection::getEmpty());
  if ( node.isLeaf() ) {
    for (unsigned int i = node.first(); i <= node.last(); ++i ) {
      closest += scene.getPrimitive(primIds[i]).getIntersection( r );
    }
    return closest;
  }
  const unsigned int far = (r.getDirection()[node.getAxis()] > 0.0f) ? 1 : 0;
  const unsigned int near = 1 - far;

  const float distToNear = (node.getExtremum(near) - r.getOrigin()[node.getAxis()] ) * r.getInvDirection()[node.getAxis()];
  const float distToFar  = (node.getExtremum(far)  - r.getOrigin()[node.getAxis()] ) * r.getInvDirection()[node.getAxis()];

  if ( tmin > distToNear) { // near voxel ist not needed to be traverses 
    if ( distToFar < tmax ) { // far voxel has to be traversed

      closest += traverseRecursive(node.getChild(far), r, fmaxf ( tmin, distToFar ), tmax);
    } // else: ray passes throug empty space between node planes
  } else if ( distToFar > tmax) { // far voxel has not to be visited
    closest += traverseRecursive( node.getChild(near), r, tmin,  fminf ( tmax, distToNear ));
  } else { // both nodes have to be visited
    closest += traverseRecursive( node.getChild(near), r, tmin,  fminf ( tmax, distToNear ));
    if ( !closest.isEmpty() )
      tmax = fminf(closest.getT(), tmax );
    closest += traverseRecursive( node.getChild(far), r, fmaxf ( tmin, distToFar ), tmax);
  }
  return closest;
}

Float4 BihCompact::haveIntersections(const RaySegmentSSE& ray) const {
  return 0.0f;
}


void BihCompact::determineFirstIntersection(const RaySegmentSSE& ray, IntersectionSSE& result) const {
}

void BihCompact::getAllIntersections(const RaySegment& ray, List< const Intersection >& results) const {
}

void BihCompact::construct() {
  const unsigned int primCount = scene.getPrimitiveCount();
  if ( nodes ) {
    delete nodes;
    delete[] primIds;
  }
  primIds = new unsigned int[primCount];
  for (unsigned int i = 0; i < primCount; ++i)
    primIds[i] = i;
  nodes = (BihNodeCompact*)malloc((primCount * 2 - 1) * sizeof(BihNodeCompact));
  nextFree = nodes + 1; // first one is root node
  subdivide(nodes, 0, primCount - 1, scene.getAABB(), 24);
  nodes[0].analyze();
}

void BihCompact::subdivide(BihNodeCompact *node, unsigned int start, unsigned int end, const AABB& nodeAABB, unsigned int depth) {
  assert(node >= nodes);
  assert(node < ( nodes + scene.getPrimitiveCount() * 2 - 1));
  assert ( end < scene.getPrimitiveCount() );
  assert ( start <= end );
  if ( ( (end - start) < minPrimPerLeaf ) || ( depth == 0 ) ) {
    node->makeLeaf(start, end);
    return; // create leaf
  }


  // determine longest axis of bounding box
  const Vec3 aabbWidth( nodeAABB.getWidths() );
  unsigned char axis = 0;
  axis = ( aabbWidth[1] > aabbWidth[0] )    ? 1 : 0;
  axis = ( aabbWidth[2] > aabbWidth[axis] ) ? 2 : axis;

  // determine splitposition on half of longest axis
  const float splitPos = nodeAABB.getMin(axis) + ( aabbWidth[axis] * 0.5f );

  // Good ole quicksortlike partitioning
  unsigned int left = start;
  unsigned int right = end;
  do {
    while ( left < right && scene.getPrimitive( primIds[left] ).getCenter()[axis] <= splitPos ) 
      ++left;

    while ( right > left && scene.getPrimitive( primIds[right] ).getCenter()[axis] > splitPos )
      --right;

    if ( left < right ) {
      unsigned int tmp = primIds[left];
      primIds[left] = primIds[right];
      primIds[right] = tmp;
    }
  } while ( left < right );

  if ( scene.getPrimitive( primIds[right] ).getCenter()[axis] <
       scene.getPrimitive( primIds[left]  ).getCenter()[axis] ) {
    unsigned int tmp = primIds[left];
    primIds[left] = primIds[right];
    primIds[right] = tmp;
  }

  const AABB leftBox = nodeAABB.getHalfBox(axis, splitPos, true);
  const AABB rightBox = nodeAABB.getHalfBox(axis, splitPos, false);
  // don't accept empty leaves
  if ( left == end + 1 )
    return subdivide (node, start, end, leftBox, depth - 1);
  else if ( left == start )
    return subdivide (node, start, end, rightBox, depth - 1);
  else {
    float leftMax = nodeAABB.getMin(axis);
    float rightMin = nodeAABB.getMax(axis);
    for ( unsigned int i = start; i < left; ++i ) {
      for ( unsigned char c = 0; c < 3; ++c )
        if ( scene.getPrimitive(primIds[i]).getVertex( c )[axis] > leftMax )
          leftMax = scene.getPrimitive(primIds[i]).getVertex( c )[axis];
    }

    for ( unsigned int i = left; i <= end; ++i ) {
      for ( unsigned char c = 0; c < 3; ++c )
        if ( scene.getPrimitive(primIds[i]).getVertex( c )[axis] < rightMin )
          rightMin = scene.getPrimitive(primIds[i]).getVertex( c )[axis];
    }

    BihNodeCompact *leftNode = nextFree++; // allocate mem for left node
    subdivide(leftNode, start, left-1, leftBox, depth - 1);

    BihNodeCompact *rightNode = nextFree++;// allocate mem for left node
    subdivide(rightNode, left , end, rightBox, depth - 1);    // right node

    node->makeInner(rightNode, leftMax, rightMin, axis);
  }
}
