//
// C++ Implementation: kdtreesahnlogn
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "kdtreesahnlogn.h"
#include "scene.h"
#include <iostream>
#include <assert.h>
#include "kdnode.h"

#define NODE_SIZE (sizeof(KdNode) / sizeof(unsigned int))

using namespace Occluder;

KdTreeSahNlogN::KdTreeSahNlogN(const Scene& scene): KdTreeBase(scene) {
}


KdTreeSahNlogN::~KdTreeSahNlogN() {
}

// TODO: make satic memeber of base class
static const Vec3 AXIS[] = { Vec3( 1.0, 0.0, 0.0), Vec3( 0.0, 1.0, 0.0), Vec3( 0.0, 0.0, 1.0) };


void KdTreeSahNlogN::construct() {
  cachePrimitiveAABBs();
  const unsigned int primCount = scene.getPrimitiveCount();
  unsigned int i;

  // start with generating list of *all* events 
  List<KdTreeSahNlogN::Event>events;
  events.reserve(primCount * (2 * 3)); // each primitive will generate
                                       // 2 (min,max) events on each of the three axes
  for ( unsigned int i = 0; i < primCount; ++i) {
//     const Primitive& currentPrimitive = scene.getPrimitive(i);
    const AABB& currBB = getAABBForPrimitiv(i);
    for ( unsigned int axis = 0; axis < 3; ++axis)
      if ( currBB.getMin(axis) == currBB.getMax(axis) ) {
        events.push_back(Event(currBB.getMin(axis), INPLANE, axis, i));
      } else {
        events.push_back(Event(currBB.getMin(axis), START, axis, i));
        events.push_back(Event(currBB.getMax(axis), END, axis, i));
      }
  }

  sort(events.begin(), events.end());

  unsigned int size = (2 * primCount - 1) * sizeof(KdNode) // space for treenodes
                    + (unsigned int)(1.3f * primCount) * sizeof(unsigned int)    ;// space for object references
  totalSpace = size;
//     size = 5 * sizeof(KdNode)  + 1.5f * primCount * sizeof(unsigned int);
  memBlock = (unsigned int*)malloc(size);
  for (i = 0; i < primCount; ++i) {
    memBlock[i] = i;
  }
  overlap = (Overlap*)malloc(primCount * sizeof(Overlap));

  subdivide(memBlock, primCount, scene.getAABB(), size / sizeof(unsigned int), events);
  const KdNode *root = (const KdNode *)memBlock;

  free(overlap);
  root->analyze();
  writeToDisk();
}


KdTreeBase::SplitCandidate KdTreeSahNlogN::determineSplitpos(const AABB& v, const unsigned int* primitves, const unsigned int primitveCount, List<Event>& events ) {

    unsigned int NL[3] = { 0, 0, 0}, NR[3] = { primitveCount, primitveCount, primitveCount}, NP[3] = { 0, 0, 0};
    unsigned int pEnd, pStart, pPlane;
    SAHCost currentCost;
    SplitCandidate bestSplit = { v.getMin(0), 0, false};
    SAHCost bestCost(SAH(v, bestSplit , 0, primitveCount, 0));
    SplitCandidate currentSplit;
    bestSplit.axis = 3; // for the case that no split plane will be found

//   List<KdTreeSahNlogN::Event>events;
//   events.reserve(primitveCount * (2 * 3)); // each primitive will generate
//                                        // 2 (min,max) events on each of the three axes
//   for ( unsigned int i = 0; i < primitveCount; ++i) {
//     const Primitive& currentPrimitive = scene.getPrimitive(primitves[i]);
//     const AABB& currBB = getAABBForPrimitiv(primitves[i]);
//     for ( unsigned int axis = 0; axis < 3; ++axis)
//       if ( (currentPrimitive.getNormal() == AXIS[axis]) ||  
//           (( -1.0 * currentPrimitive.getNormal()) == AXIS[axis])) {
//         events.push_back(Event(currBB.getMin(axis), INPLANE, axis, i));
//       } else {
//         events.push_back(Event(currBB.getMin(axis), START, axis, i));
//         events.push_back(Event(currBB.getMax(axis), END, axis, i));
//       }
//   }
//   sort(events.begin(), events.end());

    List<Event>::const_iterator iter = events.begin();


//     for (; iter != events.end(); ++iter) {
//       std::cout << (*iter).t << ":";
//       switch((*iter).type) {
//         case START: std::cout << "3START";break;
//         case END: std::cout << "3END";break;
//         case INPLANE: std::cout << "3PLANE";break;
//       }
//     std::cout << "\n";
//     }
//     iter = events.begin();

    while (iter != events.end()) {
      currentSplit.axis = (*iter).axis;
      currentSplit.pos  = (*iter).t;
      pEnd = pStart = pPlane = 0;

      while (  (iter !=  events.end()) 
            && (currentSplit.pos == (*iter).t) 
            && (currentSplit.axis == (*iter).axis) 
            && ((*iter).type == END) ) {
            ++pEnd;
            ++iter;
      }

      while (  (iter !=  events.end()) 
            && (currentSplit.pos == (*iter).t) 
            && (currentSplit.axis == (*iter).axis) 
            && ((*iter).type == INPLANE) ) {
            ++pPlane;
            ++iter;
      }

      while (  (iter !=  events.end()) 
            && (currentSplit.pos == (*iter).t) 
            && (currentSplit.axis == (*iter).axis) 
            && ((*iter).type == START) ) {
            ++pStart;
            ++iter;
      }

      NP[currentSplit.axis] = pPlane;
      NR[currentSplit.axis] -= pPlane;
      NR[currentSplit.axis] -= pEnd;

      if ( !(NR[currentSplit.axis] <= primitveCount) ) {
        List<Event>::const_iterator debugiter = events.begin();
        unsigned int sad = 0;
        for (; debugiter != events.end(); ++debugiter) {
          if ( (*debugiter).axis == currentSplit.axis ) {
            std::cout << sad++ << (*debugiter).t << ":";
            switch ((*debugiter).type) {
              case (START):std::cout << "START";break;
              case (INPLANE):std::cout << "PLANE";break;
              case (END):std::cout << "END";break;
              default :std::cout << "Müll";break;
            }
            std::cout << std::endl;
          }
        }
      }

      assert (NR[currentSplit.axis] <= primitveCount);

      currentCost = SAH(v, currentSplit , NL[currentSplit.axis], NR[currentSplit.axis], NP[currentSplit.axis]);
//       std::cout << currentSplit.pos << ":" << currentCost<< "," << NL[currentSplit.axis]<< "," << NR[currentSplit.axis]<< "," << NP[currentSplit.axis] << "\n";
      if ( currentCost < bestCost ) {
        bestCost = currentCost;
        bestSplit = currentSplit;
        bestSplit.putInPlaneLeft = currentCost.putInPlaneLeft;
      }
      NL[currentSplit.axis] += pStart;
      NL[currentSplit.axis] += pPlane;
      NP[currentSplit.axis] = 0;
      assert (NL[currentSplit.axis] <= primitveCount);

    }

    return bestSplit;
}


void KdTreeSahNlogN::subdivide( unsigned int *memBlock, unsigned int primitiveCount, const AABB nodeBox, unsigned int size, List<Event>& events ) {
  assert( size >= ( primitiveCount + 2 ) );
  
  if ( primitiveCount == 0) {
    KdNode::makeLeaf(memBlock, primitiveCount);
    return;
  }

  const KdTreeBase::SplitCandidate split = determineSplitpos(nodeBox, memBlock, primitiveCount, events);
  if ( split.axis == 3 ) {
      // no good split candidate found
    memBlock[primitiveCount] = memBlock[0];
    memBlock[primitiveCount+1] = memBlock[1];
    KdNode::makeLeaf(memBlock, primitiveCount);
    return;
  }

  const unsigned int *primitiveIds = memBlock;

  for ( unsigned int i = 0; i < primitiveCount; ++i)
    overlap[primitiveIds[i]] = BOTH;

  // classify triangles to be on left or right side only or on both
  for (List<Event>::iterator iter = events.begin(); iter != events.end(); ++iter) {
    const Event& event = (*iter);
    if ( event.axis == split.axis ) {
      if ( ( event.t <= split.pos ) && (event.type == END) )
        overlap[event.primId] = LEFT_ONLY;
      else if ( ( event.t >= split.pos ) && (event.type == START ) )
        overlap[event.primId] = RIGHT_ONLY;
      else if ( event.t == INPLANE ) {
        if ( split.putInPlaneLeft )
          overlap[event.primId] = LEFT_ONLY;
        else
          overlap[event.primId] = RIGHT_ONLY;
      }
    }
  }

  // seperate left- and right-only events
  List<Event> E_LO, E_RO;
  List<Event> E_BL, E_BR;
  for (List<Event>::iterator iter = events.begin(); iter != events.end(); ++iter) {
    const Event& event = (*iter);
    if ( overlap[event.primId] == LEFT_ONLY )
      E_LO.push_back(event);
    else if ( overlap[event.primId] == RIGHT_ONLY )
      E_RO.push_back(event);
//     else { // primitive overlaps both voxel, events have to bee added to both
//       if ( event.t < split.pos )
//         E_BL.push_back(event);
//       else
//         E_BR.push_back(event);
//     }
  }

  // generate new events for triangle straddling split plane
  for ( unsigned int i = 0; i < primitiveCount; ++i) {
    const unsigned int currentPrimitiveId = primitiveIds[i];
    const AABB& aabb = getAABBForPrimitiv(currentPrimitiveId);
    if ( overlap[currentPrimitiveId] == BOTH ) {
      E_BL.push_back(Event(fmaxf(nodeBox.getMin(split.axis), aabb.getMin(split.axis))     , START, split.axis, currentPrimitiveId));
      E_BL.push_back(Event(split.pos                   , END  , split.axis, currentPrimitiveId));
      E_BR.push_back(Event(split.pos                   , START, split.axis, currentPrimitiveId));
      E_BR.push_back(Event(fminf(nodeBox.getMax(split.axis), aabb.getMax(split.axis))     , END  , split.axis, currentPrimitiveId));

      for ( unsigned char otherAxis = (split.axis + 1) % 3 ; otherAxis != split.axis; otherAxis = (otherAxis + 1) % 3) {
        E_BL.push_back(Event(fmaxf(nodeBox.getMin(otherAxis), aabb.getMin(otherAxis))  , START  , otherAxis, currentPrimitiveId));
        E_BL.push_back(Event(fminf(nodeBox.getMax(otherAxis), aabb.getMax(otherAxis))  , END, otherAxis, currentPrimitiveId));
        E_BR.push_back(Event(fmaxf(nodeBox.getMin(otherAxis), aabb.getMin(otherAxis))  , START  , otherAxis, currentPrimitiveId));
        E_BR.push_back(Event(fminf(nodeBox.getMax(otherAxis), aabb.getMax(otherAxis))  , END, otherAxis, currentPrimitiveId));
      }
    }
  }
  sort(E_BL.begin(), E_BL.end());
  sort(E_BR.begin(), E_BR.end());

  List<Event>::iterator blIter = E_BL.begin();
  List<Event>::iterator loIter = E_LO.begin();

  List<Event> E_L, E_R;
  E_L.reserve(E_BL.size() + E_LO.size());
  E_R.reserve(E_BR.size() + E_RO.size());

  // merge lists for left subtree
  while ( ( blIter != E_BL.end() ) && ( loIter != E_LO.end() ))
    if ( (*blIter) < (*loIter) ) {
      E_L.push_back( (*blIter) );
      ++blIter;
    } else {
      E_L.push_back( (*loIter) );
      ++loIter;
    }

  if ( blIter != E_BL.end() )
    E_L.insert(E_L.end(), blIter, E_BL.end());
  else if ( loIter != E_LO.end() )
    E_L.insert(E_L.end(), loIter, E_LO.end());

  // merge lists for right subtree
  List<Event>::iterator brIter = E_BR.begin();
  List<Event>::iterator roIter = E_RO.begin();
  while ( ( brIter != E_BR.end() ) && ( roIter != E_RO.end() ))
    if ( (*brIter) < (*roIter) ) {
      E_R.push_back( (*brIter) );
      ++brIter;
    } else {
      E_R.push_back( (*roIter) );
      ++roIter;
    }

  if ( brIter != E_BR.end() )
    E_R.insert(E_R.end(), brIter, E_BR.end());
  else if ( roIter != E_RO.end() )
    E_R.insert(E_R.end(), roIter, E_RO.end());

  assert( (E_R.size() == (E_RO.size() + E_BR.size())));
  assert( (E_L.size() == (E_LO.size() + E_BL.size())));


  unsigned int *fallBack = (unsigned int*) malloc(size * sizeof(unsigned int));
  memcpy(fallBack, memBlock, sizeof(unsigned int) * primitiveCount);

  unsigned int leftAndRightStart = size;
  unsigned int rightOnlyStart = size;
  unsigned int lastUnclassified = primitiveCount - 1;

  unsigned int l = 0;
  for ( unsigned int i = 0; i < primitiveCount; ++i) {
    const AABB primAABB = scene.getPrimitive(memBlock[l]).getAABB();
    if ( primAABB.getMax(split.axis) <= split.pos) { // completely in left node
      ++l;
    } else  if ( primAABB.getMin(split.axis) >= split.pos) { // completely in right node
      --leftAndRightStart;
      --rightOnlyStart;
      memBlock[leftAndRightStart] = memBlock[rightOnlyStart];
      memBlock[rightOnlyStart] = memBlock[l];
      memBlock[l] = memBlock[lastUnclassified];
      --lastUnclassified;
    } else {// overlaps both nodes -> reference replication
      if ( (leftAndRightStart - lastUnclassified) < ( 3 * NODE_SIZE + (rightOnlyStart - leftAndRightStart)) ) {
         // not enough memory for 1 inner node & 2 leaves -> create leaf
        memcpy(memBlock + NODE_SIZE, fallBack, primitiveCount * sizeof(unsigned int));
        KdNode::makeLeaf(memBlock, primitiveCount);
        free(fallBack);
        return;
      }
      --leftAndRightStart;
      memBlock[leftAndRightStart] = memBlock[l];
      memBlock[l] = memBlock[lastUnclassified];
      --lastUnclassified;
    }
  }
  // |____left only__|___________________________|__left&right__|__right only__|
  const unsigned int leftOnly = l;// number of objects in left only block

  const unsigned int leftAndRight = (rightOnlyStart - leftAndRightStart);
  // not enough memory for 1 inner node & 2 leaves & replicated references -> create leaf
  // TODO: move this test completely into upper loop
  if ( (leftAndRightStart - l) < ( 3 * NODE_SIZE + leftAndRight) ) {
    memcpy(memBlock + NODE_SIZE, fallBack, primitiveCount * sizeof(unsigned int));
    KdNode::makeLeaf(memBlock, primitiveCount);
    free(fallBack);
    return;
  }
  free(fallBack);
  

  const unsigned int leftPrimitives = leftOnly + leftAndRight;
  const unsigned int rightPrimitives = size - leftAndRightStart;

  float p = (float)leftPrimitives / (leftPrimitives + rightPrimitives);
  memBlock[l] = memBlock[0];
  memBlock[l + 1] = memBlock[1];
  l += NODE_SIZE;
  // |NODE|__left only__|_______________________|__left&right__|__right only__|
  
  const unsigned int leftStart = NODE_SIZE;
  // copy 'left and right' block to end of 'left only' block
  memmove( memBlock + leftStart + leftOnly, memBlock + leftAndRightStart, leftAndRight * sizeof( unsigned int));
  // |NODE|__left only__|__left&right__|________|__left&right__|__right only__|

  //   for left node ends after: this node + left only + replicated references
  const unsigned int leftEnd    = leftStart + leftOnly + leftAndRight + NODE_SIZE;

  const unsigned int free       = leftAndRightStart - leftEnd - NODE_SIZE;

  // the amount of mem scheduled for the left child
  const unsigned int leftFree   = (unsigned int)(p * free);
  const unsigned int rightStart = leftEnd + leftFree;
  // shift content of ride node left to grant free mem for its children
  memmove( memBlock + rightStart, memBlock + leftAndRightStart, rightPrimitives * sizeof(unsigned int));
  // p = i.e. 0.5
  // |NODE|__left only__|__left&right__|____|__left&right__|__right only__|____|
  KdNode::makeInnernode(memBlock, split.axis, split.pos, rightStart << 2);
  subdivide(memBlock + leftStart, leftPrimitives, nodeBox.getHalfBox(split.axis, split.pos, true),  rightStart - NODE_SIZE, E_L );
  subdivide(memBlock + rightStart, rightPrimitives, nodeBox.getHalfBox(split.axis, split.pos, false),  size - rightStart, E_R );
}

