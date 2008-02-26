//
// C++ Interface: kdtreesahnaive
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERKDTREESAHNAIVE_H
#define OCCLUDERKDTREESAHNAIVE_H

#include <kdtreebase.h>

namespace Occluder {

/**
KdTree which uses the Surface Area Heuristic to obtain better split than simple spatial median. It compares the costs of all splitplanes at primitive borders, which results in an O(n²) complexity

  @author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class KdTreeSAHNaive : public KdTreeBase {
public:
    KdTreeSAHNaive(const Scene& scene);

    ~KdTreeSAHNaive();
protected:
    virtual SplitCandidate determineSplitpos(const AABB& v, const unsigned int *primitves, const unsigned int primitveCount);

    /**
      Caluclate the cost for a split.
      @param P_L probability that a ray hits left voxel
      @param P_R probability that a ray hits right voxel
      @param N_L number of primitives in left voxel
      @param N_R number of primitives in right voxel
      @return the cost for a split with the given parameters
     **/
    static float cost(const float P_L, const float P_R, const unsigned int N_L, const unsigned int N_R);

    /**
      @param v current voxel
      @param spit axis and position of the split candidate
      @param N_L number of primitives in left voxel
      @param N_R number of primitives in right voxel
      @param N_P number of primitives being orthogonal and in splitplane
     **/
    static float SAH(const AABB& v,const KdTreeBase::SplitCandidate split, 
               const unsigned int N_L, const unsigned int N_R, const unsigned int N_P);
private:


  /**
     Cost constant to do a traversal step.
   **/
  static const float C_trav;
  /**
    Cost constant for intersecting the ray with a primitive.
    Of course only constant as long there is just one primitive type.
   **/
  static const float C_intersect;

};

// ----------------------- implementation of inlined methods -----------------------------------

inline float KdTreeSAHNaive::cost(const float P_L, const float P_R, const unsigned int N_L, const unsigned int N_R) {
  return C_trav + C_intersect * ( P_L * N_L + P_R * N_R );
}

}

inline float KdTreeSAHNaive::SAH(const AABB& v,const KdTreeBase::SplitCandidate split, 
              const unsigned int N_L, const unsigned int N_R, const unsigned int N_P) {
  const AABB v_l(v.getHalfBox(split.axis, split.pos, true));
  const AABB v_r(v.getHalfBox(split.axis, split.pos, false));
  
  const float oneOverV = 1.0f / v.getSurfaceArea();
  const float P_L = v_l.getSurfaceArea() * oneOverV;
  const float P_R = v_r.getSurfaceArea() * oneOverV;

  if ( N_L == 0 )
    return cost(P_L, P_R, N_L, N_R + N_P);
  return cost(P_L, P_R, N_L + N_P, N_R);
}


#endif
