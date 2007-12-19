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

#define STACKDEPTH 512

BIH2::BIH2 ( const Scene& scene )
    : AccelerationStruct ( scene ), triangleIndices ( 0 ), minimalPrimitiveCount ( 3 ), maxDepth ( 64 ),
     markednode ( 0 ) {}


BIH2::~BIH2() {
}

void BIH2::traverse ( const BihNode& node, RadianceRay& r, float tmin, float tmax, unsigned int depth ) {
  if ( node.type == 3 ) {
    IntersectionResult ir;
    for ( unsigned int i = node.leafContent[0]; i <= node.leafContent[1]; ++i ) {
      Triangle& hitTriangle = triangles[triangleIndices[i]];
      hitTriangle.intersect ( r );

    }
  } else {
    // check ray direction to determine identity of 'near' and 'far' children
    unsigned int near=0, far=1;
    if ( r.getDirection().value [node.type] < 0.0f ) {
      // near is right, far is left
      near = 1; far = 0;
    }

    float tNear = ( node.planes[near] - r.getStart().value[node.type] ) * r.getInvDirection().value[node.type];
    float tFar = ( node.planes[far] - r.getStart().value[node.type] )  * r.getInvDirection().value[node.type];
    if ( tmin > tNear ) {
      tmin = fmaxf ( tmin, tFar );
      traverse ( node.leftchild[far], r, tmin, tmax, depth + 1 );
    } else
      if ( tmax  < tFar ) {
        tmax = fminf ( tmax, tNear );
        traverse ( node.leftchild[near], r, tmin, tmax, depth + 1 );
      } else {
        traverse ( node.leftchild[near], r, tmin, fminf ( tmax, tNear ) , depth + 1 );
        traverse ( node.leftchild[far],  r, fmaxf ( tmin, tFar ), tmax, depth + 1 );
      }
  }
}

void BIH2::traverseIterative (RadianceRay& r ) {

  Stack stack[STACKDEPTH];
  int stackpos = 1;
  float tmin,tmax;
  stack[0].node = &nodes.get(0);
  stack[0].tmin = r.getMin();
  stack[0].tmax = r.getMax();
  const BihNode *node;
  while ( --stackpos > -1 ) {

    // pop from stack
    Stack &current = stack[stackpos];
    tmin = current.tmin;
    tmax = current.tmax;
    node = current.node;

    if ( node->type == 3 ) {
      IntersectionResult ir;
      for ( unsigned int i = node->leafContent[0]; i <= node->leafContent[1]; ++i ) {
        Triangle& hitTriangle = triangles[triangleIndices[i]];
        hitTriangle.intersect ( r );

      }
    } else {
      // check ray direction to determine identity of 'near' and 'far' children
      unsigned int near=0, far=1;
      if ( r.getDirection().value[node->type] < 0.0f ) {
        // near is right, far is left
        near = 1; far = 0;
      }

      float tNear = ( node->planes[near] - r.getStart().value[node->type] ) * r.getInvDirection().value[node->type];
      float tFar = ( node->planes[far] - r.getStart().value[node->type] )  * r.getInvDirection().value[node->type];
      if ( tmin > tNear ) {
        tmin = fmaxf ( tmin, tFar );
        stack[stackpos].tmin = tmin;
        stack[stackpos].tmax = tmax;
        stack[stackpos].node = node->leftchild + far;
        ++stackpos;
      } else if ( tmax  < tFar ) {
        tmax = fminf ( tmax, tNear );
        stack[stackpos].tmin = tmin;
        stack[stackpos].tmax = tmax;
        stack[stackpos].node = node->leftchild + near;
        ++stackpos;
      } else {
        stack[stackpos].tmin = tmin;
        stack[stackpos].tmax = tmax;
        stack[stackpos].node = node->leftchild + near;
        ++stackpos;
        stack[stackpos].tmin = tmin;
        stack[stackpos].tmax = tmax;
        stack[stackpos].node = node->leftchild + far;
        ++stackpos;
      }
    }
  }

}

bool BIH2::isBlocked(Ray& r) {
  return traverseShadow(r);
}

bool BIH2::traverseShadow ( Ray& r ) {
  float tmin, tmax;
  Stack stack[STACKDEPTH];
  int stackpos = 1;
  stack[0].node = &nodes.get(0);
  stack[0].tmin = r.getMin();
  stack[0].tmax = r.getMax();
  const BihNode *node;
  while ( --stackpos > -1 ) {

    // pop from stack
    Stack &current = stack[stackpos];
    tmin = current.tmin;
    tmax = current.tmax;
    node = current.node;

    if ( node->type == 3 ) {
      IntersectionResult ir;
      for ( unsigned int i = node->leafContent[0]; i <= node->leafContent[1]; ++i ) {
        Triangle& hitTriangle = triangles[triangleIndices[i]];
          if ( hitTriangle.intersect ( r ) ) 
            return true;
      }
    } else {
      // check ray direction to determine identity of 'near' and 'far' children
      unsigned int near=0, far=1;
      if ( r.getDirection().value [node->type] < 0.0f ) {
        // near is right, far is left
        near = 1; far = 0;
      }

      float tNear = ( node->planes[near] - r.getStart().value[node->type] ) * r.getInvDirection().value[node->type];
      float tFar = ( node->planes[far] - r.getStart().value[node->type] )  * r.getInvDirection().value[node->type];
      if ( tmin > tNear ) {
        tmin = fmaxf ( tmin, tFar );
        stack[stackpos].tmin = tmin;
        stack[stackpos].tmax = tmax;
        stack[stackpos].node = node->leftchild + far;
        ++stackpos;
      } else if ( tmax  < tFar ) {
        tmax = fminf ( tmax, tNear );
        stack[stackpos].tmin = tmin;
        stack[stackpos].tmax = tmax;
        stack[stackpos].node = node->leftchild + near;
        ++stackpos;
      } else {
        stack[stackpos].tmin = tmin;
        stack[stackpos].tmax = tmax;
        stack[stackpos].node = node->leftchild + near;
        ++stackpos;
        stack[stackpos].tmin = tmin;
        stack[stackpos].tmax = tmax;
        stack[stackpos].node = node->leftchild + far;
        ++stackpos;
      }
    }
  }
  return false;
}

const RGBvalue BIH2::trace ( RadianceRay& r, unsigned int depth ) {
  if ( !trimRaytoBounds( r ) )
    return RGBvalue( 0.0, 0.0, 0.0 );

  RGBvalue result ( 0.0, 0.0, 0.0 );
/*  r.setMin(fmaxf(tmin, 0.0));
  r.setMax(tmax); */ 
  traverseIterative ( r );
  r.shade(result, depth);    
  return result;
}

const Intersection&  BIH2::getClosestIntersection(RadianceRay& r) {
  traverseIterative ( r );
  return r.getClosestIntersection();
}
void BIH2::construct() {
  const unsigned int objectCount = triangles.size();
  triangleIndices = ( unsigned int * ) malloc ( objectCount * sizeof ( unsigned int ) );
  for ( unsigned int i = 0 ; i < objectCount ; ++i )
    triangleIndices[i] = i;
  subdivide ( nodes.getNextFree(), 0, objectCount-1, bounds, 0 );
  std::cout << "construction done. Nodecount:" << nodes.size() << std::endl;
  std::cout << "consistency check: " << ( isConsistent() ?"true":"false" ) << std::endl;
}

void BIH2::subdivide ( BihNode &thisNode, unsigned int start, unsigned int end, const float *currBounds, unsigned int depth ) {
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

bool BIH2::isConsistent() {
  return checkConsistency ( &nodes.get(0) );
}

bool BIH2::checkConsistency ( BihNode *node ) {
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


