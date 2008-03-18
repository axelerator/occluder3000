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

#include "accelerationstructure.h"

namespace Occluder {
class KdNode;

/**
 * Basic Kd-Tree implementation
 * @author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class KdTreeBase : public AccelerationStructure {
public:
  KdTreeBase(const Scene& scene);

  virtual ~KdTreeBase();

  virtual bool hasIntersection(const RaySegment& ray) const;
  virtual const Intersection getFirstIntersection(const RaySegment& ray) const;
  virtual void construct();
  virtual Float4 haveIntersections(const RaySegmentSSE& ray) const;
  virtual void determineFirstIntersection(const RaySegmentSSE& ray, IntersectionSSE& result) const;
  virtual void getAllIntersections(const RaySegment& ray, List< const Intersection >& results) const;

  void writeToDisk() const;

protected:
  typedef struct {
    float pos;
    unsigned char axis;
    bool putInPlaneLeft;
  } SplitCandidate ;

  class SAHCost {
  public:
    SAHCost() {}
    SAHCost(bool putInPlaneLeft, float cost): putInPlaneLeft(putInPlaneLeft), cost(cost) {}
    bool operator<(const SAHCost op) const;
    bool putInPlaneLeft;
    float cost;
  } ;

  enum Overlap { LEFT_ONLY, BOTH, RIGHT_ONLY };
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
    @return SAH costs for splittin a voxel and a hint wether to insert elements in the splittinplane
            into the left or the right subvoxel.
    **/
  static SAHCost SAH(const AABB& v,const KdTreeBase::SplitCandidate split, 
              const unsigned int N_L, const unsigned int N_R, const unsigned int N_P);

  /**
    In the base version the position is simply determined by splittin the voxel in the middle, along
    the longest axis ('spatial median')
    @param v axis-alignes bounding box of the voxel that is to be split
    @param primitives pointer to array of ids of contained primitives
    @param primitveCount number of contained primitives
    **/
  virtual SplitCandidate determineSplitpos(const AABB& v, const unsigned int *primitves, const unsigned int primitveCount);

  /**
    The kd-tree caches the AABBs pf the triangles since they are accessed often,
    and should not be computed every time.
    **/
  const AABB& getAABBForPrimitiv(unsigned int i);

  /**
    Determins if a primitive ends up in the left or right subvoxel
    @param primitiveID position of the primitive in th primitive list
    @param split specifies where the voxel is to be split
    @param aabb axis-aligned bounding box of the voxel
    @return wether the object intersects only the left, the right or both voxels
    **/
  Overlap classify(unsigned int primitiveID, const SplitCandidate split, const AABB &aabb);

  /**
    Determins if a primitive ends up in the left or right subvoxel, but uses just the
    overlpa of the primitives AABB and the voxel for decision
    @param primitiveID position of the primitive in th primitive list
    @param split specifies where the voxel is to be split
    @param aabb axis-aligned bounding box of the voxel
    @return wether the object intersects only the left, the right or both voxels
    **/
  Overlap classifyVague(unsigned int primitiveID, const SplitCandidate split, const AABB &aabb);

  /**
    Deriving classes implementing an own construction this method enables the 
    caching of the primitives AABBs.
   **/
  void cachePrimitiveAABBs();

  /**
    Pointer to the start of the continous memory block the tree
    will be constructed and stored in
  **/
  unsigned int *memBlock;

  /**
    The (constant) size of the memblock. More space will not be
    taken by the tree.
  **/
  unsigned int totalSpace;

private:

  void subdivide( unsigned int *memBlock, unsigned int primitiveCount, const AABB nodeBox, unsigned int size);
  Intersection traverseRecursive( const KdNode& node, const RaySegment& r) const;

  AABB *primitiveBoxes;


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

inline const AABB& KdTreeBase::getAABBForPrimitiv(unsigned int i) {
  return primitiveBoxes[i];
}

inline float KdTreeBase::cost(const float P_L, const float P_R, const unsigned int N_L, const unsigned int N_R) {
  return C_trav + C_intersect * ( P_L * N_L + P_R * N_R );
}

inline KdTreeBase::SAHCost KdTreeBase::SAH(const AABB& v,const KdTreeBase::SplitCandidate split, 
              const unsigned int N_L, const unsigned int N_R, const unsigned int N_P) {
  const AABB v_l(v.getHalfBox(split.axis, split.pos, true));
  const AABB v_r(v.getHalfBox(split.axis, split.pos, false));
  
  const float oneOverV = 1.0f / v.getSurfaceArea();
  const float P_L = v_l.getSurfaceArea() * oneOverV;
  const float P_R = v_r.getSurfaceArea() * oneOverV;

  if ( N_L == 0 )
    return SAHCost(true, cost(P_L, P_R, N_L, N_R + N_P));
  return SAHCost(false,cost(P_L, P_R, N_L + N_P, N_R));
}

inline bool KdTreeBase::SAHCost::operator<(const SAHCost op) const {
  return cost < op.cost;
}

}

#endif
