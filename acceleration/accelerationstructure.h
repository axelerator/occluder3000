//
// C++ Interface: accelerationstructure
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ACCELERATIONSTRUCTURE_H
#define ACCELERATIONSTRUCTURE_H


#include "raysegment.h"
#include "intersection.h"
#include "list.h"
namespace Occluder {
class Scene;
/**
    Contains the geometry of a scene.
    Base class for all structures constructed to accellerate the process of 
    finding ray/triangle-intersections.

    @author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class AccelerationStructure {
public:
    AccelerationStructure( const Scene& scene );

    virtual ~AccelerationStructure();
    /**
      Determines if the raysegment is intersected by any primitiv.
      Primararily used for shadow rays.
      @return true if the ray hit a primitive between tmin and tmax.
    **/
    virtual bool hasIntersection(const RaySegment& ray) const = 0;

    /**
      Determines if the raysegment is intersected by any primitiv.
      Primararily used for shadow rays. To avoid self intersection the
      ray contains a reference to the primitive of which's surface it's
      originating from.
      @return true if the ray hit a primitive between tmin and tmax,
              and is not the referenced primitive
    **/
    virtual bool hasIntersection(const RaySegmentIgnore& ray) const = 0;

    /**
      Searches the first intersection of the ray with the scene.
      Needed for primary ray and the photon tracing stage.
      @return An intersection object ( containing info about location ,u,v of hit). 
              Can be the 'empty' intersection.
    **/
    virtual const Intersection getFirstIntersection(const RaySegment& ray)  const = 0;

    /**
      @return All intersections of the raysegment with the scene 
    **/
    virtual void getAllIntersections(const RaySegment& ray, List<const Intersection>& results) const  = 0;

    /**
      same as @see AccelerationStructure#hasIntersection(const RaySegmentIgnore& ray) but the operation
      is performed on four rays in parallel with SSE instructions.
     **/
    virtual void determineFirstIntersection(const RaySegmentSSE& ray, IntersectionSSE& result) const = 0;

protected:
  const Scene& scene;
};

}
#endif
