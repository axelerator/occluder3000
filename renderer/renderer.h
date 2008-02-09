//
// C++ Interface: renderer
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RENDERER_H
#define RENDERER_H

namespace Occluder {

class Scene;

/**
Base class for different rendering algorithms

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
    enum RenderType {SINGLE_RAY};



class Renderer{
public:
    Renderer();
    virtual ~Renderer();


    virtual void render( const Scene&, unsigned char *mem ) const = 0;
    
    static Renderer* createRenderer(RenderType type);

};
}
#endif
