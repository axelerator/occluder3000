//
// C++ Interface: kdtreesahnlog2n
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERKDTREESAHNLOG2N_H
#define OCCLUDERKDTREESAHNLOG2N_H

#include <kdtreebase.h>

namespace Occluder {

/**
This Kd-tree is constructed with the SAH version of the pbrt book in O(Nlog²N).

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class KdTreeSahNlog2N : public KdTreeBase
{
public:
    KdTreeSahNlog2N(const Scene& scene);

    ~KdTreeSahNlog2N();

protected:
    virtual SplitCandidate determineSplitpos(const AABB& v, const unsigned int* primitves, const unsigned int primitveCount);

    enum EventType { END, INPLANE, START };
    
    class Event {
      public:
      Event(float t, EventType type): t(t), type(type) {}
    
      bool operator<(const Event& e) const {
      return (t < e.t) || ( t == e.t && type < e.type);
      }
    
      float t;
      EventType type;
    };

};

}

#endif
