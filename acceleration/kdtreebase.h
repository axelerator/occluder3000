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

#include <accelerationstructure.h>

namespace Occluder {
class KdNodeBloated;
    /**
    Basic Kd-Tree implementation

    	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
    */
class KdTreeBase : public AccelerationStructure {
public:
        KdTreeBase(const Scene& scene, unsigned int sampleCount = 8);

        ~KdTreeBase();

        virtual bool hasIntersection(const RaySegment& ray) const;
        virtual const Intersection getFirstIntersection(const RaySegment& ray) const;
        virtual void construct();

private:
  KdNodeBloated *subdivide(List<unsigned int> primitives, const AABB& aabb);

  AABB *primitiveBBs;
  KdNodeBloated *root;

  const unsigned int sampleCount;
};

typedef struct {
  float pos;
  unsigned char axis;
} SplitCandidate;

}

#endif
