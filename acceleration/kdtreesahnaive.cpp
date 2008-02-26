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

const float KdTreeSAHNaive::C_trav = 0.1f;
const float KdTreeSAHNaive::C_intersect = 1.0f;


KdTreeSAHNaive::KdTreeSAHNaive(const Scene& scene): KdTreeBase(scene) {
}


KdTreeSAHNaive::~KdTreeSAHNaive() {
}


KdTreeBase::SplitCandidate KdTreeSAHNaive::determineSplitpos(const AABB& v, const unsigned int *primitves, const unsigned int primitveCount) {
  // determine longest axis of bounding box
  const Vec3 aabbWidth( v.getWidths() );
  unsigned char axis = 0;
  axis = ( aabbWidth[1] > aabbWidth[0] )    ? 1 : 0;
  axis = ( aabbWidth[2] > aabbWidth[axis] ) ? 2 : axis;

  // determine splitposition on half of longest axis
  float bestSplit;
  float bestCost;
  unsigned int n_l, n_r, n_p;
  KdTreeBase::SplitCandidate currentSplit;
  float currentCost;
  currentSplit.axis = axis;
  for ( unsigned int p = 0; p < primitveCount; ++p) {
    currentSplit.pos = getAABBForPrimitiv(primitves[p]).getMin(axis);
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

    currentCost = SAH(v, currentSplit, n_l, n_r, n_p); 
    if ( currentCost < bestCost) {
      bestCost = currentCost;
      bestSplit = currentSplit.pos;
    }

  }
  const KdTreeBase::SplitCandidate result = {bestSplit, axis};
  return result;
}



