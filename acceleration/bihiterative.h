//
// C++ Interface: bihiterative
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERBIHITERATIVE_H
#define OCCLUDERBIHITERATIVE_H

#include "accelerationstructure.h"
#include "stack.h"
#include "bihnode.h"

namespace Occluder {

/**
Bih implentation using the compact node representation and traverses the bih in an iterative instead of an recursive manner.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class BihIterative : public AccelerationStructure
{
public:
    BihIterative(const Scene& scene, unsigned int primPerLeaf);

    virtual bool hasIntersection(const RaySegment& ray) const;
    virtual const Intersection getFirstIntersection(const RaySegment& ray) const;
    virtual Float4 haveIntersections(const RaySegmentSSE& ray) const;
    virtual void construct();
    virtual void determineFirstIntersection(const RaySegmentSSE& ray, IntersectionSSE& result) const;
    virtual void getAllIntersections(const RaySegment& ray, List< const Intersection >& results) const;
private:
  void subdivide(BihNodeCompact *node, unsigned int start, unsigned int end, const AABB& nodeAABB, unsigned int depth);
  Intersection traverseRecursive(const BihNodeCompact& node, const RaySegment& r, float tmin, float tmax) const;
  bool traverseRecursiveShadow(const BihNodeCompact& node, const RaySegment& r, float tmin, float tmax) const;
  unsigned int *primIds;
  const unsigned int minPrimPerLeaf;
  BihNodeCompact *nodes;
  BihNodeCompact *nextFree;

  /**
    Stack element for iterative traversal
   **/
  class Stacknode {
    public:
      Stacknode(){}
      Stacknode(const BihNodeCompact *node, float tmin, float tmax):
      node(node), tmin(tmin), tmax(tmax){
      assert(node != 0);
      }
      Stacknode& operator=(const Stacknode& op) {
        memcpy(this, &op, sizeof(Stacknode));
        return *this;
      }
      const BihNodeCompact *node;
      float tmin,tmax;
  } ;

  mutable Stack<Stacknode> traversalStack;
};




}

#endif
