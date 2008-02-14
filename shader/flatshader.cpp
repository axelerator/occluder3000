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
#include "intersectionsse.h"

using namespace Occluder;

FlatShader::FlatShader(const std::string& name, const Scene& scene, const Vec3& color)
: Shader(name, scene), color(color) {
}


FlatShader::~FlatShader() {
}


Vec3 FlatShader::getRadiance(const Vec3& direction, const Intersection& intersection, unsigned int depth) const {
  return color;
}

Vec3SSE FlatShader::getRadiance( const Vec3SSE& directions,  const IntersectionSSE& intersections, unsigned int depth) const {
Float4 hitMask = intersections.getHitMask();
Vec3SSE result(color);
  result.c[0] = result.c[0] & hitMask; 
  result.c[1] = result.c[1] & hitMask; 
  result.c[2] = result.c[2] & hitMask; 
return result;
}


void FlatShader::setPropertyFromString(const std::string& key, const std::string& value ) {
  if ( key == "color" )
    color = Vec3( value.c_str());
  else
    Shader::setPropertyFromString(key, value);
}
