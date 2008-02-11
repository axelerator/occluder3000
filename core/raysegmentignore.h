//
// C++ Interface: raysegmentignore
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERRAYSEGMENTIGNORE_H
#define OCCLUDERRAYSEGMENTIGNORE_H

#include <raysegment.h>

namespace Occluder {
    class Primitive;
    /**
    A special case of a ray, where a reference to primitive is stored along with the ray for which an intersection will be ignored. This is used to avoid self intersection of rays which originate at a surface.

    	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
    */
class RaySegmentIgnore : public RaySegment {
public:
        RaySegmentIgnore(const Vec3& origin, const Vec3& direction, const Primitive& ignore, float tmin, float tmax);

        ~RaySegmentIgnore();

        /**
           @return the primitive this ray is defined not to intersect with.
         **/
        const Primitive& getIgnoredPrimitve() const;
private:
        const Primitive& ignore;


    };


// ------------------- implementation of inlined methods -------------------------------

    inline const Primitive& RaySegmentIgnore::getIgnoredPrimitve() const {
        return ignore;
    }

}

#endif
