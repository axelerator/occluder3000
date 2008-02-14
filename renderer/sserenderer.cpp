//
// C++ Implementation: sserenderer
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "sserenderer.h"
#include "raysegmentsse.h"
#include "camera.h"
#include "scene.h"
#include "intersectionsse.h"
using namespace Occluder;

SSERenderer::SSERenderer()
: Renderer() {
}


SSERenderer::~SSERenderer() {
}

void Occluder::SSERenderer::render( const Scene& scene, unsigned char *mem ) const {
  const Camera& cam = scene.getCamera();

  const Vec3 projPlaneU(cam.getU() / cam.getResolution()[0]);
  const Vec3 projPlaneV(cam.getV() / cam.getResolution()[1]);
  
  const unsigned int halfWidth = (cam.getResolution()[0] / 2);
  const unsigned int halfHeight = (cam.getResolution()[1] / 2);

  const Vec3 packetU(cam.getU() / halfWidth);
  const Vec3 packetV(cam.getV() / halfHeight) ;
  
  Vec3 projectPoint ( cam.getProjcetionplaneOrigin() );
  const unsigned int stride = cam.getResolution()[0] * 3;
  const unsigned int packetOffsets[4] = { 0, 3, stride, stride + 3 };
  unsigned int r = 0;
  unsigned char *packetStart;

  for ( unsigned int y = 0; y < halfHeight; ++y ) {
    for ( unsigned int x = 0; x < halfWidth; ++x ) {
      const Vec3SSE directions(projectPoint - cam.getPosition(), projPlaneU, projPlaneV);
      const RaySegmentSSE packet( cam.getPosition(), directions  );
      const IntersectionSSE intersections( scene.trace( packet ) );

      const Vec3SSE color = intersections.getRadiance( packet, 5) * Float4(255.0f);
      packetStart = mem + 2 * stride * y + 6 * x;
      for ( r = 0; r < 4; ++r) {
//       const RaySegment currentRay(cam.getPosition(), directions.get(r));
//       const Intersection intersection = scene.trace(currentRay);
//       Vec3 testc(0.0f);
//       if ( !intersection.isEmpty()) 
//         testc = intersection.getRadiance(currentRay.getDirection().normal(), 5);
//       packetStart[packetOffsets[r]]     = testc[0] * 255.0f;
//       packetStart[packetOffsets[r] + 1] = testc[1] * 255.0f;
//       packetStart[packetOffsets[r] + 2] = testc[2] * 255.0f;


          packetStart[packetOffsets[r]]     = color.c[0].v.f[r];
          packetStart[packetOffsets[r] + 1] = color.c[1].v.f[r];
          packetStart[packetOffsets[r] + 2] = color.c[2].v.f[r];
      }
      projectPoint += packetU;
    }
    projectPoint -= cam.getU();
    projectPoint  += packetV;
  }
}
