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
#include "raypacket.h"
#include "stats.h"
#define MAX_PRIMCOUNT_PER_LEAF 3


KdTree::KdTree ( const Scene &scene ) : AccelerationStruct ( scene ){}

void KdTree::deleteNode ( KdTreenode &node ) {
  if ( node.axis == 3 ) 
    delete  node.prims ;
}

KdTree::~KdTree() {
   deleteNode ( nodes.get(0) );
}

bool KdTree::traverseIterative ( RadianceRay& r ) const {
  KdTreeStacknode stack[128];
  int stackpointer = 0;
  stack[0].node = 0;
  KdTreenode *node = &nodes.get(0);
  float d ;
  unsigned int leftIsNear[] = { (r.getDirection().value[0] > 0.0) ? 1 : 0,
    (r.getDirection().value[1] > 0.0) ? 1 : 0 ,(r.getDirection().value[2] > 0.0) ? 1 : 0 } ;
  
  do {
    while( node->axis != 3 ) {
      statsinc("Traversalsteps per Frame");
      d = ( node->splitPos - r.getStart().value[node->axis] ) * r.getInvDirection().value[node->axis] ;
      if ( d <= r.getMin() ) { // just far side
        node = node->leftchild + leftIsNear[node->axis];
      } else if ( d >= r.getMax() ) { // just near side
        node = node->leftchild + 1 - leftIsNear[node->axis];
      } else {
          stack[++stackpointer].node = node->leftchild + leftIsNear[node->axis] ;
          stack[stackpointer].tMin = d;
          stack[stackpointer].tMax = r.getMax();
          node = node->leftchild + 1 - leftIsNear[node->axis];
          r.setMax(d);
      }
    }
    statsinc("Traversalsteps per Frame");
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

bool KdTree::traverseShadowIterative ( Ray& r ) const {
  KdTreeStacknode stack[128];
  int stackpointer = 0;
  stack[0].node = 0;
  KdTreenode *node = &nodes.get(0);
  float d;    
  
  do {
    while( node->axis != 3 ) {
      d = ( node->splitPos - r.getStart().value[node->axis] ) * r.getInvDirection().value[node->axis] ;
      if ( d <= r.getMin() ) { // just far side
        node = node->leftchild + ((r.getDirection().value[node->axis] > 0.0) ? 1 : 0 );
      } else if ( d >= r.getMax() ) { // just near side
        node = node->leftchild + ((r.getDirection().value[node->axis] > 0.0) ? 0 : 1 ) ;
      } else {
        if (r.getDirection().value[node->axis] < 0.0) {
          stack[++stackpointer].node = node->leftchild ;
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

void KdTree::traversePacket ( RayPacket& rp ) const {
//   KdTreeStacknode stack[128];
//   int stackpointer = 0;
//   stack[0].node = 0;
//   KdTreenode *node = &nodes.get(0);
//   float d, dmin, dmax, splitdist = 0.0 ;
//   static unsigned int i, idx[] = { 0, rp.getPacketWidth()-1, rp.getRayCount() - rp.getPacketWidth(), rp.getRayCount()-1 };
//   float tmax = fmaxf(fmaxf(fmaxf( rp.getMax(idx[0]), rp.getMax(idx[1])), rp.getMax(idx[2])),rp.getMax(idx[3]));
//   float tmin = fminf(fminf(fminf( rp.getMin(idx[0]), rp.getMin(idx[1])), rp.getMin(idx[2])),rp.getMin(idx[3]));
//   bool inNearNode = false;
//   unsigned int leftIsNear = (rp.getDirection(0).value[node->axis] > 0.0) ? 1 : 0 ;
//   bool needFront;
//   bool needBack;
//   do {
//     while( node->axis != 3 ) {
//       splitdist = node->splitPos - rp.getOrigin().value[node->axis];
//       dmin = dmax = splitdist * rp.getInvDirection(0).value[node->axis];
//       for(i = 1 ; i < rp.getRayCount() ; ++i){
//         d = splitdist * rp.getInvDirection(i).value[node->axis];
//         if ( d < dmin)
//           dmin = d;
//         else if (d > dmax)
//           dmax = d;
//       }
//       
//      needFront = false;
//      needBack = false;
//      
//      for( i = 0 ; i < rp.getRayCount() ; ++i ){
//        d = splitdist * rp.getInvDirection(i).value[node->axis];
//        needBack |= ( d > tmin );
//        needFront |= (d < tmax);
//      }
// 
//       
//      if ( needBack && needFront) {
//           inNearNode = true;
//           stack[++stackpointer].node = node->leftchild + ( 1 - leftIsNear);
//           stack[stackpointer].tMin = dmax;
//           stack[stackpointer].tMax = tmax;
//           node = node->leftchild + leftIsNear;
//           tmax = dmin;
//      } else if ( needBack ) { // just far side
//         node = node->leftchild + leftIsNear;
//         tmin = dmin;
//      } else { // just near side
//         tmax = dmax;
//         node = node->leftchild + (1 - leftIsNear) ;
//       }
//      
// /*      if ( dmax <= tmin ) { // just far side
//         node = node->leftchild + leftIsNear;
//         inNearNode = false;
//       } else if ( dmin >= tmax ) { // just near side
//         inNearNode = true;
//         node = node->leftchild + (1 - leftIsNear) ;
//       } else {
//           inNearNode = true;
//           stack[++stackpointer].node = node->leftchild + ( 1 - leftIsNear);
//           stack[stackpointer].tMin = dmax;
//           stack[stackpointer].tMax = tmax;
//           node = node->leftchild + leftIsNear;
//           tmax = dmin;
//      }*/     
//     }
//     // calc precise tmax for all rays in packet
// //     if ( inNearNode ) 
// //       for (i = 0 ; i < rp.getPacketWidth() ; ++i ) {
// //         rp.setMax(i, splitdist * rp.getInvDirection(i).value[node->axis] );
// //         rp.setMin(i, tmin );
// //         }
// //     else
// //       for (i = 0 ; i < rp.getPacketWidth() ; ++i ) {
// //         rp.setMax(i, tmax );
// //         rp.setMin(i, splitdist * rp.getInvDirection(i).value[node->axis] );
// //         }
//    
// 
//     for ( i = 0; i < node->prims->size ; ++i ) {
//       const Triangle& tri = triangles[node->prims->data[i]];
//       tri.intersect ( rp, node->prims->data[i] );
//     }
//     
//     // pop stack
//     node = stack[stackpointer].node;
//     tmin = stack[stackpointer].tMin;
//     tmax = stack[stackpointer].tMax;
//     inNearNode = false;
//     --stackpointer;
//   } while ( node ) ;
}

bool KdTree::trace ( RayPacket& rp, unsigned int depth ) const {
  if ( !trimRaytoBounds(rp) )
    return false; // full miss

  KdTreePacektStacknode stack[128];
  int stackpointer = 0;
  stack[0].node = 0;
  KdTreenode *node = &nodes.get(0);
  __m128 d ;
  unsigned int leftIsNear[] = { (rp.shaft.direction.c[0].v.f[0] > 0.0) ? 1 : 0,
    (rp.shaft.direction.c[1].v.f[0] > 0.0) ? 1 : 0 ,(rp.shaft.direction.c[2].v.f[0] > 0.0) ? 1 : 0 } ;
  
  do {
    while( node->axis != 3 ) {
      statsinc("Traversalsteps per Frame");
      d = _mm_mul_ps(_mm_sub_ps(_mm_set1_ps(node->splitPos), (rp.shaft.origin.c[node->axis]).v.sse), rp.shaft.inv_direction.c[node->axis].v.sse);
      if ( _mm_movemask_ps(_mm_cmple_ps(d, rp.shaft.tmin.v.sse)) == 15 ) { // just far side
        node = node->leftchild + leftIsNear[node->axis];
      } else if ( _mm_movemask_ps(_mm_cmpnlt_ps(d, rp.shaft.tmax.v.sse) ) == 15) { // just near side
        node = node->leftchild + 1 - leftIsNear[node->axis];
      } else {
          stack[++stackpointer].node = node->leftchild + leftIsNear[node->axis] ;
          stack[stackpointer].tMin = d;
          stack[stackpointer].tMax = rp.shaft.tmax.v.sse;
          node = node->leftchild + 1 - leftIsNear[node->axis];
          rp.shaft.tmax = d;
      }
    }
    statsinc("Traversalsteps per Frame");
    for (unsigned char i = 0; i < node->prims->size ; ++i ) {
      const Triangle& tri = triangles[node->prims->data[i]];
      tri.intersect ( rp, node->prims->data[i] );
    }

    int earlyExit = 15;
    for (unsigned int i = 0; i < rp.getR4Count(); ++i)
      earlyExit &= rp.r4[i].mask;
    if ( earlyExit == 15 )
      return true;
    // pop stack
    node = stack[stackpointer].node;
    rp.shaft.tmin = stack[stackpointer].tMin;
    rp.shaft.tmax = stack[stackpointer].tMax;
    --stackpointer;
  } while ( node ) ;

  return true;
}

bool KdTree::isBlocked(Ray& r) const {
 return traverseShadowIterative( r );
}

const Intersection& KdTree::getClosestIntersection(RadianceRay& r) const {
    traverseIterative ( r );
    return r.getClosestIntersection();
}

/**
 * Assumes normalized ray.
**/
const RGBvalue KdTree::trace ( RadianceRay& r, unsigned int depth ) const {
  if ( !trimRaytoBounds( r ) )
    return RGBvalue( 0.0, 0.0, 0.0 );
  traverseIterative ( r );
  RGBvalue result ( 0.0, 0.0, 0.0 );
  r.shade(result, depth);
  return result;
}

void KdTree::construct() {
  nodes.clear();
  unsigned int triCount = triangles.size();
  std::vector<unsigned int> prims;
  prims.reserve ( triCount );
  for ( unsigned int i = 0; i < triCount; ++i )
    prims.push_back ( i );
  subdivide ( nodes.getNextFree(), prims, bounds, 30 );
//   std::cout << "Nodes: " << nodes.size() << std::endl;
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
  const float totalArea = boundLength[0] * boundLength[1] * boundLength[2];
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
  return 0.1 + 1.2 * ( (leftarea/totalArea) * left + (rightarea/totalArea) * right );
}

void KdTree::subdivide ( KdTreenode &node, const std::vector<unsigned int> &prims, float *bounds, unsigned int depth ) {
  statsset("Treedepth", fmax(depth, Stats::getInstance().get("Treedepth")));
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
