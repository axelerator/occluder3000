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
#include <GL/gl.h>
class AccelerationStruct;
class Camera;
class Scene;
/**
	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Renderer{
public:
    Renderer();
    ~Renderer();

static void renderrPackets ( const Scene &scene, GLubyte *mem );
static void renderr ( const Scene &scene, GLubyte *mem );
static void directByPhoton ( const Scene &scene, GLubyte *mem );
};

#endif
