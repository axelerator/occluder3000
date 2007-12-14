//
// C++ Implementation: kdtree
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "kdtree.h"
#include "radianceray.h"

#define MAX_PRIMCOUNT_PER_LEAF 3



KdTree::KdTree ( const Scene &scene )
    : AccelerationStruct ( scene )
#ifdef VISUAL_DEBUGGER
    , nodeId ( 0 ), markednode ( 0 )
#endif 
{}

    void KdTree::deleteNode ( KdTreenode &node ) {
  if ( node.axis == 3 ) 
    delete  node.prims ;
}

KdTree::~KdTree() {
   deleteNode ( nodes.get(0) );
}

static int hc = 0;
bool KdTree::traverseIterative ( RadianceRay& r, float tMin, float tMax) {
  KdTreenode *stack[128];
  int stackpointer = 0;
  stack[0] = 0;
  KdTreenode *node = &nodes.get(0);
  float d;

    
  do {
    while( node->axis != 3 ) {
      d = ( node->splitPos - r.getStart().value[node->axis] ) * r.getInvDirection().value[node->axis] ;
//       if ( d <= tMin ) {
//         node = node->leftchild + ((r.getDirection().value[node->axis] < 0.0) ? 0 : 1 );
//       } else if ( d >= tMax ) {
//         node = node->leftchild + ((r.getDirection().value[node->axis] < 0.0) ? 1 : 0 ) ;
//       } else {
        if (r.getDirection().value[node->axis] < 0.0) {
          stack[++stackpointer] = node->leftchild;
          node = node->leftchild + 1;
        } else {
          stack[++stackpointer] = node->leftchild + 1;
          node = node->leftchild;
//         }
      }
    }

    for (unsigned char i = 0; i < node->prims->size ; ++i ) {
      const Triangle& tri = triangles[node->prims->data[i]];
      tri.intersect ( r );
    }
    if ( r.didHitSomething() )
      return true;
    // pop stack
    node = stack[stackpointer];
    --stackpointer;

  } while ( node ) ;

  return false;
}


bool KdTree::traverse ( const KdTreenode& node, RadianceRay& r, float tMax) {
  if ( node.axis == 3 ) {
    IntersectionResult tempIR;
    for (unsigned char i = 0; i < node.prims->size ; ++i ) {
      const Triangle& tri = triangles[node.prims->data[i]];
      tri.intersect ( r );
      ++hc;
    }
    return r.didHitSomething();
  }
  bool justFar = ( r.getDirection().value[node.axis]  < 0.0f ) ?
                 r.getStart().value[node.axis] < node.splitPos:
                 node.splitPos < r.getStart().value[node.axis];
  unsigned int near=0, far=1;
  if ( r.getDirection().value[node.axis] < 0.0f ) {
    // near is right, far is left
    near = 1; far = 0;
  }
  if ( !justFar ) {
    if ( traverse ( *(node.leftchild +  near), r, tMax ) )
      return true;
  }
  bool skipFar = (( node.splitPos - r.getStart().value[node.axis] ) * r.getInvDirection().value[node.axis]) > tMax;
  return !skipFar && traverse ( *(node.leftchild + far), r, tMax ) ;

}

/**
 * Assumes normalized ray.
**/
const RGBvalue KdTree::trace ( Ray& r, unsigned int depth ) {
  /*
  * Ray-box intersection using IEEE numerical properties to ensure that the
  * test is both robust and efficient, as described in:
  *
  *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
  *      "An Efficient and Robust Ray-Box Intersection Algorithm"
  *      Journal of graphics tools, 10(1):49-54, 2005
  *
  *      * slightly altered to find point of intersection *
  */
  float t0 = 0.0;
  float t1 = UNENDLICH;
  float tmin, tmax, tymin, tymax, tzmin, tzmax;
  Vector3D parameters[2] = {Vector3D ( bounds[0], bounds[2], bounds[4] ),
                            Vector3D ( bounds[1], bounds[3], bounds[5] ) };
  const Vector3D &inv_direction = r.getInvDirection();
  int sign[3];
  sign[0] = ( inv_direction.value[0] < 0 );
  sign[1] = ( inv_direction.value[1] < 0 );
  sign[2] = ( inv_direction.value[2] < 0 );

  tmin = ( parameters[sign[0]].value[0] - r.getStart().value[0] ) * inv_direction.value[0];
  tmax = ( parameters[1-sign[0]].value[0] - r.getStart().value[0] ) * inv_direction.value[0];
  tymin = ( parameters[sign[1]].value[1] - r.getStart().value[1] ) * inv_direction.value[1];
  tymax = ( parameters[1-sign[1]].value[1] - r.getStart().value[1] ) * inv_direction.value[1];
  if ( ( tmin > tymax ) || ( tymin > tmax ) ) {
    return RGBvalue ( 0.0, 0.0, 0.0 );
  }
  if ( tymin > tmin )
    tmin = tymin;
  if ( tymax < tmax )
    tmax = tymax;
  tzmin = ( parameters[sign[2]].value[2] - r.getStart().value[2] ) * inv_direction.value[2];
  tzmax = ( parameters[1-sign[2]].value[2] - r.getStart().value[2] ) * inv_direction.value[2];
  if ( ( tmin > tzmax ) || ( tzmin > tmax ) ) {
    return RGBvalue ( 0.0, 0.0, 0.0 );
  }
  if ( tzmin > tmin )
    tmin = tzmin;
  if ( tzmax < tmax )
    tmax = tzmax;
  if ( ( tmin < t1 ) && ( tmax > t0 ) ) {
    ;
  } else
    return RGBvalue ( 0.0, 0.0, 0.0 );

  hc = 0;
  RadianceRay rr ( r.getStart(), r.getDirection());
//   traverse ( nodes.get(0), rr, tmax );
  traverseIterative ( rr, tmin, tmax );
//    RGBvalue result (hc / triangles.size(), 0.0, 0.0 );
  RGBvalue result ( 0.0, 0.0, 0.0 );
  if ( rr.didHitSomething() ) {
//    return RGBvalue(1.0, 0.0, 0.0);
    const Intersection &i = rr.getClosestIntersection();
    const Triangle &hitTriangle = * ( i.triangle );
    Vector3D n ( hitTriangle.getNormalAt ( i ) );
    const PhongMaterial& mat = hitTriangle.getMaterial();
    const std::vector<Light> lights = scene.getLights();
    std::vector<Light>::const_iterator it;
    IntersectionResult doesntMatter;
    for ( it = lights.begin(); it!=lights.end(); ++it ) {
      const Light& light = *it;
      Vector3D l ( light.getPosition() -  i.intersectionPoint );
      tmax = l.length();
      l.normalize();
      Ray intersectToLigth ( i.intersectionPoint, l );
      float dif = n * l;
      if ( dif > 0.0 ) {
        result.add ( dif * mat.diffuse[0] * light.getColor().getRGB() [0],
                     dif * mat.diffuse[1] * light.getColor().getRGB() [1],
                     dif * mat.diffuse[2] * light.getColor().getRGB() [2] );
      }
    }
  }
  return result;
}

void KdTree::construct() {
  unsigned int triCount = triangles.size();
  std::vector<unsigned int> prims;
  prims.reserve ( triCount );
  for ( unsigned int i = 0; i < triCount; ++i )
    prims.push_back ( i );
  subdivide ( nodes.getNextFree(), prims, bounds, 20 );
  std::cout << "Nodes: " << nodes.size() << std::endl;
  assert( checkConsitency() );
}


float KdTree::calculatecost ( KdTreenode &node, const std::vector<unsigned int> &prims, float *bounds, float splitPos ) {
  float leftmost, rightmost;
  const float boundLength[3] = { bounds[1] - bounds[0],  bounds[3] - bounds[2],  bounds[5] - bounds[4] };
  unsigned int left = 0, right = 0;
  std::vector<unsigned int>::const_iterator iter = prims.begin();
  for ( ; iter != prims.end(); ++iter ) {
    const Triangle& tri = triangles[*iter];
    leftmost = fminf ( tri.getPoint ( 0 ).value[node.axis], fminf ( tri.getPoint ( 1 ).value[node.axis], tri.getPoint ( 2 ).value[node.axis] ) );
    rightmost = fmaxf ( tri.getPoint ( 0 ).value[node.axis], fmaxf ( tri.getPoint ( 1 ).value[node.axis], tri.getPoint ( 2 ).value[node.axis] ) );
    if ( leftmost < splitPos )
      ++left;
    if ( rightmost > splitPos )
      ++right;
  }
  float leftarea,rightarea;
  switch ( node.axis ) {
    case 0:
      leftarea  = 2 * ( splitPos - bounds[0] ) * boundLength[1] * boundLength[2];
      rightarea = 2 * ( bounds[1] - splitPos ) * boundLength[1] * boundLength[2];
      break;
    case 1:
      leftarea  = 2 * ( splitPos - bounds[2] ) * boundLength[0] * boundLength[2];
      rightarea = 2 * ( bounds[3] - splitPos ) * boundLength[0] * boundLength[2];
      break;
    case 2:
      leftarea  = 2 * ( splitPos - bounds[4] ) * boundLength[1] * boundLength[0];
      rightarea = 2 * ( bounds[5] - splitPos ) * boundLength[1] * boundLength[0];
      break;
    default:
      leftarea = rightarea = 0;
  }
  return 0.1 + 1.2 * ( leftarea * left + rightarea * right );
}

void KdTree::subdivide ( KdTreenode &node, const std::vector<unsigned int> &prims, float *bounds, unsigned int depth ) {
#ifdef VISUAL_DEBUGGER
  node.idx = ++nodeId;
  memcpy ( node.bounds, bounds, 6 * sizeof ( float ) );
#endif
  if ( prims.size() <= MAX_PRIMCOUNT_PER_LEAF || !depth ) {
    node.axis = 3;
    assert(prims.size() <= 256);
    node.prims = new SmallStaticArray<unsigned int>((unsigned char)prims.size());
    for (unsigned int i = 0; i < node.prims->size; ++i)
      node.prims->data[i] = prims[i];
    return;
  }
  std::vector<unsigned int>::const_iterator iter = prims.begin();
  const float boundLength[3] = { bounds[1] - bounds[0],  bounds[3] - bounds[2],  bounds[5] - bounds[4] };
  node.axis = 0;

  if ( boundLength[1] > boundLength[0] )
    node.axis = 1;
  if ( boundLength[2] > boundLength[node.axis] )
    node.axis = 2;

  float cost, bestpos = -1;
  float bestcost = 1000000;
  iter = prims.begin();
  for ( ; iter != prims.end(); ++iter ) {
    const Triangle& tri = triangles[*iter];
    float leftmost, rightmost;
    leftmost = fminf ( tri.getPoint ( 0 ).value[node.axis], fminf ( tri.getPoint ( 1 ).value[node.axis], tri.getPoint ( 2 ).value[node.axis] ) );
    rightmost = fmaxf ( tri.getPoint ( 0 ).value[node.axis], fmaxf ( tri.getPoint ( 1 ).value[node.axis], tri.getPoint ( 2 ).value[node.axis] ) );


    if ( ( cost = calculatecost ( node, prims, bounds, leftmost ) ) < bestcost )
      bestcost = cost, bestpos = leftmost;

    if ( ( cost = calculatecost ( node, prims, bounds, rightmost ) ) < bestcost )
      bestcost = cost, bestpos = rightmost;
  }
  node.splitPos = bestpos;

  float thisCost = 2 * boundLength[0] * boundLength[1] * boundLength[2] * prims.size() ;
  if ( thisCost < bestcost ) {
    node.axis = 3;
    assert(prims.size() <= 256);
    node.prims = new SmallStaticArray<unsigned int>((unsigned char)prims.size());
    for (unsigned int i = 0; i < node.prims->size; ++i)
      node.prims->data[i] = prims[i];
    return;
  }

  std::vector<unsigned int> left;
  std::vector<unsigned int> right;

  iter = prims.begin();
  float leftmost, rightmost;
  for ( ; iter != prims.end(); ++iter ) {
    const Triangle& tri = triangles[*iter];
    leftmost = fminf ( tri.getPoint ( 0 ).value[node.axis], fminf ( tri.getPoint ( 1 ).value[node.axis], tri.getPoint ( 2 ).value[node.axis] ) );
    rightmost = fmaxf ( tri.getPoint ( 0 ).value[node.axis], fmaxf ( tri.getPoint ( 1 ).value[node.axis], tri.getPoint ( 2 ).value[node.axis] ) );
    if ( leftmost < node.splitPos )
      left.push_back ( *iter );
    if ( rightmost > node.splitPos )
      right.push_back ( *iter );
  }

  // split the current bounding box in two along the current axis
  float leftbounds[6], rightbounds[6];
  switch ( node.axis ) {
    case 0: leftbounds[0] = bounds[0];
      leftbounds[1] = node.splitPos ;
      memcpy ( leftbounds+2, bounds+2,  4 * sizeof ( float ) );

      rightbounds[0] = node.splitPos;
      rightbounds[1] = bounds[1];
      memcpy ( rightbounds+2, bounds+2, 4 * sizeof ( float ) );
      break;
    case 1: leftbounds[2] = bounds[2];
      leftbounds[3] = node.splitPos ;
      leftbounds[0] = bounds[0];
      leftbounds[1] = bounds[1];
      leftbounds[4] = bounds[4];
      leftbounds[5] = bounds[5];

      rightbounds[2] = node.splitPos;
      rightbounds[3] = bounds[3];
      rightbounds[0] = bounds[0];
      rightbounds[1] = bounds[1];
      rightbounds[4] = bounds[4];
      rightbounds[5] = bounds[5];
      break;
    case 2: leftbounds[4] = bounds[4];
      leftbounds[5] = node.splitPos ;
      memcpy ( leftbounds, bounds, 4 * sizeof ( float ) );
      rightbounds[4] = node.splitPos;
      rightbounds[5] = bounds[5];
      memcpy ( rightbounds, bounds, 4 * sizeof ( float ) );
      break;
  }

  node.leftchild = nodes.getNextFreePair();
  subdivide ( *node.leftchild, left, leftbounds, depth - 1 );
  
  subdivide ( *(node.leftchild + 1), right, rightbounds, depth - 1 );
  
}

bool KdTree::checkConsitency() const {
  std::set<unsigned int> missing;
  for (unsigned int triIdx = 0; triIdx < triangles.size(); ++triIdx) {
    missing.insert(triIdx);
  }  
  if ( !checkConsRec(&nodes.get(0), missing)) {
    std::cout << "Nodes inconsistent." << std::endl;
    return false;
  }
  
  if ( !missing.empty() ) {
    std::cout << missing.size() << "unassigned triangles" << std::endl;
    return false;
  }
  return true;
}

bool KdTree::checkConsRec(KdTreenode * node, std::set<unsigned int>& missing) const {
  //check if pointer is in nodes array;
  bool nodefound = false;
  for (unsigned int i = 0; i < nodes.size() && !nodefound; ++i) {
    nodefound = ( &nodes.get(i) == node );
  }
  
  if ( !nodefound ) {
    std::cout << "Invalid reference found." << std::endl;
    return false;
  }
  
  if ( node->axis == 3 ) {
    for ( unsigned int i = 0; i < node->prims->size; ++i) {
      missing.erase(node->prims->data[i]);
    }
    return true;
  } else {
    return checkConsRec(node->leftchild, missing) && 
           checkConsRec(node->leftchild + 1, missing);
  }

}



#ifdef VISUAL_DEBUGGER
void KdTree::drawContent ( const KdTreenode& node, GLWidget* context, bool parentMarked ) const {
  parentMarked |= ( node.idx == markednode );

  if ( node.axis == 3 ) {
    glEnable ( GL_LIGHTING );
    glColor3f ( 1.0, 1.0, 1.0 );
    if ( parentMarked ) {
      std::vector<unsigned int>::const_iterator iter = node.prims->begin();
      for ( ; iter != node.prims->end(); ++iter ) {
        const Triangle& t = triangles[*iter];
        t.drawGL();
      }
    }
    glDisable ( GL_LIGHTING );
    if ( node.idx == markednode ) {
      glColor3f ( 1.0, 1.0, 1.0 );
    } else {
      glColor3f ( 0.3, 0.3, 0.3 );
    }

    glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
    float dims[3] = { node.bounds[1] - node.bounds[0], node.bounds[3] - node.bounds[2], node.bounds[5] - node.bounds[4] };
    float center[3] = {node.bounds[0] + dims[0] * 0.5, node.bounds[2] + dims[1] * 0.5, node.bounds[4] + dims[2] * 0.5};
    context->drawMinicube ( center[0], center[1], center[2], dims );
    glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
  } else {

    if ( node.idx == markednode ) {
      glEnable ( GL_BLEND );
      glDisable ( GL_LIGHTING );
      glDisable ( GL_DEPTH_TEST );
      glColor4f ( 0.3, 0.0, 1.0, 0.3 );
      glBegin ( GL_QUADS );
      switch ( node.axis ) {
        case 0:
          glVertex3f ( node.splitPos, node.bounds[2], node.bounds[4] );
          glVertex3f ( node.splitPos, node.bounds[2], node.bounds[5] );
          glVertex3f ( node.splitPos, node.bounds[3], node.bounds[5] );
          glVertex3f ( node.splitPos, node.bounds[3], node.bounds[4] );
          break;
        case 1:
          glVertex3f ( node.bounds[0], node.splitPos, node.bounds[4] );
          glVertex3f ( node.bounds[1], node.splitPos, node.bounds[4] );
          glVertex3f ( node.bounds[1], node.splitPos, node.bounds[5] );
          glVertex3f ( node.bounds[0], node.splitPos, node.bounds[5] );
          break;
        case 2:
          glVertex3f ( node.bounds[0], node.bounds[2], node.splitPos );
          glVertex3f ( node.bounds[0], node.bounds[3], node.splitPos );
          glVertex3f ( node.bounds[1], node.bounds[3], node.splitPos );
          glVertex3f ( node.bounds[1], node.bounds[2], node.splitPos );
          break;
      }
      glEnd();
      glDisable ( GL_BLEND );
      glEnable ( GL_LIGHTING );
      glEnable ( GL_DEPTH_TEST );
    }


    drawContent ( * ( node.children[0] ), context, parentMarked );
    drawContent ( * ( node.children[1] ), context, parentMarked );
  }
}

void KdTree::draw ( GLWidget* context ) const {
  drawContent ( *tree, context, false );
}
static float leftMax = 0.0;

const Vector3D KdTree::drawTree ( const KdTreenode& node, unsigned int depth,  GLWidget* context ) const {
  if ( node.axis == 3 ) {
    if ( node.idx == markednode )
      glColor3f ( 0.2, 0.2, 1.0 );
    else
      glColor3f ( 0.2, 0.2, 0.2 );
    Vector3D pos ( leftMax += 0.06, 1.0 - depth * 0.04, 0.0 );
    glPushName ( node.idx );
    context->drawMinicube ( pos[0], pos[1], pos[2], 0.02 );
    glPopName();
    return pos;
  }



  Vector3D currLeft = drawTree ( * ( node.children[0] ), depth + 1,context );
  Vector3D currRight = drawTree ( * ( node.children[1] ), depth + 1, context );
  float xpos = ( currLeft[0] + currRight[0] ) * 0.5f;
  if ( node.idx == markednode ) {
    glColor3f ( 0.2, 0.2, 1.0 );
  } else
    glColor3f ( 0.2, 0.2, 0.2 );
  Vector3D nodePos = Vector3D ( xpos , ( float ) ( 1.0 - depth * 0.04f ), ( float ) 0.0f );
  glPushName ( node.idx );
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

void KdTree::drawSchema ( GLWidget* context ) const {
  leftMax = 0.0;
  glDisable ( GL_LIGHTING );
  drawTree ( *tree, 0, context );
}

void KdTree::drawWithNames ( GLWidget* context ) const {
  leftMax = 0.0;
  glDisable ( GL_LIGHTING );
  drawTree ( *tree, 0, context );
}

void KdTree::keyReleaseEvent ( QKeyEvent* event ) {
  AccelerationStruct::keyReleaseEvent ( event );
}

void KdTree::select ( int selected ) {
  markednode = selected;
}
#endif

