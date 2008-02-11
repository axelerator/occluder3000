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
#include "sse4.h"

namespace Occluder {
class Scene;
class Intersection;
class IntersectionSSE;
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
    Shader( const std::string& name, const Scene& scene );

    virtual ~Shader();
    /**
      Calculates the radiance for a point in space the is emitted into a
      certain direction.
      @param direction the direction for which the radiance is to be determined.
                 pointing towards the surface.
      @param intersection containing information about the point where radiance is to
                          be calculated at
      @param dept recursion steps left until recursion has to stop
      @return the calulated radiance
     **/
    virtual Vec3 getRadiance(const Vec3& direction, const Intersection& intersection, unsigned int depth) const = 0;

    /**
      Calculates the radiance for four points, each into one direction, utilizing SSE instructions.
      @param directions the direction for each point for which radiance is evaluated
      @param intersections the for points plus addintional data for which radiance is
                           to be calculated.
      @param dept recursion steps left until recursion has to stop
     **/
    virtual Vec3SSE getRadiance( const Vec3SSE& directions, 
                                 const IntersectionSSE& intersections, unsigned int depth) const {return Vec3SSE (Vec3(1.0f));};

    virtual void setPropertyFromString(const std::string& key, const std::string& value );

protected:
  const std::string name;
  const Scene& scene;

};
}
#endif
