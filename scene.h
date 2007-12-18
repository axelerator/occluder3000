//
// C++ Interface: scene
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <map>
#include <string>
#include "light.h"
#include "phongmaterial.h"
#include "assert.h"

class AccelerationStruct;
/**
Container for geomtrydatastruct and other objects (lights, cams, etc)

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Scene{
public:
    Scene();
    void addMaterial(const std::string& name, const PhongMaterial& mat) {
      material.insert(std::pair<std::string, PhongMaterial>(name, mat));
    }
    const PhongMaterial& getMaterial(std::string name) {
      return material[name];
    }
    void addLight(const Light& l);
    const std::vector<Light> &getLights() const { return lights; }
    Light &getLight(int i) { return lights[i]; }
    void setGeometry(AccelerationStruct *geometry) {assert(geometry != 0 ); this->geometry = geometry;}
    AccelerationStruct &getGeometry() const { return *geometry; }
    ~Scene();
private:
  std::vector<Light> lights;
  AccelerationStruct *geometry;
  std::map<std::string, PhongMaterial> material;
};

#endif
