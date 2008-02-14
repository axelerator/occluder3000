//
// C++ Interface: intersectionsse
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERINTERSECTIONSSE_H
#define OCCLUDERINTERSECTIONSSE_H

#include "assert.h"

#include "sse4.h"
#include "intersection.h"
#include "scene.h"

namespace Occluder {
class Scene;
/**
Stores four intersections at once in an SSE suitable SOA format.
In contrast to single intersections an IntersectionSSE is passed
to a primitive/ray intersection test and is potenially updated.
  @author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class IntersectionSSE {
public:
  IntersectionSSE();

  ~IntersectionSSE(){};
  void updateMask(int newHits);

  /**
    Updates u,t,v for the hits where t < this->t for each ray
    @param hitMask mask indicating which rays hit at all
    @param t parametric distance to the hit
    @param u surface coordinate
    @param v surface coordinate
    @param primIdx listindex of the primitive in the scene
   **/
  void updateHits(const Float4& hitMask, const Float4& t, 
      const Float4& u, const Float4& v, unsigned int primIdx);
  const Float4 getHitMask() const ;
  /**
    Calculates the radiance at the points of intersection into the given directions.
    @param ray for which radiance is to be calculated, pointing towards surface
    @param depth steps left until recursion will abort
    @return the calculated radiance
    **/  
  Vec3SSE getRadiance(const RaySegmentSSE& ray, unsigned int depth) const;

  /**
    In the case the intersections didnt happen with the same primitive
    they have to be handled uniquely.
    @param r index, which intersection of the four rays should be return
    @return an intersection
   **/
  const Intersection getSingleIntersection(unsigned int r) const;

  /**
    To avoid overhead position calculations ist
   **/
  void calcPositions(const RaySegmentSSE r);

  /**
    @return locations of the intersections
  **/
  const Vec3SSE getLocations() const;

  /**
    The ids of the primitives that the intersections did happen with
    are now going to be resolved into adresses.
    The final positions, are calculated now too
    @param scene the scene containing the primitives with the determined indieces
    @param ray the rays are necessary to calculate the final positions, which were
               calculated exclusivly in parametric form before.
    **/
  void resolvePrimitiveIds(const Scene& scene, const RaySegmentSSE& rays);
  
  /**
    @return the normals at the intersection points.
            this is only valid if all intersection 
            happend with the same primitive
    **/
  const Vec3SSE& getNormals() const;

  /**
    @return if none of the rays hit any primitive
   **/
  bool isEmpty() const;

private:

   Vec3SSE position;
   Float4 u,
          v;
   /**
      corresponds to t im ray equation: R = origin t * direction
      since only intersections in positive direction are interesting
      t < 0.0 indicates the 'empty'- or no-intersection case
   **/
   Float4 t; 

   /**
      During the search for intersections primitives are stored as int
      ids, because this way the information can be stored and processed
      in a 128bit SSE register.
    **/
   Float4 primitive;
  /**
    indicates which rays hit a primitive
   **/
  Float4 hitMask;
  
  /**
    indicates wether all rays hit the same primitive
   **/
  bool allSamePrimitive;

  /**
    Before determining radiance for the intersections
    the adresses of the primitives have to be known.
   **/
  const Primitive *primitivePointers[4];

  /**
    If all intersection happend with the same primitive,
    the normals at the points of intersection will be stored
    here, after the closest intersections have been determined.
    * BUT ONLY IF ALL INTERSECTIONS HIT THE SAME PRIM *
   **/
  Vec3SSE normals;
};

// --------------------- implementation of inlined methods ----------------------

inline IntersectionSSE::IntersectionSSE() :
  t(UNENDLICH), primitive((unsigned int)0), hitMask(0.0f), allSamePrimitive(false) {
  
}



inline const Float4 IntersectionSSE::getHitMask() const {
  return hitMask;
}

inline const Intersection IntersectionSSE::getSingleIntersection(unsigned int r) const {
  return Intersection(Vec3(position.get(r)), u.v.f[r], v.v.f[r], t.v.f[r], *(primitivePointers[r]));
}

inline  const Vec3SSE IntersectionSSE::getLocations() const {
  return position;
}

}

inline const Vec3SSE&  IntersectionSSE::getNormals() const {
  assert(allSamePrimitive);
  return normals;
}


inline bool IntersectionSSE::isEmpty() const {
  return hitMask == 0;
}
#endif
