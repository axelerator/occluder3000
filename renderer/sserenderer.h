//
// C++ Interface: sserenderer
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERSSERENDERER_H
#define OCCLUDERSSERENDERER_H

#include <renderer.h>

namespace Occluder {

/**
Renders the image in packets of 2x2 rays incorporating SSE instructions to parallelize rendering of four rays.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class SSERenderer : public Renderer
{
public:
    SSERenderer();

    ~SSERenderer();
    virtual void render( const Scene& scene, unsigned char *mem ) const;

};

}

#endif
