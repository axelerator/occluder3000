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

FlatShader::FlatShader(const Vec3& color, const Scene& scene)
: Shader(scene), color(color) {
}


FlatShader::~FlatShader() {
}


Vec3 FlatShader::getRadiance(const Vec3& direction, const Intersection& intersection) const {
  return color;
}


