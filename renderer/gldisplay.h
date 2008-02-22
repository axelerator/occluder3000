//
// C++ Interface: gldisplay
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GLDISPLAY_H
#define GLDISPLAY_H

#include <GL/glew.h>

#include "scene.h"

namespace Occluder {
class Renderer;

/**
Displays the results of the rendering in a GL-window

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class GLDisplay{
public:
    GLDisplay(const Renderer& renderer, unsigned int width, unsigned int height);

    ~GLDisplay();
    void init();
    double display(const Scene& scene);

private:
    const Renderer& renderer;
    GLubyte *mem;
    const unsigned int width, height;

};
}
#endif
