//
// C++ Implementation: renderer
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "renderer.h"
#include "singlerayrenderer.h"

using namespace Occluder;

Renderer::Renderer(){}


Renderer::~Renderer(){}

Renderer* createRenderer( RenderType type) {
  switch ( type ) {
    case  SINGLE_RAY: return new SingleRayRenderer();
    default: return 0;
  }
  return 0;
}

