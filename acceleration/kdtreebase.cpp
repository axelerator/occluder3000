//
// C++ Implementation: kdtreebase
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "kdtreebase.h"
#include "scene.h"
#include <assert.h>

using namespace Occluder;

KdTreeBase::KdTreeBase(const Scene& scene, unsigned int sampleCount):
AccelerationStructure(scene),
primitiveBBs(0),
sampleCount(sampleCount) {

    assert( (sampleCount % 4) == 0 ); // must be multiple of four
    // to be able to use SSE register
}


KdTreeBase::~KdTreeBase() {
    if ( primitiveBBs )
        delete primitiveBBs;
}


bool KdTreeBase::hasIntersection(const RaySegment& ray) const {
  return false;
}

const Intersection KdTreeBase::getFirstIntersection(const RaySegment& ray) const {
  return Intersection::getEmpty();
}

void KdTreeBase::construct() {
    // malloc not new because we want no contructor calls here
    primitiveBBs = (AABB*)malloc(scene.getPrimitiveCount() * sizeof(AABB));
    const unsigned int primCount = scene.getPrimitiveCount();
    for ( unsigned int i = 0; i < primCount; ++i)
        primitiveBBs[i] = scene.getPrimitive(i).getAABB();
}

KdNodeBloated *KdTreeBase::subdivide(List<unsigned int> primitives, const AABB& aabb) {

    const Vec3 aabbWidth( aabb.getWidths() );
    unsigned char axis = 0;
    if ( aabbWidth[1] > aabbWidth[2] )
      axis = 1;
    if ( aabbWidth[2] > aabbWidth[axis] )
      axis = 2;

    const float samplePosDelta = aabbWidth[axis] / ( sampleCount + 1 );

    float *candidates;
    candidates = (float*)malloc( sampleCount * sizeof(float));
    unsigned int *CL, *CR;
    CL = (unsigned int*)malloc( sampleCount * sizeof(int));
    CR = (unsigned int*)malloc( sampleCount * sizeof(int));
    memset(&CL, 0, sampleCount * sizeof(int));
    memset(&CR, 0, sampleCount * sizeof(int));
    candidates[0] = aabb.getMin(axis) + samplePosDelta;
    for ( unsigned int i = 1; i < sampleCount; ++i) {
      candidates[i] = candidates[i - 1] + samplePosDelta;
    }

    const unsigned int primCount = scene.getPrimitiveCount();
//     const unsigned int sets = sampleCount / 4;

    for ( unsigned int sample = 0; sample < sampleCount; ++sample) {
        for (unsigned int primId = 0; primId < primCount; ++primId) {
                const AABB& currAABB = primitiveBBs[primId];
                CR += ( candidates[sample] < currAABB.getMin(axis) ) ? 1 : 0;
                CL += ( candidates[sample] > currAABB.getMax(axis) ) ? 1 : 0;
        }
    }

    free(candidates);
    free(CL);
    free(CR);
    return 0;
}

