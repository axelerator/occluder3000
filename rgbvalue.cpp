//
// C++ Implementation: rgbvalue
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "rgbvalue.h"

RGBvalue::RGBvalue(float r, float g, float b){
rgb[0] = r;
rgb[1] = g;
rgb[2] = b;
}
RGBvalue::RGBvalue(const float* rgb){
this->rgb[0] = rgb[0];
this->rgb[1] = rgb[1];
this->rgb[2] = rgb[2];
}

RGBvalue::RGBvalue(const RGBvalue& copy){
this->rgb[0] = copy.rgb[0];
this->rgb[1] = copy.rgb[1];
this->rgb[2] = copy.rgb[2];
}

void RGBvalue::add(float r, float g, float b) {
  rgb[0] += r;
  rgb[1] += g;
  rgb[2] += b;
}

RGBvalue::~RGBvalue()
{}


