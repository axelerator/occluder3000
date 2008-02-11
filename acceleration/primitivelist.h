//
// C++ Interface: primitivelist
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PRIMITIVELIST_H
#define PRIMITIVELIST_H

#include <accelerationstructure.h>
namespace Occluder {
/**
Does no acceleration at all. finds intersection by sequentially scanning object list.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class PrimitiveList : public AccelerationStructure
{
public:
    PrimitiveList( const Scene& scene );

    ~PrimitiveList();

    virtual bool hasIntersection(const RaySegment& ray) const;
    virtual bool hasIntersection(const RaySegmentIgnore& ray) const;
    virtual const Intersection getFirstIntersection(const RaySegment& ray) const;
    virtual void getAllIntersections(const RaySegment& ray, List< const Intersection >& results) const;
    virtual void determineFirstIntersection(const RaySegmentSSE& ray, IntersectionSSE& result) const ;
};
}
#endif
