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
#include "light.h"

#include "assert.h"

class AccelerationStruct;
/**
Container for geomtrydatastruct and other objects (lights, cams, etc)

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Scene{
public:
    Scene();
    void addLight(const Light& l);
    const std::vector<Light> &getLights() const { return lights; }
    Light &getLight(int i) { return lights[i]; }
    void setGeometry(AccelerationStruct *geometry) {assert(geometry != 0 ); this->geometry = geometry;}
    AccelerationStruct &getGeometry() const { return *geometry; }
    ~Scene();
private:
  std::vector<Light> lights;
  AccelerationStruct *geometry;
};

#endif
