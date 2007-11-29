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

BIH2::BIH2 ( const Scene& scene )
    : AccelerationStruct ( scene ), triangleIndices ( 0 ), minimalPrimitiveCount ( 3 ), maxDepth ( 64 ),
    reserved ( 0 ), occupied ( 0 ), markednode ( 0 ) {}


BIH2::~BIH2() {
  free ( nodes );
}

void BIH2::traverse ( const BihNode& node, RadianceRay& r, fliess tmin, fliess tmax, unsigned int depth) {
  if ( node.type == 3 ) {
    IntersectionResult ir;
    for ( unsigned int i = node.leafContent[0]; i <= node.leafContent[1]; ++i) {
      Triangle& hitTriangle = triangles[triangleIndices[i]];
       hitTriangle.intersect(r);

    }
  } else {
      // check ray direction to determine identity of 'near' and 'far' children
      unsigned int near=0, far=1;
      if ( r.getDirection() [node.type] < 0.0f ) {
        // near is right, far is left
        near = 1; far = 0;
      }
    
    fliess tNear = ( node.planes[near] - r.getStart().value[node.type] ) * r.getInvDirection().value[node.type];
    fliess tFar = ( node.planes[far] - r.getStart().value[node.type] )  * r.getInvDirection().value[node.type];
    if ( tmin > tNear ) {
      tmin = Fliess::max( tmin, tFar );
      traverse(*node.children[far], r, tmin, tmax, depth + 1  );
    } else
    if ( tmax  < tFar ) {
      tmax = Fliess::min( tmax, tNear );
      traverse(*node.children[near], r, tmin, tmax, depth + 1  );
    } else {
      traverse(*node.children[near], r, tmin, Fliess::min( tmax, tNear ) , depth + 1 );
      traverse(*node.children[far],  r, Fliess::max ( tmin, tFar ), tmax, depth + 1 );
    }
  }
}

void BIH2::traverseIterative ( const BihNode* startnode, RadianceRay& r, fliess tmin, fliess tmax) {
  
  Stack stack[256];
  int stackpos = 1;
  stack[0].node = startnode;
  stack[0].tmin = tmin;
  stack[0].tmax = tmax;
  const BihNode *node;
  while (--stackpos > -1 ) {
  
  // pop from stack
  Stack &current = stack[stackpos];
  tmin = current.tmin;
  tmax = current.tmax;
  node = current.node;
  
    if ( node->type == 3 ) {
      IntersectionResult ir;
      for ( unsigned int i = node->leafContent[0]; i <= node->leafContent[1]; ++i) {
        Triangle& hitTriangle = triangles[triangleIndices[i]];
        hitTriangle.intersect(r);
  
      }
    } else {
        // check ray direction to determine identity of 'near' and 'far' children
        unsigned int near=0, far=1;
        if ( r.getDirection() [node->type] < 0.0f ) {
          // near is right, far is left
          near = 1; far = 0;
        }
      
      fliess tNear = ( node->planes[near] - r.getStart().value[node->type] ) * r.getInvDirection().value[node->type];
      fliess tFar = ( node->planes[far] - r.getStart().value[node->type] )  * r.getInvDirection().value[node->type];
      if ( tmin > tNear ) {
        tmin = Fliess::max( tmin, tFar );
        stack[stackpos].tmin = tmin;
        stack[stackpos].tmax = tmax;
        stack[stackpos].node = node->children[far];
        ++stackpos;
      } else if ( tmax  < tFar ) {
        tmax = Fliess::min( tmax, tNear );
        stack[stackpos].tmin = tmin;
        stack[stackpos].tmax = tmax;
        stack[stackpos].node = node->children[near];
        ++stackpos;
      } else {
        stack[stackpos].tmin = tmin;
        stack[stackpos].tmax = tmax;
        stack[stackpos].node = node->children[near];
        ++stackpos;
        stack[stackpos].tmin = tmin;
        stack[stackpos].tmax = tmax;
        stack[stackpos].node = node->children[far];
        ++stackpos;
      }
    }
  }

}

const RGBvalue BIH2::trace ( Ray& r, unsigned int depth ) {
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
  Vector3D inv_direction(1.0/r.getDirection()[0], 1.0/r.getDirection()[1], 1.0/r.getDirection()[2]);
  int sign[3];
  sign[0] = (inv_direction[0] < 0);
  sign[1] = (inv_direction[1] < 0);
  sign[2] = (inv_direction[2] < 0);

  tmin = (parameters[sign[0]][0] - r.getStart()[0]) * inv_direction[0];
  tmax = (parameters[1-sign[0]][0] - r.getStart()[0]) * inv_direction[0];
  tymin = (parameters[sign[1]][1] - r.getStart()[1]) * inv_direction[1];
  tymax = (parameters[1-sign[1]][1] - r.getStart()[1]) * inv_direction[1];
  if ( (tmin > tymax) || (tymin > tmax) ) {
    return RGBvalue(0.0, 0.0, 0.0);
  }
  if (tymin > tmin)
    tmin = tymin;
  if (tymax < tmax)
    tmax = tymax;
  tzmin = (parameters[sign[2]][2] - r.getStart()[2]) * inv_direction[2];
  tzmax = (parameters[1-sign[2]][2] - r.getStart()[2]) * inv_direction[2];
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

  RGBvalue result(0.0, 0.0, 0.0);
  RadianceRay rr(r.getStart(), r.getDirection());
//   traverse(nodes[0], rr, tmin, tmax, 0);
  traverseIterative(nodes, rr, tmin, tmax);
  if ( rr.didHitSomething() ) {
//    return RGBvalue(1.0, 0.0, 0.0);
      const Intersection &i = rr.getClosestIntersection();
      const Triangle &hitTriangle = *(i.triangle);
      Vector3D n ( hitTriangle.getNormalAt ( i ) );
      const PhongMaterial& mat = hitTriangle.getMaterial();
      const std::vector<Light> lights = scene.getLights();
      std::vector<Light>::const_iterator it;
      for ( it = lights.begin(); it!=lights.end(); ++it ) {
          const Light& light = *it;

          Vector3D l ( light.getPosition() - i.intersectionPoint );
          l.normalize();
          fliess dif = n * l;
          //dif = 1.0;
          if (dif > 0.0)
              result.add ( dif * mat.diffuse[0] * light.getColor().getRGB()[0],
                                dif * mat.diffuse[1] * light.getColor().getRGB()[1],
                                dif * mat.diffuse[2] * light.getColor().getRGB()[2] );
      }
  }
  return result;
}


void BIH2::construct() {
  const unsigned int objectCount = triangles.size();
  triangleIndices = ( unsigned int * ) malloc ( objectCount * sizeof ( unsigned int ) );
  for ( unsigned int i = 0 ; i < objectCount ; ++i )
    triangleIndices[i] = i;

  reserved = 30;
  nodes = ( BihNode * ) malloc ( reserved * sizeof ( BihNode ) );
  occupied = 1;
  nodes[0].idx = 0;
  subdivide ( nodes[0], 0, objectCount-1, bounds, 0 );
  std::cout << "construction done. Nodecount:" << occupied << std::endl;
  std::cout << "consistency check: " << (isConsistent()?"true":"false") << std::endl;
}

void BIH2::subdivide ( BihNode &thisNode, unsigned int start, unsigned int end, const fliess *currBounds, unsigned int depth ) {
  assert ( end < triangles.size() );
  assert ( start <= end );
  memcpy ( thisNode.bounds, currBounds, 6 * sizeof ( float ) );

  // determine if we hit a termination condition
  if ( ( ( end - start ) < minimalPrimitiveCount )
       || ( depth > maxDepth ) ) {
    thisNode.type = 3;
    thisNode.leafContent[0] = start;
    thisNode.leafContent[1] = end;
    return; // Yeah! we created a leaf !
  }

  // determine longest axis of bounding box
  const fliess bbLength[3] = {
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
  const fliess splitVal = currBounds[axis * 2] + ( bbLength[axis] * 0.5 );

  unsigned int left = start;
  unsigned int right = end;

  do {

    while ( left < right && triangles[triangleIndices[left]].getCenter() [axis] <= splitVal ) {
      ++left;
    }

    while ( right > left && triangles[triangleIndices[right]].getCenter() [axis] > splitVal ) {
      --right;
    }

    if ( left < right ) {
      unsigned int tmp = triangleIndices[left];
      triangleIndices[left] = triangleIndices[right];
      triangleIndices[right] = tmp;
    }

  } while ( left < right );

  if ( triangles[triangleIndices[right]].getCenter() [axis] < triangles[triangleIndices[left]].getCenter() [axis] ) {
    unsigned int tmp = triangleIndices[left];
    triangleIndices[left] = triangleIndices[right];
    triangleIndices[right] = tmp;
  }

  // split the current bounding box in two along the current axis
  fliess leftBounds[6], rightBounds[6];
  switch ( axis ) {
    case 0: leftBounds[0] = currBounds[0];
      leftBounds[1] = splitVal ;
      memcpy ( leftBounds+2, currBounds+2,  4 * sizeof ( fliess ) );

      rightBounds[0] = splitVal;
      rightBounds[1] = currBounds[1];
      memcpy ( rightBounds+2, currBounds+2, 4 * sizeof ( fliess ) );
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
      memcpy ( leftBounds, currBounds, 4 * sizeof ( fliess ) );
      rightBounds[4] = splitVal;
      rightBounds[5] = currBounds[5];
      memcpy ( rightBounds, currBounds, 4 * sizeof ( fliess ) );
      break;
  }



  if ( left == end + 1 )
    subdivide ( thisNode, start, end, leftBounds, depth );
  else if ( left == start )
    subdivide ( thisNode, start, end, rightBounds, depth );
  else {
    if ( occupied + 2 >= reserved ) {
      reserved *= 2;
      nodes = ( BihNode * ) realloc ( nodes, reserved * sizeof ( BihNode ) );
    }
    occupied += 2;
    fliess leftMax = -UNENDLICH;
    fliess rightMin = UNENDLICH;
    for ( unsigned int i = start; i < left; ++i ) {
      for ( unsigned char c = 0; c < 3; ++c )
        if ( triangles[triangleIndices[i]].getPoint ( c ) [axis] > leftMax )
          leftMax = triangles[triangleIndices[i]].getPoint ( c ) [axis];
    }

    for ( unsigned int i = left; i <= end; ++i ) {
      for ( unsigned char c = 0; c < 3; ++c )
        if ( triangles[triangleIndices[i]].getPoint ( c ) [axis] < rightMin )
          rightMin = triangles[triangleIndices[i]].getPoint ( c ) [axis];
    }

    BihNode *leftNode = nodes + occupied - 2;
    BihNode *rightNode = nodes + occupied - 1;
    leftNode->idx = occupied - 2;
    rightNode->idx = occupied - 1;
    thisNode.planes[0] = leftMax;
    thisNode.planes[1] = rightMin;

    thisNode.type = axis;
    thisNode.children[0] = leftNode;
    thisNode.children[1] = rightNode;
    subdivide ( *leftNode, start, left-1, leftBounds, depth );
    subdivide ( *rightNode, left, end, rightBounds, depth );
  }
}


#ifdef VISUAL_DEBUGGER
#include "glwidget.h"

const void BIH2::drawTree ( const BihNode &node, bool drawLeaves, GLWidget *context ) const {
  if ( node.type == 3 ) {
    if ( drawLeaves || node.idx == markednode )
      for ( int i = node.leafContent[0]; i <= node.leafContent[1]; ++i ) {
        const Triangle& t = triangles[triangleIndices[i]];
        glColor3f ( 1.0, 1.0, 1.0 );
        glEnable ( GL_LIGHTING );
        t.drawGL();
        glDisable ( GL_LIGHTING );
      }
    return;
  }

  if ( node.idx == markednode ) {
    glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
    float dims[3] = { node.bounds[1] - node.bounds[0], node.bounds[3] - node.bounds[2], node.bounds[5] - node.bounds[4] };
    float center[3] = {node.bounds[0] + dims[0] * 0.5, node.bounds[2] + dims[1] * 0.5, node.bounds[4] + dims[2] * 0.5};
    context->drawMinicube ( center[0], center[1], center[2], dims );
    glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
  }

  drawTree ( * ( node.left ), drawLeaves || node.idx == markednode, context );
  drawTree ( * ( node.right ), drawLeaves || node.idx == markednode, context );
  if ( node.idx == markednode ) {
    glBegin ( GL_QUADS );
    switch ( node.type ) {
      case 0: glColor3f ( 0.3, 0.0, 0.0 );
        glVertex3f ( node.planes[0], node.bounds[2], node.bounds[4] );
        glVertex3f ( node.planes[0], node.bounds[2], node.bounds[5] );
        glVertex3f ( node.planes[0], node.bounds[3], node.bounds[5] );
        glVertex3f ( node.planes[0], node.bounds[3], node.bounds[4] );
        glColor3f ( 0.0, 0.0, 0.3 );
        glVertex3f ( node.planes[1], node.bounds[2], node.bounds[4] );
        glVertex3f ( node.planes[1], node.bounds[2], node.bounds[5] );
        glVertex3f ( node.planes[1], node.bounds[3], node.bounds[5] );
        glVertex3f ( node.planes[1], node.bounds[3], node.bounds[4] );
        break;
      case 1: glColor3f ( 0.3, 0.0, 0.0 );
        glVertex3f ( node.bounds[0], node.planes[1], node.bounds[4] );
        glVertex3f ( node.bounds[1], node.planes[1], node.bounds[4] );
        glVertex3f ( node.bounds[1], node.planes[1], node.bounds[5] );
        glVertex3f ( node.bounds[0], node.planes[1], node.bounds[5] );
        glColor3f ( 0.0, 0.0, 0.3 );
        glVertex3f ( node.bounds[0], node.planes[0], node.bounds[4] );
        glVertex3f ( node.bounds[1], node.planes[0], node.bounds[4] );
        glVertex3f ( node.bounds[1], node.planes[0], node.bounds[5] );
        glVertex3f ( node.bounds[0], node.planes[0], node.bounds[5] );
        break;
      case 2: glColor3f ( 0.3, 0.0, 0.0 );
        glVertex3f ( node.bounds[0], node.bounds[2], node.planes[0] );
        glVertex3f ( node.bounds[0], node.bounds[3], node.planes[0] );
        glVertex3f ( node.bounds[1], node.bounds[3], node.planes[0] );
        glVertex3f ( node.bounds[1], node.bounds[2], node.planes[0] );
        glColor3f ( 0.0, 0.0, 0.3 );
        glVertex3f ( node.bounds[0], node.bounds[2], node.planes[1] );
        glVertex3f ( node.bounds[0], node.bounds[3], node.planes[1] );
        glVertex3f ( node.bounds[1], node.bounds[3], node.planes[1] );
        glVertex3f ( node.bounds[1], node.bounds[2], node.planes[1] );
        break;
    }
    glEnd();
  }
}



void BIH2::draw ( GLWidget* context ) const {
  glEnable ( GL_LIGHTING );
  glColor3f ( 1.0, 1.0, 1.0 );
  drawTree ( nodes[0], false, context );
}

void BIH2::select ( int selected ) {
  markednode = selected;
  if ( selected < occupied ) {
    BihNode *n = nodes + selected;
    if ( n->type == 3 )
      std::cout  << "Leaf: triangles:" << n->leafContent[0] << " .. " << n->leafContent[1] << std::endl;
    else
      std::cout  << "Split: left:" << n->planes[0] << "  right:" << n->planes[1];
    switch ( n->type ) {
      case 0: std::cout << " x-axis";break;
      case 1: std::cout << " y-axis";break;
      case 2: std::cout << " z-axis";break;
    }
    std::cout << std::endl;

  } else {
    std::cout  << "out of range";
  }
}

static float leftMax = 0.0;
const Vector3D BIH2::drawTreeWithNames ( const BihNode &node, int depth, GLWidget* context ) const {
  if ( node.type == 3 ) {
    glPushName ( node.idx );
    Vector3D pos ( leftMax += 0.06, 1.0 - depth * 0.04, 0.0 );
    context->drawMinicube ( pos[0], pos[1], pos[2], 0.02 );
    glPopName();
    return pos;
  }

  Vector3D currLeft = drawTreeWithNames ( * ( node.left ), depth + 1, context );
  Vector3D currRight = drawTreeWithNames ( * ( node.right ), depth + 1, context );
  float xpos = ( currLeft[0] + currRight[0] ) * 0.5f;
  glPushName ( node.idx );
  Vector3D nodePos = Vector3D ( xpos , ( float ) ( 1.0 - depth * 0.04f ), ( float ) 0.0f );
  context->drawMinicube ( nodePos[0], nodePos[1], nodePos[2], 0.02 );
  glPopName();
  glBegin ( GL_LINES );
  glVertex3fv ( nodePos.value );
  glVertex3fv ( currLeft.value );
  glVertex3fv ( nodePos.value );
  glVertex3fv ( currRight.value );
  glEnd();
  return nodePos;
}

void BIH2::drawWithNames ( GLWidget* context ) const {
  leftMax = 0.0;
  drawTreeWithNames ( nodes[0], 0, context );
}



const Vector3D BIH2::drawSchemaR ( const BihNode &node, int depth, GLWidget* context ) const {
  if ( node.type == 3 ) {
    if ( node.idx == markednode )
      glColor3f ( 0.2, 0.2, 1.0 );
    else
      glColor3f ( 0.2, 0.2, 0.2 );
    Vector3D pos ( leftMax += 0.06, 1.0 - depth * 0.04, 0.0 );
    context->drawMinicube ( pos[0], pos[1], pos[2], 0.02 );
    return pos;
  }

  Vector3D currLeft = drawSchemaR ( * ( node.left ), depth + 1, context );
  Vector3D currRight = drawSchemaR ( * ( node.right ), depth + 1, context );
  float xpos = ( currLeft[0] + currRight[0] ) * 0.5f;
  if ( node.idx == markednode ) {
    glColor3f ( 0.2, 0.2, 1.0 );
  } else
    glColor3f ( 0.2, 0.2, 0.2 );
  Vector3D nodePos = Vector3D ( xpos , ( float ) ( 1.0 - depth * 0.04f ), ( float ) 0.0f );
  context->drawMinicube ( nodePos[0], nodePos[1], nodePos[2], 0.02 );
  glBegin ( GL_LINES );
  glVertex3fv ( nodePos.value );
  glVertex3fv ( currLeft.value );
  glVertex3fv ( nodePos.value );
  glVertex3fv ( currRight.value );
  glEnd();
  return nodePos;
}

void BIH2::drawSchema ( GLWidget* context ) const {
  leftMax = 0.0;
  glDisable ( GL_LIGHTING );
  drawSchemaR ( nodes[0], 0, context );
}
#endif

bool BIH2::isConsistent() {

  return ( reserved > occupied ) && checkConsistency ( nodes );
}

bool BIH2::checkConsistency ( BihNode *node ) {
  if ( node->type > 3 ) 
    return false;

  unsigned int max = triangles.size();
  if ( node->type == 3 ) {
    return (node->leafContent[1] - node->leafContent[0] <= minimalPrimitiveCount  ) && ( node->leafContent[1] < max ) && ( node->leafContent[0] <= ( node->leafContent[1] ) );
  }

  if ( !node->children[0] || !node->children[1] )
    return false;
  return checkConsistency ( node->children[0] ) && checkConsistency ( node->children[1] );

}


