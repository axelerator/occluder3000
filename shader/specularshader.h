//
// C++ Interface: specularshader
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERSPECULARSHADER_H
#define OCCLUDERSPECULARSHADER_H

#include "directshader.h"

namespace Occluder {

    /**
    Includes perfect specular reflection in radiance calculation.

    	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
    */
class SpecularShader : public DirectShader {
public:
        SpecularShader(const std::string& name, const Scene& scene, const Vec3& color = Vec3(0.0f), const float reflectivity = 1.0f, const float alpha = 1.0f, const float refIdx = 1.0f);


        ~SpecularShader();

        virtual Vec3 getRadiance(const Vec3& direction, const Intersection& intersection, unsigned int depth) const;
        virtual void setPropertyFromString(const std::string& key, const std::string& value );
private:
  /**
    0.0 = no specular reflection
    1.0 = completely specular
   **/
  float reflectivity; 

  /**
    0.0 = completely transmittive
   **/
  float alpha;

  /**
    refractive index
   **/
  float refIdx;
};
}

#endif
