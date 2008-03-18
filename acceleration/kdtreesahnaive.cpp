//
// C++ Implementation: kdtreesahnaive
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "kdtreesahnaive.h"
#include "scene.h"

using namespace Occluder;




KdTreeSAHNaive::KdTreeSAHNaive(const Scene& scene): KdTreeBase(scene) {
}


KdTreeSAHNaive::~KdTreeSAHNaive() {
}


KdTreeBase::SplitCandidate KdTreeSAHNaive::determineSplitpos(const AABB& v, const unsigned int *primitves, const unsigned int primitveCount) {

    // determine splitposition on half of longest axis
    SAHCost bestCost(false, 1000.0f);
    unsigned int n_l, n_r, n_p;
    SplitCandidate currentSplit;
    SplitCandidate bestSplit = { 0.0f, 3, false};
    for (unsigned int axis = 0; axis < 3; ++axis) {
        currentSplit.axis = axis;
        for ( unsigned int p = 0; p < primitveCount; ++p) {
            for (unsigned int minOrMax = 0; minOrMax < 2; ++minOrMax) {

                currentSplit.pos = minOrMax ? getAABBForPrimitiv(primitves[p]).getMin(axis):getAABBForPrimitiv(primitves[p]).getMax(axis);
                if (currentSplit.pos > v.getMin(axis) && currentSplit.pos < v.getMax(axis) ) {
                    n_l = n_r = n_p = 0;
                    for ( unsigned int r = 0; r < primitveCount; ++r) {
                        const AABB& primBox = getAABBForPrimitiv(primitves[r]);
                        if ( primBox.getMin(currentSplit.axis) < currentSplit.pos )
                            ++n_l;
                        if ( primBox.getMax(currentSplit.axis) > currentSplit.pos )
                            ++n_r;
                        if ( primBox.getMax(currentSplit.axis) == currentSplit.pos && primBox.getMin(currentSplit.axis) == currentSplit.pos )
                            ++n_p;
                    }

                    const SAHCost currentCost = SAH(v, currentSplit, n_l, n_r, n_p);
                    if ( currentCost < bestCost) {
                        bestCost = currentCost;
                        bestSplit = currentSplit;
                        bestSplit.putInPlaneLeft = currentCost.putInPlaneLeft;
                    }
                }
            }
        }
    }

    return bestSplit;
}



