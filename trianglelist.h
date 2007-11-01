//
// C++ Interface: trianglelist
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TRIANGLELIST_H
#define TRIANGLELIST_H
#include "accelerationstruct.h"
/**
  @author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Trianglelist : public AccelerationStruct
{
  public:
    Trianglelist(const Scene& scene);
    virtual const RGBvalue trace(Ray& r, unsigned int depth = 0);
    virtual ~Trianglelist();
    virtual void construct();
};

#endif