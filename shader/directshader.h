//
// C++ Interface: directshader
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERDIRECTSHADER_H
#define OCCLUDERDIRECTSHADER_H

#include <shader.h>

namespace Occluder {

    /**
    Calculates radiance that is recieved directly from light sources

    	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
    */
class DirectShader : public Shader {
public:
    DirectShader(const std::string& name, const Scene& scene, const Vec3& color = Vec3(1.0f));

    ~DirectShader();

    virtual Vec3 getRadiance(const Vec3& direction, const Intersection& intersection, unsigned int depth) const;
    virtual Vec3SSE getRadiance( const Vec3SSE& directions,
                              const IntersectionSSE& intersections, unsigned int depth) const;
    virtual void setPropertyFromString(const std::string& key, const std::string& value );
    
private:
  Vec3 color;

};
}
#endif
