//
// C++ Implementation: kdtreesahnlog2n
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "kdtreesahnlog2n.h"
#include <assert.h>
#include "scene.h"
#include <iostream>
using namespace Occluder;

KdTreeSahNlog2N::KdTreeSahNlog2N(const Scene& scene): KdTreeBase(scene) {
}


KdTreeSahNlog2N::~KdTreeSahNlog2N() {
}

// TODO: make satic memeber of base class
static const Vec3 AXIS[] = { Vec3( 1.0, 0.0, 0.0), Vec3( 0.0, 1.0, 0.0), Vec3( 0.0, 0.0, 1.0) };
  

KdTreeBase::SplitCandidate KdTreeSahNlog2N::determineSplitpos(const AABB& v, const unsigned int* primitves, const unsigned int primitveCount) {

  SAHCost bestCost(false, 10000.0f);

  SAHCost currentCost;
  SplitCandidate currentSplit;
  SplitCandidate bestSplit = { 0.0f, 3, false};
  bestSplit.axis = 3; // to mark that there's been no good one yet

  for (unsigned int axis = 0; axis < 3; ++axis) {
    List<KdTreeSahNlog2N::Event>events;
    events.reserve(primitveCount * 2);
  
    for ( unsigned int p = 0; p < primitveCount; ++p) {
      const Primitive& currentPrimitive = scene.getPrimitive(primitves[p]);
      const AABB& currBB = getAABBForPrimitiv(primitves[p]);
      if ( (currentPrimitive.getNormal() == AXIS[axis]) 
          ||  (( -1.0 * currentPrimitive.getNormal()) == AXIS[axis])) {
          events.push_back(Event(fmaxf(currBB.getMin(axis), v.getMin(axis)), INPLANE));
      } else {
          events.push_back(Event(fmaxf(currBB.getMin(axis), v.getMin(axis)), START));
          events.push_back(Event(fminf(currBB.getMax(axis), v.getMax(axis)), END));
      }
    }
  
    sort(events.begin(), events.end());
    unsigned int inplane = 0, leftOf = 0, rightOf = primitveCount;
    currentSplit.axis = axis;
    unsigned int dend, dstart, dplane;
    List<Event>::const_iterator iter = events.begin();

//     for (; iter != events.end(); ++iter) {
//       std::cout << (*iter).t << ":";
//       switch((*iter).type) {
//         case START: std::cout << "START";break;
//         case END: std::cout << "END";break;
//         case INPLANE: std::cout << "PLANE";break;
//       }
//     std::cout << "\n";
//     }
//     iter = events.begin();
    while (iter != events.end()) {
      dend = dstart = dplane = 0;
      currentSplit.pos = (*iter).t;

      while ( (iter != events.end()) && ((*iter).t == currentSplit.pos ) && (*iter).type == END ) {
        ++dend;
        ++iter;
      }
      
      while ( (iter != events.end()) && ((*iter).t == currentSplit.pos ) && (*iter).type == INPLANE ) {
        ++dplane;
        ++iter;
      }
      
      while ( (iter != events.end()) && ((*iter).t == currentSplit.pos ) && (*iter).type == START ) {
        ++dstart;
        ++iter;
      }

      inplane = dplane;
      leftOf -= dplane;
      rightOf -= dend;
      currentCost =  SAH(v, currentSplit , leftOf, rightOf, inplane);
//       std::cout << currentSplit.pos << ":" << currentCost<< "," << leftOf << ","<< rightOf << ","<< inplane << "\n";

      if ( currentCost < bestCost ) {
        bestCost = currentCost;
        bestSplit = currentSplit;
        bestSplit.putInPlaneLeft = bestCost.putInPlaneLeft;
      }

      leftOf += dstart;
      leftOf += dplane;
      inplane = 0;

    }
  }
// std::cout << std::endl;
  return bestSplit;
}
