//
// C++ Interface: singlerayrenderer
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SINGLERAYRENDERER_H
#define SINGLERAYRENDERER_H

#include <renderer.h>
namespace Occluder {
/**
Every pixel is traced with a single ray.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class SingleRayRenderer : public Renderer
{
public:
    SingleRayRenderer();

    virtual ~SingleRayRenderer();

    virtual void render(const Scene& scene, unsigned char *mem ) const;

};
}
#endif
