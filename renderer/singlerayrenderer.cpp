//
// C++ Implementation: singlerayrenderer
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "singlerayrenderer.h"
#include "scene.h"
#include "intersection.h"

using namespace Occluder;
SingleRayRenderer::SingleRayRenderer()
: Renderer() {
}


SingleRayRenderer::~SingleRayRenderer() {
}


void SingleRayRenderer::render(const Scene& scene,unsigned char *mem ) const {
  const Camera& cam = scene.getCamera();

  const Vec3 projPlaneU(cam.getU() / cam.getResolution()[0]);
  const Vec3 projPlaneV(cam.getV() / cam.getResolution()[1]);

  Vec3 projectPoint ( cam.getProjcetionplaneOrigin() );
  // Calculate the color for every single pixel
  unsigned int offset = 0;
  const float resolution[] = { cam.getResolution()[0], cam.getResolution()[1]};
  for ( unsigned int y = 0 ; y < resolution[1] ; ++y ) {
    for ( unsigned int x = 0; x < resolution[0]; ++x ) {
      const RaySegment currentRay(cam.getPosition(), ( projectPoint - cam.getPosition() ).normal());
      const Intersection intersection = scene.trace(currentRay);
      if ( !intersection.isEmpty()) {
        const Vec3 radiance = intersection.getRadiance(currentRay.getDirection(), 5);
        mem[offset++] = (unsigned char)(radiance[0] * 255.0f );
        mem[offset++] = (unsigned char)(radiance[1] * 255.0f );
        mem[offset++] = (unsigned char)(radiance[2] * 255.0f );
      } else
        offset += 3;
      projectPoint += projPlaneU;
    }
    projectPoint += projPlaneV;
    projectPoint -= cam.getU();
  }
}

