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
bool KdTree::traverseIterative ( RadianceRay& r ) {
  KdTreeStacknode stack[128];
  int stackpointer = 0;
  stack[0].node = 0;
  KdTreenode *node = &nodes.get(0);
  float d ;
  
    
  do {
    while( node->axis != 3 ) {
      d = ( node->splitPos - r.getStart().value[node->axis] ) * r.getInvDirection().value[node->axis] ;
      if ( d <= r.getMin() ) { // just far side
        node = node->leftchild + ((r.getDirection().value[node->axis] > 0.0) ? 1 : 0 );
      } else if ( d >= r.getMax() ) { // just near side
        node = node->leftchild + ((r.getDirection().value[node->axis] > 0.0) ? 0 : 1 ) ;
      } else {
        if (r.getDirection().value[node->axis] < 0.0) {
          stack[++stackpointer].node = node->leftchild;
          stack[stackpointer].tMin = r.getMin();
          stack[stackpointer].tMax = r.getMax();
          node = node->leftchild + 1;
          r.setMax(d);
        } else {
          stack[++stackpointer].node = node->leftchild + 1;
          stack[stackpointer].tMin = d;
          stack[stackpointer].tMax = r.getMax();
          node = node->leftchild;
          r.setMax(d);
      }
      }
    }

    for (unsigned char i = 0; i < node->prims->size ; ++i ) {
      const Triangle& tri = triangles[node->prims->data[i]];
      tri.intersect ( r );
    }
    if ( r.didHitSomething() )
      return true;
    // pop stack
    node = stack[stackpointer].node;
    r.setMin(stack[stackpointer].tMin);
    r.setMax(stack[stackpointer].tMax);
    --stackpointer;

  } while ( node ) ;

  return false;
}

bool KdTree::traverseShadowIterative ( Ray& r ) {
  KdTreeStacknode stack[128];
  int stackpointer = 0;
  stack[0].node = 0;
  KdTreenode *node = &nodes.get(0);
  float d ;
  
    
  do {
    while( node->axis != 3 ) {
      d = ( node->splitPos - r.getStart().value[node->axis] ) * r.getInvDirection().value[node->axis] ;
      if ( d <= r.getMin() ) { // just far side
        node = node->leftchild + ((r.getDirection().value[node->axis] > 0.0) ? 1 : 0 );
      } else if ( d >= r.getMax() ) { // just near side
        node = node->leftchild + ((r.getDirection().value[node->axis] > 0.0) ? 0 : 1 ) ;
      } else {
        if (r.getDirection().value[node->axis] < 0.0) {
          stack[++stackpointer].node = node->leftchild;
          stack[stackpointer].tMin = r.getMin();
          stack[stackpointer].tMax = r.getMax();
          node = node->leftchild + 1;
          r.setMax(d);
        } else {
          stack[++stackpointer].node = node->leftchild + 1;
          stack[stackpointer].tMin = d;
          stack[stackpointer].tMax = r.getMax();
          node = node->leftchild;
          r.setMax(d);
      }
      }
    }

    for (unsigned char i = 0; i < node->prims->size ; ++i ) {
      const Triangle& tri = triangles[node->prims->data[i]];
      if ( tri.intersect ( r ) )
        return true;
    }

    // pop stack
    node = stack[stackpointer].node;
    r.setMin(stack[stackpointer].tMin);
    r.setMax(stack[stackpointer].tMax);
    --stackpointer;

  } while ( node ) ;

  return false;
}

void KdTree::traceall(KdTreenode &node, RadianceRay& r) {
  if ( node.axis == 3 ) {
        for (unsigned char i = 0; i < node.prims->size ; ++i ) {
      const Triangle& tri = triangles[node.prims->data[i]];
      tri.intersect ( r );
      ++hc;
    }
  } else {
    float d = ( node.splitPos - r.getStart().value[node.axis] ) * r.getInvDirection().value[node.axis] ;  
    KdTreenode &near = node.leftchild[(r.getDirection().value[node.axis] > 0.0) ? 0 : 1];
    KdTreenode &far =  node.leftchild[(r.getDirection().value[node.axis] > 0.0) ? 1 : 0];
    
    float tMax = r.getMax();
    float tMin = r.getMin();
    if ( d >= tMin ) {
      r.setMax(d);
      traceall(near, r);
    }
     
    if (d <= tMax && !r.didHitSomething() ) {
      r.setMax(tMax);
      r.setMin(d);
      traceall(far, r);
    }
    r.setMin(tMin);
  }
}

bool KdTree::isBlocked(Ray& r) {
 return traverseShadowIterative( r );
}

/**
 * Assumes normalized ray.
**/
const RGBvalue KdTree::trace ( RadianceRay& r, unsigned int depth ) {
  if ( !trimRaytoBounds( r ) )
    return RGBvalue( 0.0, 0.0, 0.0 );
  traverseIterative ( r );
  RGBvalue result ( 0.0, 0.0, 0.0 );
  r.shade(result, depth);
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

