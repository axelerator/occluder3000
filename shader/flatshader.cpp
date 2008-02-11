//
// C++ Implementation: flatshader
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "flatshader.h"

using namespace Occluder;

FlatShader::FlatShader(const std::string& name, const Scene& scene, const Vec3& color)
: Shader(name, scene), color(color) {
}


FlatShader::~FlatShader() {
}


Vec3 FlatShader::getRadiance(const Vec3& direction, const Intersection& intersection, unsigned int depth) const {
  return color;
}


void FlatShader::setPropertyFromString(const std::string& key, const std::string& value ) {
  if ( key == "color" )
    color = Vec3( value.c_str());
  else
    Shader::setPropertyFromString(key, value);
}
