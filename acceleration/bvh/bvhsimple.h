//
// C++ Interface: bvhsimple
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERBVHSIMPLE_H
#define OCCLUDERBVHSIMPLE_H

#include "accelerationstructure.h"
#include "bvhnodesimple.h"

namespace Occluder {

/**
  A basic implementation of a bounding volume hierarchy
	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
 **/
class BvhSimple : public AccelerationStructure {
  public:
    BvhSimple(const Scene& scene);

    ~BvhSimple();

    virtual bool hasIntersection(const RaySegment& ray) const;
    virtual const Intersection getFirstIntersection(const RaySegment& ray) const;
    virtual void construct();
    
    virtual void getAllIntersections(const RaySegment& ray, List<const Intersection>& results) const {};
    virtual void determineFirstIntersection(const RaySegmentSSE& ray, IntersectionSSE& result) const {} ;
    virtual Float4 haveIntersections(const RaySegmentSSE& ray) const {return 0.0f;};

    /**
      prints some statistics
     **/
    void analyze();
  private:
    BvhNodeSimple *subdivide(const List<unsigned int>& primitives);



    /**
      root node of the tree for the BVH
     **/
    BvhNodeSimple *root;
};

}

#endif
