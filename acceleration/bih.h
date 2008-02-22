//
// C++ Interface: bih
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERBIH_H
#define OCCLUDERBIH_H

#include <accelerationstructure.h>
#include "bihnodebloated.h"

namespace Occluder {

/**
Bounding Interval Hierarhy to accelerate ray queries.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Bih : public AccelerationStructure
{
public:
    Bih(const Scene& scene, unsigned int minPrimPerLeaf = 1);

    ~Bih();

    virtual bool hasIntersection(const RaySegment& ray) const;
    virtual const Intersection getFirstIntersection(const RaySegment& ray) const;
    virtual Float4 haveIntersections(const RaySegmentSSE& ray) const;
    virtual void construct();
    virtual void determineFirstIntersection(const RaySegmentSSE& ray, IntersectionSSE& result) const;
    virtual void getAllIntersections(const RaySegment& ray, List< const Intersection >& results) const;

private:

    BihNode* subdivide(unsigned int start, unsigned int end, const AABB& nodeAABB, unsigned int depth, unsigned int retry);
    Intersection traverseRecursive(const BihNode&, const RaySegment& r, float tmin, float tmax) const;
    bool traverseRecursiveShadow(const BihNode&, const RaySegment& r, float tmin, float tmax) const;


    unsigned int *primIds;
    const unsigned int minPrimPerLeaf;
    BihNode *root;
};

}



#endif
