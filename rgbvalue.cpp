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
#include "stringutil.h"

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

RGBvalue::RGBvalue(const std::string& s){
  const std::string trimmed(StringUtil::trim(s));
  const size_t firstSpace = trimmed.find_first_of(" ");
  const size_t secondSpace = trimmed.find_last_of(" ");
  const std::string v1(trimmed.substr(0, firstSpace));
  const std::string v2(trimmed.substr(firstSpace + 1, secondSpace - firstSpace - 1));
  const std::string v3(trimmed.substr(secondSpace + 1));
  rgb[0] = atof(v1.c_str());
  rgb[1] = atof(v2.c_str());
  rgb[2] = atof(v3.c_str());
}

void RGBvalue::add(float r, float g, float b) {
  rgb[0] += r;
  rgb[1] += g;
  rgb[2] += b;
}

RGBvalue::~RGBvalue()
{}


