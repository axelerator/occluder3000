//
// C++ Interface: phongmaterial
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PHONGMATERIAL_H
#define PHONGMATERIAL_H
#include <string.h>
#include "sse4.h"
/**
	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class PhongMaterial{
public:
    PhongMaterial(float r = 0.0f, float g = 0.0f, float b = 0.0f, float alpha = 0.0f, float refract = 0.0f, float reflection = 0.0f );
    PhongMaterial(const PhongMaterial& mat) { memcpy(this, &mat, sizeof(PhongMaterial));}
    PhongMaterial& operator=(const PhongMaterial& mat) { memcpy(this, &mat, sizeof(PhongMaterial)); return *this;}
    ~PhongMaterial();
    float diffuse[3];
//     SSEVec3D dif4;
    float alpha;
    float refract;
    float reflection;
};

#endif
