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

#define STACKDEPTH 512

BIH::BIH ( const Scene& scene )
    : AccelerationStruct ( scene ), triangleIndices ( 0 ), minimalPrimitiveCount ( 3 ), maxDepth ( 64 ),
     markednode ( 0 ) {}


BIH::~BIH() {
}

void BIH::traverseIterative (RadianceRay& r ) {

  Stack stack[STACKDEPTH];
  int stackpos = 1;
  float tmin,tmax, tNear, tFar;
  stack[0].node = &nodes.get(0);
  stack[0].tmin = r.getMin();
  stack[0].tmax = r.getMax();
  const BihNode *node;
  unsigned int near, far, nearP[3] = { 0, 0, 0 }, farP[3] = { 1, 1, 1 }, i;
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
      Triangle& hitTriangle = triangles[triangleIndices[i]];
      hitTriangle.intersect ( r );
    }
  }  
}

bool BIH::isBlocked(Ray& r) {
  return traverseShadow(r);
}

bool BIH::traverseShadow ( Ray& r ) {
  Stack stack[STACKDEPTH];
  int stackpos = 1;
  float tmin,tmax, tNear, tFar;
  stack[0].node = &nodes.get(0);
  stack[0].tmin = r.getMin();
  stack[0].tmax = r.getMax();
  const BihNode *node;
  unsigned int near, far, nearP[3] = { 0, 0, 0 }, farP[3] = { 1, 1, 1 }, i;
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
      Triangle& hitTriangle = triangles[triangleIndices[i]];
      if (hitTriangle.intersect ( r ))
        return true;
    }
  }  
  return false;
}

const RGBvalue BIH::trace ( RadianceRay& r, unsigned int depth ) {
  if ( !trimRaytoBounds( r ) )
    return RGBvalue( 0.0, 0.0, 0.0 );

  RGBvalue result ( 0.0, 0.0, 0.0 );
  traverseIterative ( r );
  r.shade(result, depth);    
  return result;
}

const Intersection&  BIH::getClosestIntersection(RadianceRay& r) {
  traverseIterative ( r );
  return r.getClosestIntersection();
}

void BIH::recurse(RayPacket& rp) {
  Stack stack[STACKDEPTH];
  int stackpos = 1;
  static unsigned int idx[] = { 0, rp.getPacketWidth()-1, rp.getRayCount() - rp.getPacketWidth(), rp.getRayCount()-1 };
  const BihNode *node = 0;
  float tmin, tmax, tNear, tFar, pnear, pfar;
  stack[0].node = &nodes.get(0);
  stack[0].tmin = fminf(fminf(fminf( rp.getMin(idx[0]), rp.getMin(idx[1])), rp.getMin(idx[2])),rp.getMin(idx[3]));
  stack[0].tmax = fmaxf(fmaxf(fmaxf( rp.getMax(idx[0]), rp.getMax(idx[1])), rp.getMax(idx[2])),rp.getMax(idx[3]));
//  const BihNode *node;
  unsigned int near, far, nearP[3] = { 0, 0, 0 }, farP[3] = { 1, 1, 1 }, i;
  // check ray direction to determine identity of 'near' and 'far' children
  for ( unsigned int i = 0; i < 3; ++i )
    if ( rp.getDirection(0).value[i] < 0.0f ) 
      // near is right, far is left
      nearP[i] = 1, farP[i] = 0;
  while ( --stackpos > -1 ) {

    const Stack &current = stack[stackpos];
    // pop from stack
    tmin = current.tmin;
    tmax = current.tmax;
    node = current.node;

    while ( node->type != 3 ) {
      near = 0, far = 1;
      if ( rp.getDirection(0).value[node->type] < 0.0f )  
        // near is right, far is left
        near = 1, far = 0;
      pnear = node->planes[near] - rp.getOrigin().value[node->type];
      pfar = node->planes[far] - rp.getOrigin().value[node->type];
      tNear = pnear * rp.getInvDirection(0).value[node->type];
      tFar  = pfar  * rp.getInvDirection(0).value[node->type];
      
      for ( unsigned int i = 1; i < 4; ++i ) {
        tNear = fmaxf(tNear, pnear * rp.getInvDirection(idx[i]).value[node->type]);
        tFar  = fminf(tFar, pfar  * rp.getInvDirection(idx[i]).value[node->type]);
      }    
    
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
      Triangle& hitTriangle = triangles[triangleIndices[i]];
      hitTriangle.intersect ( rp );
    }
  }  
}

bool BIH::trace ( RayPacket& rp, unsigned int depth ) {
  if ( !trimRaytoBounds(rp) )
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
//   std::cout << "construction done. Nodecount:" << nodes.size() << std::endl;
//   std::cout << "consistency check: " << ( isConsistent() ?"true":"false" ) << std::endl;
}

void BIH::subdivide ( BihNode &thisNode, unsigned int start, unsigned int end, const float *currBounds, unsigned int depth ) {
  assert ( end < triangles.size() );
  assert ( start <= end );
  // determine if we hit a termination condition
  if ( ( ( end - start ) < minimalPrimitiveCount )
       || ( depth > maxDepth ) ) {
    thisNode.type = 3;
    thisNode.leafContent[0] = start;
    thisNode.leafContent[1] = end;
    return; // Yeah! we created a leaf !
  }

  // determine longest axis of bounding box
  const float bbLength[3] = {  currBounds[1] - currBounds[0],
                               currBounds[3] - currBounds[2],
                               currBounds[5] - currBounds[4] };
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
    subdivide ( *(thisNode.leftchild + 1), left, end, rightBounds, depth );
  }
}

bool BIH::isConsistent() {
  return checkConsistency ( &nodes.get(0) );
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


