//
// C++ Implementation: light
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "light.h"
#include <iostream>

using namespace Occluder;

Light::Light() :
  direction(Vec3(0.0f, -1.0f, 0.0f)),
  color(Vec3(1.0f)),
  position(Vec3(1.0f))
{}


Light::~Light() {
}

void Light::setPropertyFromString(const std::string& key, const std::string& value) {
  if ( key == "position") {
    position = Vec3(value);
  } else if ( key == "diffuse") {
    color = Vec3(value);
  } else if ( key == "direction") 
    direction = Vec3(value);
   else 
    std::cerr << "unknown property for light: " << key << std::endl;
}

