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

/**
Container for geomtrydatastruct and other objects (lights, cams, etc)

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Scene{
public:
    Scene();
    void addLight(const Light& l);
    const std::vector<Light> &getLights() const { return lights; }
    ~Scene();
private:
std::vector<Light> lights;
};

#endif
