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
        DirectShader(const Vec3& color, const Scene& scene);

        ~DirectShader();

        virtual Vec3 getRadiance(const Vec3& direction, const Intersection& intersection) const;

    
private:
  const Vec3 color;

};
}
#endif
