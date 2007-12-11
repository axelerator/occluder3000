//
// C++ Implementation: boundaryintervalhierarchy
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "boundaryintervalhierarchy.h"
#include <assert.h>

BoundaryIntervalHierarchy::BoundaryIntervalHierarchy ( const Scene& scene )
    : AccelerationStruct ( scene ) {}


BoundaryIntervalHierarchy::~BoundaryIntervalHierarchy() {}


const RGBvalue BoundaryIntervalHierarchy::trace ( Ray& r, unsigned int depth ) {
  IntersectionResult ir;
  static float tmin = 0.1;
  static float tmax = 10.0;
  if ( root->traverse ( r, ir, tmin, tmax ) ) {
        const Triangle& hitTriangle = triangles[ir.triangleIdx];
        Vector3D n ( hitTriangle.getNormalAt (ir ) );
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

bool BIHInternalNode::traverse ( Ray& r, IntersectionResult& ir, float tmin, float tmax ) {

  // check ray direction to determine identity of 'near' and 'far' children
  int near=0, far=1;
  if ( r.getDirection().value[axis] < 0.0f ) {
    // near is right, far is left
    near = 1; far = 0;
  }

  // compute intersection with near split
  float tnear = ( clip[near] - r.getStart().value[axis] ) / r.getDirection().value[axis];

  // compute intersection with far split
  float tfar = ( clip[far] - r.getStart().value[axis] ) / r.getDirection().value[axis];


  // if tmin >= near split, tmin = max(tmin,far split), traverse far child
  // else if tmax < farsplit, tmax = min( tmax, nearrsplit ), traverse near child
  // else traverse both
  bool bhit = false;
  if ( tmin >= tnear ) {
    tmin = fmaxf ( tmin, tfar );
    bhit = children[far]->traverse ( r, ir, tmin, tmax );
  } else if ( tmax < tfar ) {
    tmax = fminf ( tmax, tnear );
    bhit = children[near]->traverse ( r, ir, tmin, tmax );

  } else {
    bhit = children[near]->traverse ( r, ir, tmin, fminf ( tmax, tnear ) );
//     bhit = children[near]->traverse ( r, ir, tmin,tmax );
    bhit = children[far]->traverse ( r, ir, fmaxf ( tmin, tfar ), tmax ) || bhit;
//     bhit = children[far]->traverse ( r, ir, tmin, tmax ) || bhit;
  }

  return bhit;

}

bool BIHLeaf::traverse ( Ray& r, IntersectionResult& ir, float tmin, float tmax ) {
  if ( b->getTriangle ( index ).intersect ( r, ir ) ) {
    ir.triangleIdx = index;
    return true;
  }
  return false;
}

void BoundaryIntervalHierarchy::construct() {
  BIHLeaf::b = this;
  const unsigned int objectCount = triangles.size();
  std::vector<int> triangleIndices ( objectCount );
  for ( unsigned int i = 0 ; i < objectCount ; ++i )
    triangleIndices[i] = i;
  root = subdivide ( bounds, triangleIndices );

}
/**
 * To test consisteny: Checks if objects are inside given bounds
 */
bool BoundaryIntervalHierarchy::testObjctsVsBB ( float *cbounds, std::vector<int> &objects ) {
  std::vector<int>::iterator iter = objects.begin();
  while ( iter != objects.end() ) {
    const Triangle& tri = triangles[*iter];
    for ( unsigned char i=0; i < 3; ++i ) // check all 3 points of triangle
      for ( unsigned char c=0; c < 3; ++c ) // check all 3 components of point
        if ( tri.getPoint ( i ).value[c] < cbounds[2*c] || tri.getPoint ( i ).value[c] > cbounds[2*c+1] )
          return false;
    ++iter;
  }
  return true;
}

BIHTreeNode *BoundaryIntervalHierarchy::subdivide ( float *currBounds, std::vector<int> &objects, unsigned int depth ) {
  assert ( objects.size() > 0 );
//   assert(testObjctsVsBB(currBounds, objects));
  if ( objects.size() == 1 )
    return new BIHLeaf ( objects[0] );

  const float boundLength[3] = { currBounds[1] - currBounds[0],  currBounds[3] - currBounds[2],  currBounds[5] - currBounds[4]};
  unsigned char axis;
  // determine longest axis of bounding box
  if ( boundLength[0] > boundLength[1] )
    axis = 0;
  else
    axis = 1;
  if ( boundLength[2] > boundLength[axis] )
    axis = 2;

  unsigned char boundsIdx = 2 * axis;
  const float seperatePlane = currBounds[boundsIdx] + boundLength[axis] * 0.5;
  /*  float leftMax = currBounds[boundsIdx];
    float rightMin = currBounds[boundsIdx + 1];*/
  float leftMax = -INFINITY;
  float rightMin = INFINITY;

  unsigned char charIterator;

  std::vector<int> leftObjects;
  std::vector<int> rightObjects;
  std::vector<int>::const_iterator i;
  for ( i = objects.begin(); i != objects.end(); ++i ) {
    const Triangle& triangle = triangles[*i];
    if ( triangle.getCenter().value[axis] < seperatePlane ) {
      leftObjects.push_back ( *i );
      for ( charIterator = 0; charIterator < 3; ++ charIterator )
        if ( triangle.getPoint ( charIterator ).value[axis] > leftMax )
          leftMax = triangle.getPoint ( charIterator ).value[axis];
    } else {
      rightObjects.push_back ( *i );
      for ( charIterator = 0; charIterator < 3; ++ charIterator )
        if ( triangle.getPoint ( charIterator ).value[axis] < rightMin )
          rightMin = triangle.getPoint ( charIterator ).value[axis];
    }
  }

  // split the current bounding box in two along the current axis
  float leftBounds[6], rightBounds[6];
  switch ( axis ) {
    case 0: leftBounds[0] = currBounds[0];
      leftBounds[1] = seperatePlane ;
      memcpy ( leftBounds+2, currBounds+2,  4 * sizeof ( float ) );

      rightBounds[0] = seperatePlane;
      rightBounds[1] = currBounds[1];
      memcpy ( rightBounds+2, currBounds+2, 4 * sizeof ( float ) );
      break;
    case 1: leftBounds[2] = currBounds[2];
      leftBounds[3] = seperatePlane ;
      leftBounds[0] = currBounds[0];
      leftBounds[1] = currBounds[1];
      leftBounds[4] = currBounds[4];
      leftBounds[5] = currBounds[5];

      rightBounds[2] = seperatePlane;
      rightBounds[3] = currBounds[3];
      rightBounds[0] = currBounds[0];
      rightBounds[1] = currBounds[1];
      rightBounds[4] = currBounds[4];
      rightBounds[5] = currBounds[5];
      break;
    case 2: leftBounds[4] = currBounds[4];
      leftBounds[5] = seperatePlane ;
      memcpy ( leftBounds, currBounds, 4 * sizeof ( float ) );
      rightBounds[4] = seperatePlane;
      rightBounds[5] = currBounds[5];
      memcpy ( rightBounds, currBounds, 4 * sizeof ( float ) );
      break;
  }
  assert ( leftObjects.size()  + rightObjects.size() > 0 );
  // in case one half is empty will repeat the subdivision step on the other half
  if ( leftObjects.size() == 0 ) {
    return subdivide ( rightBounds, rightObjects, depth + 1 );
  } else if ( rightObjects.size() == 0 ) {
    return subdivide ( leftBounds, leftObjects, depth + 1 );
  }

  // when both nodes contain at least one object return a internal node
  BIHInternalNode *result = new BIHInternalNode ( axis, leftMax, rightMin, currBounds );
  result->children[0] = subdivide ( leftBounds, leftObjects, depth + 1 );
  result->children[1] = subdivide ( rightBounds, rightObjects, depth + 1 );
  return result;
}

void BoundaryIntervalHierarchy::drawBB ( float *bounds ) {
  glBegin ( GL_LINE_LOOP );
  glVertex3f ( bounds[0], bounds[2], bounds[4] );
  glVertex3f ( bounds[0], bounds[3], bounds[4] );
  glVertex3f ( bounds[1], bounds[3], bounds[4] );
  glVertex3f ( bounds[1], bounds[2], bounds[4] );
  glEnd();
  glBegin ( GL_LINE_LOOP );
  glVertex3f ( bounds[0], bounds[2], bounds[5] );
  glVertex3f ( bounds[0], bounds[3], bounds[5] );
  glVertex3f ( bounds[1], bounds[3], bounds[5] );
  glVertex3f ( bounds[1], bounds[2], bounds[5] );
  glEnd();

  glBegin ( GL_LINES );
  glVertex3f ( bounds[0], bounds[2], bounds[4] );
  glVertex3f ( bounds[0], bounds[2], bounds[5] );
  glVertex3f ( bounds[0], bounds[3], bounds[4] );
  glVertex3f ( bounds[0], bounds[3], bounds[5] );
  glVertex3f ( bounds[1], bounds[3], bounds[4] );
  glVertex3f ( bounds[1], bounds[3], bounds[5] );
  glVertex3f ( bounds[1], bounds[2], bounds[4] );
  glVertex3f ( bounds[1], bounds[2], bounds[5] );

  glEnd();
}

/*!
    \fn BoundaryIntervalHierarchy::drawGL()
 */
void BoundaryIntervalHierarchy::drawGL() {
  root->drawGL();

}

BoundaryIntervalHierarchy *BIHLeaf::b ;
