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

Scene::Scene()
{}

void Scene::addLight(const Light& l) {
  lights.push_back(l);
}

Scene::~Scene()
{
}


