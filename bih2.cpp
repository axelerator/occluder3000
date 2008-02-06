//
// C++ Implementation: bih2
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bih2.h"
#include "assert.h"
#include "radianceray.h"
#include "raypacket.h"
#include "stats.h"

#define STACKDEPTH 512

BIH::BIH ( const Scene& scene )
    : AccelerationStruct ( scene ), triangleIndices ( 0 ), minimalPrimitiveCount ( 1 ), maxDepth ( 33 ),
    markednode ( 0 ) {}


BIH::~BIH() {}
void BIH::traverseRecursive ( RadianceRay& r, const BihNode & node, float tmin, float tmax) const {
  if ( node.type == 3) {
    statsinc("Traversalsteps per Frame");
    for (unsigned int i = node.leafContent[0]; i <= node.leafContent[1]; ++i ) {
      const Triangle& hitTriangle = triangles[triangleIndices[i]];
      hitTriangle.intersect ( r );
      #ifndef NDEBUG
        ++r.hittestcount;
      #endif
    }  
    return;
  }
  const unsigned int far = (r.getDirection().value[node.type] > 0.0f) ? 1 : 0;
  const unsigned int near = 1 - far;

  const float distToNear = (node.planes[near] - r.getStart().value[node.type] ) * r.getInvDirection().value[node.type];
  const float distToFar  = (node.planes[far]  - r.getStart().value[node.type] ) * r.getInvDirection().value[node.type];

  if ( tmin > distToNear) { // near voxel ist not needed to be traverses 
    if ( distToFar < tmax ) { // far voxel has to be traversed
      traverseRecursive( r, node.leftchild[far], fmaxf ( tmin, distToFar ), tmax);
    } // else: ray passes throug empty space between node planes
  } else if ( distToFar > tmax) { // far voxel has not to be visited
    traverseRecursive( r, node.leftchild[near], tmin,  fminf ( tmax, distToNear ));
  } else { // both nodes have to be visited
    traverseRecursive( r, node.leftchild[near], tmin,  fminf ( tmax, distToNear ));
    if ( r.didHitSomething() )
      tmax = fminf(r.getClosestIntersection().t, tmax );
    traverseRecursive( r, node.leftchild[far], fmaxf ( tmin, distToFar ), tmax);
  }
}


void BIH::traverseIterative ( RadianceRay& r ) const {

  Stack stack[STACKDEPTH];
  int stackpos = 1;
  float tmin,tmax, tNear, tFar;
  stack[0].node = &nodes.get ( 0 );
  stack[0].tmin = r.getMin();
  stack[0].tmax = r.getMax();
  const BihNode *node;
  unsigned int near, far, nearP[3] = { 0, 0, 0 }
                                     , farP[3] = { 1, 1, 1 }, i;
  // check ray direction to determine identity of 'near' and 'far' children
  for ( unsigned int i = 0; i < 3; ++i )
    if ( r.getDirection().value[i] < 0.0f )
      // near is right, far is left
      nearP[i] = 1, farP[i] = 0;
  while ( --stackpos > -1 ) {
    const Stack &current = stack[stackpos];
    // pop from stack
    tmin = current.tmin;
    tmax = current.tmax;
    node = current.node;

    while ( node->type != 3 ) {
      statsinc("Traversalsteps per Frame");
      near = nearP[node->type];
      far = farP[node->type];
      tNear = ( node->planes[near] - r.getStart().value[node->type] ) * r.getInvDirection().value[node->type];
      tFar = ( node->planes[far] - r.getStart().value[node->type] )  * r.getInvDirection().value[node->type];
      if ( tmin > tNear ) {
        if ( tFar < tmax) {
          tmin = fmaxf ( tmin, tFar );
          node = node->leftchild + far;
        } else
          break;
      } else if ( tmax  < tFar ) {
        tmax = fminf ( tmax, tNear );
        node = node->leftchild + near;
      } else {
        stack[stackpos].tmin = fmaxf ( tmin, tFar );
        stack[stackpos].tmax = tmax;
        stack[stackpos++].node = node->leftchild + far;
        tmax = fminf ( tmax, tNear );
        node = node->leftchild + near;
      }
    }
    if (node->type != 3)
      continue;
    statsinc("Traversalsteps per Frame");
    for ( i = node->leafContent[0]; i <= node->leafContent[1]; ++i ) {
      const Triangle& hitTriangle = triangles[triangleIndices[i]];
      hitTriangle.intersect ( r );
    }
  }
}

bool BIH::isBlocked ( Ray& r ) const{
  return traverseShadow ( r );
}

bool BIH::traverseShadow ( Ray& r ) const {

  Stack stack[STACKDEPTH];
  int stackpos = 1;
  float tmin,tmax, tNear, tFar;
  stack[0].node = &nodes.get ( 0 );
  stack[0].tmin = r.getMin();
  stack[0].tmax = r.getMax();
  const BihNode *node;
  unsigned int near, far, nearP[3] = { 0, 0, 0 }
                                     , farP[3] = { 1, 1, 1 }, i;
  // check ray direction to determine identity of 'near' and 'far' children
  for ( unsigned int i = 0; i < 3; ++i )
    if ( r.getDirection().value[i] < 0.0f )
      // near is right, far is left
      nearP[i] = 1, farP[i] = 0;
  while ( --stackpos > -1 ) {
    const Stack &current = stack[stackpos];
    // pop from stack
    tmin = current.tmin;
    tmax = current.tmax;
    node = current.node;

    while ( node->type != 3 ) {
      near = nearP[node->type];
      far = farP[node->type];
      tNear = ( node->planes[near] - r.getStart().value[node->type] ) * r.getInvDirection().value[node->type];
      tFar = ( node->planes[far] - r.getStart().value[node->type] )  * r.getInvDirection().value[node->type];
      if ( tmin > tNear ) {
        tmin = fmaxf ( tmin, tFar );
        node = node->leftchild + far;
      } else if ( tmax  < tFar ) {
        tmax = fminf ( tmax, tNear );
        node = node->leftchild + near;
      } else {
        stack[stackpos].tmin = fmaxf ( tmin, tFar );
        stack[stackpos].tmax = tmax;
        stack[stackpos++].node = node->leftchild + far;
        tmax = fminf ( tmax, tNear );
        node = node->leftchild + near;
      }
    }
    for ( i = node->leafContent[0]; i <= node->leafContent[1]; ++i ) {
      const Triangle& hitTriangle = triangles[triangleIndices[i]];
      if ( hitTriangle.intersect ( r ))
        return true;
    }
  }
  return false;
}

#ifndef NDEBUG
  unsigned int RadianceRay::hmax = 0;
#endif

const RGBvalue BIH::trace ( RadianceRay& r, unsigned int depth ) const {
  #ifndef NDEBUG
    r.hittestcount = 0;
  #endif

  if ( !trimRaytoBounds ( r ) )
    return RGBvalue ( 0.0, 0.0, 0.0 );

  RGBvalue result ( 0.0, 0.0, 0.0 );

  #ifndef NDEBUG

    traverseRecursive( r, nodes.get(0), r.getMin(), r.getMax() );
    if (r.hittestcount > RadianceRay::hmax ) {
      RadianceRay::hmax = r.hittestcount;
      std::cout << "max:" << RadianceRay::hmax << std::endl;
    }
  #else
   traverseIterative ( r );
  #endif
  r.shade ( result, depth );
  return result;
}

const Intersection&  BIH::getClosestIntersection ( RadianceRay& r ) const {
  traverseIterative ( r );
  return r.getClosestIntersection();
}

void BIH::recurse ( RayPacket& rp ) const {

  MultiStack stack[STACKDEPTH];
  int stackpos = 1;
  const BihNode *node = 0;
  __m128 tmin, tmax, tNear4, tFar4;
  
  stack[0].node = &nodes.get ( 0 );
  stack[0].tmin = rp.shaft.tmin.v.sse;
  stack[0].tmax = rp.shaft.tmax.v.sse;
//  const BihNode *node;
  unsigned int near, far, nearP[3] = { 0, 0, 0 }
      , farP[3] = { 1, 1, 1 }, i;
  // check ray direction to determine identity of 'near' and 'far' children
  if ( rp.shaft.direction.c[0].v.f[0] < 0.0f )
    // near is right, far is left
    nearP[0] = 1, farP[0] = 0;
  if ( rp.shaft.direction.c[1].v.f[0] < 0.0f )
    // near is right, far is left
    nearP[1] = 1, farP[1] = 0;
  if ( rp.shaft.direction.c[2].v.f[0] < 0.0f )
    // near is right, far is left
    nearP[2] = 1, farP[2] = 0;

  while ( --stackpos > -1 ) {
    const MultiStack &current = stack[stackpos];
    // pop from stack
    tmin = current.tmin;
    tmax = current.tmax;
    node = current.node;

    while ( node->type != 3 ) {
      statsinc("Traversalsteps per Frame");
      near = nearP[node->type];
      far = farP[node->type];
      
      __m128 currDist = _mm_sub_ps(_mm_set1_ps( node->planes[near] ),rp.shaft.origin.c[node->type].v.sse ) ;
       tNear4 = _mm_mul_ps( currDist, rp.shaft.inv_direction.c[node->type].v.sse );
      
      currDist = _mm_sub_ps(_mm_set1_ps( node->planes[far] ),rp.shaft.origin.c[node->type].v.sse ) ;
       tFar4 = _mm_mul_ps( currDist, rp.shaft.inv_direction.c[node->type].v.sse );
      
      if ( !_mm_movemask_ps( _mm_cmple_ps( tmin, tNear4 ) ) ) {
        tmin = _mm_max_ps ( tmin, tFar4 );
        node = node->leftchild + far;
      } else if ( !_mm_movemask_ps( _mm_cmpge_ps( tmax, tFar4) )) {
        tmax = _mm_min_ps ( tmax, tNear4 );
        node = node->leftchild + near;
      } else {
        stack[stackpos].tmin = _mm_max_ps ( tmin, tFar4 );
        stack[stackpos].tmax = tmax;
        stack[stackpos++].node = node->leftchild + far;
        tmax = _mm_min_ps ( tmax, tNear4 );
        node = node->leftchild + near;
      }
    }
    statsinc("Traversalsteps per Frame");
    for ( i = node->leafContent[0]; i <= node->leafContent[1]; ++i ) {
      const Triangle& hitTriangle = triangles[triangleIndices[i]];
      hitTriangle.intersect ( rp, triangleIndices[i] );
    }
  }
}


bool BIH::trace ( RayPacket& rp, unsigned int depth ) const {
  if ( !trimRaytoBounds ( rp ) )
    return false; // full miss
  recurse ( rp );
  return true;
}

void BIH::construct() {
  nodes.clear();
  const unsigned int objectCount = triangles.size();
  triangleIndices = ( unsigned int * ) malloc ( objectCount * sizeof ( unsigned int ) );
  for ( unsigned int i = 0 ; i < objectCount ; ++i )
    triangleIndices[i] = i;
  subdivide ( nodes.getNextFree(), 0, objectCount-1, bounds, 0 );
   std::cout << "construction done. Nodecount:" << nodes.size() << std::endl;
//   std::cout << "consistency check: " << ( isConsistent() ?"true":"false" ) << std::endl;
}

static const std::string leafcounts("Leafcount");
void BIH::subdivide ( BihNode &thisNode, unsigned int start, unsigned int end, const float *currBounds, unsigned int depth ) {
  assert ( end < triangles.size() );
  assert ( start <= end );
  // determine if we hit a termination condition
//   std::cout << depth << std::endl;
  if ( ( ( end - start ) < minimalPrimitiveCount )
       || ( depth > maxDepth ) ) {
    thisNode.type = 3;
    thisNode.leafContent[0] = start;
    thisNode.leafContent[1] = end;
    statsinc(leafcounts);
    return; // Yeah! we created a leaf !
  }

  // determine longest axis of bounding box
  const float bbLength[3] = {
                              currBounds[1] - currBounds[0],
                              currBounds[3] - currBounds[2],
                              currBounds[5] - currBounds[4]
                            };
  unsigned char axis;
  if ( bbLength[0] > bbLength[1] )
    axis = 0;
  else
    axis = 1;
  if ( bbLength[2] > bbLength[axis] )
    axis = 2;

  // Split the resulting axis in half
  const float splitVal = currBounds[axis * 2] + ( bbLength[axis] * 0.5 );

  unsigned int left = start;
  unsigned int right = end;
  // Good ole quicksortlike partitioning
  do {

    while ( left < right && triangles[triangleIndices[left]].getCenter().value[axis] <= splitVal ) {
      ++left;
    }

    while ( right > left && triangles[triangleIndices[right]].getCenter().value[axis] > splitVal ) {
      --right;
    }

    if ( left < right ) {
      unsigned int tmp = triangleIndices[left];
      triangleIndices[left] = triangleIndices[right];
      triangleIndices[right] = tmp;
    }

  } while ( left < right );

  if ( triangles[triangleIndices[right]].getCenter().value[axis] < triangles[triangleIndices[left]].getCenter().value[axis] ) {
    unsigned int tmp = triangleIndices[left];
    triangleIndices[left] = triangleIndices[right];
    triangleIndices[right] = tmp;
  }

  // split the current bounding box in two along the current axis
  float leftBounds[6], rightBounds[6];
  switch ( axis ) {
    case 0: leftBounds[0] = currBounds[0];
      leftBounds[1] = splitVal ;
      memcpy ( leftBounds+2, currBounds+2,  4 * sizeof ( float ) );

      rightBounds[0] = splitVal;
      rightBounds[1] = currBounds[1];
      memcpy ( rightBounds+2, currBounds+2, 4 * sizeof ( float ) );
      break;
    case 1: leftBounds[2] = currBounds[2];
      leftBounds[3] = splitVal ;
      leftBounds[0] = currBounds[0];
      leftBounds[1] = currBounds[1];
      leftBounds[4] = currBounds[4];
      leftBounds[5] = currBounds[5];

      rightBounds[2] = splitVal;
      rightBounds[3] = currBounds[3];
      rightBounds[0] = currBounds[0];
      rightBounds[1] = currBounds[1];
      rightBounds[4] = currBounds[4];
      rightBounds[5] = currBounds[5];
      break;
    case 2: leftBounds[4] = currBounds[4];
      leftBounds[5] = splitVal ;
      memcpy ( leftBounds, currBounds, 4 * sizeof ( float ) );
      rightBounds[4] = splitVal;
      rightBounds[5] = currBounds[5];
      memcpy ( rightBounds, currBounds, 4 * sizeof ( float ) );
      break;
  }

  if ( left == end + 1 )
    subdivide ( thisNode, start, end, leftBounds, depth );
  else if ( left == start )
    subdivide ( thisNode, start, end, rightBounds, depth );
  else {
    float leftMax = -UNENDLICH;
    float rightMin = UNENDLICH;
    for ( unsigned int i = start; i < left; ++i ) {
      for ( unsigned char c = 0; c < 3; ++c )
        if ( triangles[triangleIndices[i]].getPoint ( c ).value[axis] > leftMax )
          leftMax = triangles[triangleIndices[i]].getPoint ( c ).value[axis];
    }

    for ( unsigned int i = left; i <= end; ++i ) {
      for ( unsigned char c = 0; c < 3; ++c )
        if ( triangles[triangleIndices[i]].getPoint ( c ).value[axis] < rightMin )
          rightMin = triangles[triangleIndices[i]].getPoint ( c ).value[axis];
    }

    thisNode.leftchild = nodes.getNextFreePair();
    thisNode.planes[0] = leftMax;
    thisNode.planes[1] = rightMin;

    thisNode.type = axis;
    subdivide ( *thisNode.leftchild, start, left-1, leftBounds, depth + 1 );
    subdivide ( * ( thisNode.leftchild + 1 ), left, end, rightBounds, depth + 1 );
  }
}

void BIH::subdivide2 ( BihNode &thisNode, unsigned int start, unsigned int end, const float *currBounds, unsigned int depth ) {
  assert ( end < triangles.size() );
  assert ( start <= end );
  // determine if we hit a termination condition
  if ( ( ( end - start ) < minimalPrimitiveCount )
       || ( depth > maxDepth ) ) {
    thisNode.type = 3;
    thisNode.leafContent[0] = start;
    thisNode.leafContent[1] = end;
    statsinc(leafcounts);
    return; // Yeah! we created a leaf !
  }

  // determine longest axis of bounding box
  const float bbLength[3] = {
                              currBounds[1] - currBounds[0],
                              currBounds[3] - currBounds[2],
                              currBounds[5] - currBounds[4]
                            };
  unsigned char axis;
  if ( bbLength[0] > bbLength[1] )
    axis = 0;
  else
    axis = 1;
  if ( bbLength[2] > bbLength[axis] )
    axis = 2;

  // Split the resulting axis in half
  const float splitVal = currBounds[axis * 2] + ( bbLength[axis] * 0.5 );

  unsigned int left = start;
  unsigned int right = end;
  // Good ole quicksortlike partitioning
  do {

    while ( left < right && triangles[triangleIndices[left]].getCenter().value[axis] <= splitVal ) {
      ++left;
    }

    while ( right > left && triangles[triangleIndices[right]].getCenter().value[axis] > splitVal ) {
      --right;
    }

    if ( left < right ) {
      unsigned int tmp = triangleIndices[left];
      triangleIndices[left] = triangleIndices[right];
      triangleIndices[right] = tmp;
    }

  } while ( left < right );

  if ( triangles[triangleIndices[right]].getCenter().value[axis] < triangles[triangleIndices[left]].getCenter().value[axis] ) {
    unsigned int tmp = triangleIndices[left];
    triangleIndices[left] = triangleIndices[right];
    triangleIndices[right] = tmp;
  }

  // split the current bounding box in two along the current axis
  float leftBounds[6], rightBounds[6];
  switch ( axis ) {
    case 0: leftBounds[0] = currBounds[0];
      leftBounds[1] = splitVal ;
      memcpy ( leftBounds+2, currBounds+2,  4 * sizeof ( float ) );

      rightBounds[0] = splitVal;
      rightBounds[1] = currBounds[1];
      memcpy ( rightBounds+2, currBounds+2, 4 * sizeof ( float ) );
      break;
    case 1: leftBounds[2] = currBounds[2];
      leftBounds[3] = splitVal ;
      leftBounds[0] = currBounds[0];
      leftBounds[1] = currBounds[1];
      leftBounds[4] = currBounds[4];
      leftBounds[5] = currBounds[5];

      rightBounds[2] = splitVal;
      rightBounds[3] = currBounds[3];
      rightBounds[0] = currBounds[0];
      rightBounds[1] = currBounds[1];
      rightBounds[4] = currBounds[4];
      rightBounds[5] = currBounds[5];
      break;
    case 2: leftBounds[4] = currBounds[4];
      leftBounds[5] = splitVal ;
      memcpy ( leftBounds, currBounds, 4 * sizeof ( float ) );
      rightBounds[4] = splitVal;
      rightBounds[5] = currBounds[5];
      memcpy ( rightBounds, currBounds, 4 * sizeof ( float ) );
      break;
  }

  if ( left == end + 1 )
    subdivide ( thisNode, start, end, leftBounds, depth );
  else if ( left == start )
    subdivide ( thisNode, start, end, rightBounds, depth );
  else {
    float leftMax = -UNENDLICH;
    float rightMin = UNENDLICH;
    for ( unsigned int i = start; i < left; ++i ) {
      for ( unsigned char c = 0; c < 3; ++c )
        if ( triangles[triangleIndices[i]].getPoint ( c ).value[axis] > leftMax )
          leftMax = triangles[triangleIndices[i]].getPoint ( c ).value[axis];
    }

    for ( unsigned int i = left; i <= end; ++i ) {
      for ( unsigned char c = 0; c < 3; ++c )
        if ( triangles[triangleIndices[i]].getPoint ( c ).value[axis] < rightMin )
          rightMin = triangles[triangleIndices[i]].getPoint ( c ).value[axis];
    }

    thisNode.leftchild = nodes.getNextFreePair();
    thisNode.planes[0] = leftMax;
    thisNode.planes[1] = rightMin;

    thisNode.type = axis;
    subdivide ( *thisNode.leftchild, start, left-1, leftBounds, depth );
    subdivide ( * ( thisNode.leftchild + 1 ), left, end, rightBounds, depth );
  }
}

bool BIH::isConsistent() {
  return checkConsistency ( &nodes.get ( 0 ) );
}

bool BIH::checkConsistency ( BihNode *node ) {
  if ( node->type > 3 )
    return false;

  unsigned int max = triangles.size();
  if ( node->type == 3 ) {
    return ( node->leafContent[1] - node->leafContent[0] <= minimalPrimitiveCount ) && ( node->leafContent[1] < max ) && ( node->leafContent[0] <= ( node->leafContent[1] ) );
  }

  if ( !node->leftchild )
    return false;
  return checkConsistency ( node->leftchild ) && checkConsistency ( node->leftchild + 1 );

}

unsigned int BIH::depth(BihNode &node) {
  if ( node.type == 3)
    return 1;
  const int left = depth(node.leftchild[0]);
  const int right = depth(node.leftchild[1]);
  return ((left > right) ? left : right) + 1;
}

unsigned int BIH::leafcount(BihNode &node) {
  if ( node.type == 3)
    return 1;
  return leafcount(node.leftchild[0]) + leafcount(node.leftchild[1]);
}

void BIH::analyze() const {
  std::cout << "Tree depth:" << depth( nodes.get( 0 ) ) << "\n";
  const unsigned int lc = leafcount( nodes.get( 0 ) );
  std::cout << "Leafcount:" << lc << "\n";

  std::cout << "Triangles per leaf:" << ( (float)nodes.size()/lc);

  std::cout << std::endl;
}
