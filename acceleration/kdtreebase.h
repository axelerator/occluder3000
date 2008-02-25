//
// C++ Interface: kdtreebase
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERKDTREEBASE_H
#define OCCLUDERKDTREEBASE_H

#include "accelerationstructure.h"

namespace Occluder {
class KdNode;

    /**
    Basic Kd-Tree implementation

    	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
    */
class KdTreeBase : public AccelerationStructure {
public:
    KdTreeBase(const Scene& scene);

    virtual ~KdTreeBase();

    virtual bool hasIntersection(const RaySegment& ray) const;
    virtual const Intersection getFirstIntersection(const RaySegment& ray) const;
    virtual void construct();
    virtual Float4 haveIntersections(const RaySegmentSSE& ray) const;
    virtual void determineFirstIntersection(const RaySegmentSSE& ray, IntersectionSSE& result) const;
    virtual void getAllIntersections(const RaySegment& ray, List< const Intersection >& results) const;
private:
    void subdivide( unsigned int *memBlock, unsigned int primitiveCount, const AABB nodeBox, unsigned int size);
    Intersection traverseRecursive( const KdNode& node, const RaySegment& r, float tmin, float tmax) const;

  unsigned int *memBlock;
};

}

#endif
