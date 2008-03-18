//
// C++ Interface: kdtreesahnlogn
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERKDTREESAHNLOGN_H
#define OCCLUDERKDTREESAHNLOGN_H

#include "kdtreebase.h"
namespace Occluder {

    /**
    Kd-tree constructed wit SAH in O(N log N) based on
    "On building fast kd-Trees for Ray Tracing, and on doing that in O(N log N)"

    	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
    */
class KdTreeSahNlogN : public KdTreeBase {
public:
  KdTreeSahNlogN(const Scene& scene);
  virtual void construct();
  virtual ~KdTreeSahNlogN();

protected:
   
  enum EventType { END, INPLANE, START };

  class Event {
    public:
    Event(float t, EventType type, unsigned char axis, unsigned int primId): t(t), type(type), axis(axis), primId(primId) {}
  
    bool operator<(const Event& e) const {
//     return (t < e.t) || ( t == e.t && type < e.type) || ( t == e.t && type == e.type && axis < e.axis )  ;
    return (t < e.t) || (t == e.t && axis < e.axis) || (( t == e.t && axis == e.axis) && (type < e.type ) )  ;
    }
  
    float t;
    EventType type;
    unsigned char axis;
    unsigned int primId;
  };

  SplitCandidate determineSplitpos(const AABB& v, const unsigned int* primitves, const unsigned int primitveCount, List<Event>& events );
  void subdivide(  unsigned int *memBlock, unsigned int primitiveCount, const AABB nodeBox, unsigned int size, List<Event>& events );

  /**
    During cunstruction for each primtive has to be stored which
    voxel during the current subdivision step it overlaps
  **/
  Overlap *overlap;
};

}

#endif
