//
// C++ Implementation: shader
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "shader.h"
#include <iostream>

using namespace Occluder;

Shader::Shader(const std::string& name, const Scene& scene) : name(name), scene(scene){
}


Shader::~Shader() {
}

void Shader::setPropertyFromString(const std::string& key, const std::string& value ) {
  std::cerr << "Shader property " << key << " is not valid\n";
}
