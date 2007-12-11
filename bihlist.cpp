//
// C++ Implementation: bihlist
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bihlist.h"
#include "assert.h"

BihList::BihList(const Scene& scene): AccelerationStruct ( scene ), triangleIndices(0),
maxDepth(64), minimalPrimitiveCount(3) {}


BihList::~BihList() {
  if ( triangleIndices ) {
    free(triangleIndices);
  }
}

const RGBvalue BihList::trace ( Ray& r, unsigned int depth ) {

/*
 * Ray-box intersection using IEEE numerical properties to ensure that the
 * test is both robust and efficient, as described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 *      * slightly altered to find poin of intersection *
 */
  float t0 = 0.0;
  float t1 = UNENDLICH;
  float tmin, tmax, tymin, tymax, tzmin, tzmax;
  Vector3D parameters[2] = {Vector3D(bounds[0], bounds[2], bounds[4]), 
                            Vector3D(bounds[1], bounds[3], bounds[5])};  
  const Vector3D &inv_direction = r.getInvDirection();
  int sign[3];
  sign[0] = (inv_direction.value[0] < 0);
  sign[1] = (inv_direction.value[1] < 0);
  sign[2] = (inv_direction.value[2] < 0);

  tmin = (parameters[sign[0]].value[0] - r.getStart().value[0]) * inv_direction.value[0];
  tmax = (parameters[1-sign[0]].value[0] - r.getStart().value[0]) * inv_direction.value[0];
  tymin = (parameters[sign[1]].value[1] - r.getStart().value[1]) * inv_direction.value[1];
  tymax = (parameters[1-sign[1]].value[1] - r.getStart().value[1]) * inv_direction.value[1];
  if ( (tmin > tymax) || (tymin > tmax) ) {
    return RGBvalue(0.0, 0.0, 0.0);
  }
  if (tymin > tmin)
    tmin = tymin;
  if (tymax < tmax)
    tmax = tymax;
  tzmin = (parameters[sign[2]].value[2] - r.getStart().value[2]) * inv_direction.value[2];
  tzmax = (parameters[1-sign[2]].value[2] - r.getStart().value[2]) * inv_direction.value[2];
  if ( (tmin > tzmax) || (tzmin > tmax) ) { 
    return RGBvalue(0.0, 0.0, 0.0);
  }
  if (tzmin > tmin)
    tmin = tzmin;
  if (tzmax < tmax)
    tmax = tzmax;
  if ( (tmin < t1) && (tmax > t0) ) {
  ;
  } else    
    return RGBvalue(0.0, 0.0, 0.0);



  BihNode *node = & ( bihNodes[0] );

  float tNear;
  float tFar;
  Stacknode *stack = 0;
  unsigned int near=0, far=1;
  Stacknode *discard = 0;
  bool hit = false;
  IntersectionResult ir;
  int axis;
  BihNode tempNode;

  do {
    while ((axis = (node->axis) & 3 ) != 3 ) {
      // check ray direction to determine identity of 'near' and 'far' children
      near=0, far=1;
      if ( r.getDirection().value[axis] < 0.0f ) {
        // near is right, far is left
        near = 1; far = 0;
      }
      // compute intersection with near split
      tNear = ( node->planes[near] - r.getStart().value[axis] ) * inv_direction.value[axis];
      tFar = ( node->planes[far] - r.getStart().value[axis] )  * inv_direction.value[axis];
      tempNode.nextLeft = node->nextLeft;
      tempNode.axis &= ~3;
      if ( tNear <= tmin ) { //Near side intersected?
        // compute intersection with far split
        if ( tFar >= tmax ) { //Both sides intersected
          stack = new Stacknode ( fmaxf ( tNear,tmax ), tmax, tempNode.nextLeft + far, stack );
        }
        node = tempNode.nextLeft + near;
        if ( tmin < tFar )
          tFar = tmin;
      } else if ( tFar >= tmax ) { //Far side only
        node = tempNode.nextLeft + far;
        if ( tmax > tNear )
          tNear = tmax;
      } else if (stack != 0) {//Ray is in empty space (between the planes)
        //pop from stack
        node = stack->node;
        tmax = stack->tmax;
        tmin = stack->tmin;
        discard = stack;
        stack = stack->prev;
        delete discard;
      }
    }

    // intersect all triangles in leaf
    for ( unsigned int i = node->leafContent[0]; i <= node->leafContent[1] ; ++i ) {
      if (triangles[triangleIndices[i]].intersect(r, ir)) {
        hit = true;
        ir.triangleIdx = triangleIndices[i];
      }
    }
    if ( stack != 0 ) {
      //pop from stack
      node = stack->node;
      tmax = stack->tmax;
      tmin = stack->tmin;
      discard = stack;
      stack = stack->prev;
      delete discard;
    }
  } while ( stack != 0 );
  if ( hit ) {
        Triangle& hitTriangle = triangles[ir.triangleIdx];
        Vector3D n ( hitTriangle.getNormalAt ( ir ) );
        RGBvalue lightVessel ( 0.0, 0.0, 0.0 );
        const PhongMaterial& mat = hitTriangle.getMaterial();
        const std::vector<Light> lights = scene.getLights();
        std::vector<Light>::const_iterator it;
        for ( it = lights.begin(); it!=lights.end(); ++it ) {
            const Light& light = *it;

            Vector3D l ( light.getPosition() - ir.calcPOI() );
            l.normalize();
            float dif = n * l;
            //dif = 1.0;
            if (dif > 0.0)
                lightVessel.add ( dif * mat.diffuse[0] * light.getColor().getRGB()[0],
                                  dif * mat.diffuse[1] * light.getColor().getRGB()[1],
                                  dif * mat.diffuse[2] * light.getColor().getRGB()[2] );
        }
        return lightVessel;
    } else
        return  RGBvalue ( 0, 0, 0 );
}

void BihList::construct() {
  const unsigned int objectCount = triangles.size();
  triangleIndices =(unsigned int *) malloc(objectCount * sizeof(unsigned int));
  for ( unsigned int i = 0 ; i < objectCount ; ++i )
    triangleIndices[i] = i;
    // my first (porperbly bad guess on how many nodes we need )
  bihNodes.resize( ( ( (int)(objectCount / minimalPrimitiveCount) ) + 1 ) * 3);
  bihNodes.push_back(BihNode());
  subdivide (&bihNodes[0], 0, objectCount-1, bounds, 0);
}


void BihList::subdivide(BihNode *thisNode, unsigned int start, unsigned int end, const float *currBounds, unsigned int depth) {
  std::cout << "depth:" << depth << std::endl;
  assert(end <= triangles.size() );
  assert(start < end );
  // determine if we hit a termination condition
  if  ( ( (end - start) < minimalPrimitiveCount)
    || ( depth > maxDepth ) ) {
    thisNode->axis = 3;
    thisNode->leafContent[0] = start;
    thisNode->leafContent[1] = end;
    return; // Yeah! we created a leaf !
  }
  
  // determine longest axis of bounding box
  const float bbLength[3] = { currBounds[1] - currBounds[0],
                               currBounds[3] - currBounds[2],
                               currBounds[5] - currBounds[4]};
  unsigned char axis;
  if ( bbLength[0] > bbLength[1] )
    axis = 0;
  else
    axis = 1;
  if ( bbLength[2] > bbLength[axis] )
    axis = 2;


  // Split the resulting axis in half
  const float splitVal = currBounds[axis * 2] + ( bbLength[axis] * 0.5 );

  unsigned int leftStart = start;
  unsigned int leftEnd = start;
  
  unsigned int rightStart = end;
  unsigned int rightEnd = end;
  float leftMax = -UNENDLICH;
  float rightMin = UNENDLICH;
  // Sort the segment of the list passed to the function ( by start/end ) according to splitVal
  while ( leftEnd < (rightStart-1) ) { // -1 to avoid overlapping

    // distinguish 4 cases
    // 0 = both are on the correct side
    // 1 & 2 = only leftEnd or RightStart are wrong -> in-/decrement the other
    // 3 = both values (leftEnd and rightStart are on the 'wrong' side -> swap
    char swapCase = (( triangles[triangleIndices[leftEnd   ]].getCenter().value[axis] > splitVal) << 1 )
                  | ( triangles[triangleIndices[rightStart]].getCenter().value[axis] < splitVal) ;
    unsigned int swapVal;
    switch (swapCase ) {
      case 0: ++leftEnd; --rightStart; break;
      case 1: ++leftEnd;               break;
      case 2: --rightStart;            break;
      case 3: swapVal = triangleIndices[leftEnd];
              triangleIndices[leftEnd] = triangleIndices[rightStart];
              triangleIndices[rightStart] = swapVal;

              ++leftEnd; --rightStart; break;
    }
    for ( unsigned char trii = 0; trii < 3 ; ++trii ) {
      if ( triangles[triangleIndices[rightStart]].getPoint ( trii ).value[axis] < rightMin )
        rightMin = triangles[triangleIndices[rightStart]].getPoint ( trii ).value[axis];
      if ( triangles[triangleIndices[leftEnd]].getPoint ( trii ).value[axis] > leftMax )
        leftMax = triangles[triangleIndices[leftEnd]].getPoint ( trii ).value[axis];
    }
  }
  
  // in case of even num of primitives
  if ( rightStart == leftEnd) {
    if (triangles[triangleIndices[rightStart]].getCenter().value[axis] > splitVal)
      --leftEnd;
    else
      ++rightStart;
  // in case of odd number of odd number of primitives we have
  // the 'middle' lement not sorted into one of the lists
  } else if ( (rightStart - leftEnd) > 1)
    if (triangles[triangleIndices[rightStart - 1]].getCenter().value[axis] > splitVal)
      --rightStart;
    else
      ++leftEnd;
  assert(rightStart == leftEnd + 1 );

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
  if ( leftEnd == leftStart ) {// no primitives in left half => split the right side again
    subdivide(thisNode, start, end, rightBounds, depth);
  } else if ( rightEnd == rightStart ) {// no primitives in right half=> split the left side again
    subdivide(thisNode, start, end, leftBounds, depth);
  } else { // we successfully split the volume along axis into two valid lists !
    // now we've actually work to do -> create 2 new nodes for corresponding halfs
    bihNodes.resize(bihNodes.size() + 2); // pass on the pointers to the two newly created nodes
//     thisNode->nextLeft = (BihNode*) &(bihNodes[bihNodes.size()-2]);
    thisNode->nextLeft = (BihNode*)&(bihNodes[bihNodes.size()-2]) ;
    thisNode->axis |= (int)axis;
    thisNode->planes[0] = leftMax;
    thisNode->planes[1] = rightMin;
    subdivide( &(bihNodes[bihNodes.size()-2]), leftStart, leftEnd, leftBounds, depth + 1);
    subdivide( &(bihNodes[bihNodes.size()-1]), rightStart, rightEnd, rightBounds, depth + 1);
  }
}
