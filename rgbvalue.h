//
// C++ Interface: rgbvalue
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RGBVALUE_H
#define RGBVALUE_H

/**
  @author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class RGBvalue
{
  public:
    RGBvalue(float r, float g, float b);
    RGBvalue(const float rgb[3]);
    RGBvalue(const RGBvalue& copy);
    const float* getRGB() const { return rgb ;}
    void add(float r, float g, float b);
    ~RGBvalue();
  private:
  float rgb[3];
};

#endif
