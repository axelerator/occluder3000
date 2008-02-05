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
#include <string>

/**
  @author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class RGBvalue
{
  public:
    RGBvalue(float r = 0.0f, float g = 0.0f, float b = 0.0f);
    RGBvalue(const float rgb[3]);
    RGBvalue(const RGBvalue& copy);
    RGBvalue(const std::string& s);
    
    RGBvalue& operator=(const RGBvalue& copy) {
      memcpy(this, &copy, sizeof(RGBvalue));
      return *this;
    }
    const float* getRGB() const { return rgb ;}
    void add(float r, float g, float b);
    void set(float r, float g, float b) {rgb[0] = r; rgb[1] = g; rgb[2] = b;}
    static RGBvalue mix(const RGBvalue& c1, const RGBvalue& c2, float ratio) {
      float r2 = 1.0 - ratio;
      return RGBvalue(c1.rgb[0] * ratio + c2.rgb[0] * r2, c1.rgb[1] * ratio + c2.rgb[1] * r2, c1.rgb[2] * ratio + c2.rgb[2] * r2);
    }
    
    ~RGBvalue();
  private:
  float rgb[3];
};

#endif
