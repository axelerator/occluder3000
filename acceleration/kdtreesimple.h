//
// C++ Interface: kdtreesimple
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERKDTREESIMPLE_H
#define OCCLUDERKDTREESIMPLE_H

#include "accelerationstructure.h"

namespace Occluder {
class KdNodeBloated;
/**
Simple Kd-tree implementation with legere node layout. Construction uses a simple spatial median split for dividing nodes.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class KdTreeSimple : public AccelerationStructure
{
public:
    KdTreeSimple(const Scene& scene, unsigned int minPrimPerLeaf = 2, unsigned int maxDepth = 32);
    virtual bool hasIntersection(const RaySegment& ray) const;
    virtual const Intersection getFirstIntersection(const RaySegment& ray) const;
    virtual void construct();
    virtual Float4 haveIntersections(const RaySegmentSSE& ray) const;
    virtual void determineFirstIntersection(const RaySegmentSSE& ray, IntersectionSSE& result) const;
    virtual void getAllIntersections(const RaySegment& ray, List< const Intersection >& results) const;


    virtual ~KdTreeSimple();
private:
  KdNodeBloated *subdivide(const List<unsigned int>& primitives, const AABB& aabb, unsigned int depth);
  Intersection traverseRecursive( const KdNodeBloated& node, const RaySegment& ray, float tmin, float tmax) const ;

  AABB *primitiveBBs;
  KdNodeBloated *root;
  unsigned int primCount;
  const unsigned int minPrimPerLeaf;
  const unsigned int maxDepth;
};

}

#endif
