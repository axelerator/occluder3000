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
enum PhotonBehavior {ABSORB, DIF_REFLECT, TRANSMIT, SPEC_REFLECT};

class PhongMaterial{
public:
    PhongMaterial(float r = 1.0f, float g = 1.0f, float b = 1.0f, float alpha = 1.0f, float refract = 1.0f, float reflection = 0.0f );
    PhongMaterial(const PhongMaterial& mat) { memcpy(this, &mat, sizeof(PhongMaterial));}
    PhongMaterial& operator=(const PhongMaterial& mat) { memcpy(this, &mat, sizeof(PhongMaterial)); return *this;}
    ~PhongMaterial();
    PhotonBehavior spinRoulette() const;
    void setPropertyFromString(const std::string& key, const std::string& value);

    bool isSpecular() const { return ( alpha < 1.0f || reflection > 0.0f);}
    bool isDiffuse() const { return ( alpha > 0.0f && reflection < 1.0f);}
    float diffuse[3];
    float alpha;
    float refract;
    float reflection;
};

#endif
