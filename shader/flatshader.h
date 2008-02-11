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
    FlatShader(const std::string& name, const Scene& scene, const Vec3& color = Vec3(1.0f));

    virtual ~FlatShader();

    virtual Vec3 getRadiance(const Vec3& direction, const Intersection& intersection, unsigned int depth) const;
    virtual void setPropertyFromString(const std::string& key, const std::string& value );
private:
  Vec3 color;

};

}

#endif
