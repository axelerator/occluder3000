//
// C++ Interface: shader
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SHADER_H
#define SHADER_H


#include "raysegment.h"


namespace Occluder {
class Scene;
class Intersection;
/**
Base class for all shaders aka materials.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Shader {
public:
    /**
        @param scene contains the scene this shader belongs to, to enable
                     the shader to look up global parameters like i.e. lights
     **/
    Shader(const Scene& scene);

    virtual ~Shader();
    /**
      Calculates the radiance for a point in space the is emitted into a
      certain direction.
      @param direction the direction for which the radiance is to be determined.
                 pointing towards the surface.
      @param intersection containing information about the point where radiance is to
                          be calculated at
      @return the calulated radiance
     **/
    virtual Vec3 getRadiance(const Vec3& direction, const Intersection& intersection) const = 0;

protected:
  const Scene& scene;


};
}
#endif
