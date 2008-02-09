//
// C++ Interface: flatshader
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERFLATSHADER_H
#define OCCLUDERFLATSHADER_H

#include <shader.h>

namespace Occluder {

/**
Shades the object with a constant color. Does not take any lightsources into account.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class FlatShader : public Shader
{
public:
    FlatShader(const Vec3& color, const Scene& scene);

    virtual ~FlatShader();

    virtual Vec3 getRadiance(const Vec3& direction, const Intersection& intersection) const;

private:
  Vec3 color;

};

}

#endif
