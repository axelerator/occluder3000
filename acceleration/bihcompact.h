//
// C++ Interface: bihcompact
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERBIHCOMPACT_H
#define OCCLUDERBIHCOMPACT_H

#include "accelerationstructure.h"
#include "bihnode.h"

namespace Occluder {

    /**
    Bih implementation that uses the compact representation of a bih node.

    	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
    */
class BihCompact : public AccelerationStructure {
public:

  BihCompact(const Scene& scene, unsigned int primPerLeaf);
  
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
};
}

#endif
