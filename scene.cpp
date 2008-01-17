//
// C++ Implementation: scene
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "scene.h"

Scene::Scene() : defaultMaterial( 1.0, 1.0, 1.0, 1.0, 0.0, 0.0 ){
}

void Scene::addLight(const Light& l) {
  lights.push_back(l);
}

const PhongMaterial& Scene::getMaterial(const std::string& name)  {
  if (material.find(name) == material.end())
    return defaultMaterial;
  else
    return material[name];
}

Scene::~Scene()
{
}


