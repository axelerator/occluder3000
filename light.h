//
// C++ Interface: light
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef LIGHT_H
#define LIGHT_H
#include "vector3d.h"
#include "rgbvalue.h"
#include "sse4.h"
/**
	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Light{
public:
    Light();
    Light(const Vector3D& pos, const RGBvalue& color);
    Light(const Light& l);
    const Vector3D& getPosition() const { return position;}
    const SSEVec3D& getPosition4() const { return pos4;}
    const RGBvalue& getColor() const {return color; }
    const SSEVec3D& getColor4() const {return col4; }
    ~Light();
    void setPosition(float x, float y, float z);
    const Vector3D& getDirection() const {return direction;}
private:
    Vector3D position;
    SSEVec3D pos4;
    RGBvalue color;
    SSEVec3D col4;
    Vector3D direction;
};

#endif
