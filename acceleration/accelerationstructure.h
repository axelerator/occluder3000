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

#include "scene.h"
#include "raysegment.h"
#include "intersection.h"
#include "list.h"
namespace Occluder {
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
    virtual bool hasIntersection(const RaySegment& ray) = 0;

    /**
      Searches the first intersection of the ray with the scene.
      Needed for primary ray and the photon tracing stage.
      @return An intersection object ( containing info about location ,u,v of hit). 
              Can be the 'empty' intersection.
    **/
    virtual const Intersection getFirstIntersection(const RaySegment& ray) = 0;

    /**
      @return All intersections of the raysegment with the scene 
    **/
    virtual void getAllIntersections(const RaySegment& ray, List<const Intersection>& results) = 0;

protected:
  const Scene& scene;
};
}
#endif
