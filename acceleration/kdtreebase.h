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

protected:
    typedef struct {
      float pos;
      unsigned char axis;
    } SplitCandidate ;

    virtual SplitCandidate determineSplitpos(const AABB& v, const unsigned int *primitves, const unsigned int primitveCount);
    const AABB& getAABBForPrimitiv(unsigned int i);
private:

    void subdivide( unsigned int *memBlock, unsigned int primitiveCount, const AABB nodeBox, unsigned int size);
    Intersection traverseRecursive( const KdNode& node, const RaySegment& r) const;

  unsigned int *memBlock;
  AABB *primitiveBoxes;
};

inline const AABB& KdTreeBase::getAABBForPrimitiv(unsigned int i) {
  return primitiveBoxes[i];
}

}

#endif
