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


private:



};
}
#endif
