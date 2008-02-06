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
#include "accelerationstruct.h"
#include "camera.h"
#include "radianceray.h"
#include "scene.h"
#include "raypacket.h"

Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}

void Renderer::renderr (const Scene &scene, GLubyte *mem ) {
  const AccelerationStruct *tl = &scene.getGeometry();
  const Camera& cam = scene.getCamera();
  const Vector3D& u ( cam.u );
  const Vector3D& projOrigin ( cam.projOrigin );
  const Vector3D& position ( cam.position );

  const Vector3D &projPlaneU(cam.u / cam.resolution[0]);
  const Vector3D &projPlaneV(cam.v / cam.resolution[1]);


  Vector3D projectPoint ( projOrigin );
  Vector3D currentU;
  Vector3D currentV;

  // Calculate the color for every single pixel
  RadianceRay currentRay ( scene );
  currentRay.setStart ( cam.position );
  const float *rgb;
  unsigned int offset = 0;
  #ifndef NDEBUG
   unsigned long triri = 0;
  #endif
  for ( unsigned int y = 0 ; y < cam.resolution[1] ; ++y ) {
    for ( unsigned int x = 0; x < cam.resolution[0]; ++x ) {
      currentRay.setDirection ( ( projectPoint - position ).normal() );
      currentRay.setMin ( 0.0f );
      currentRay.setMax ( UNENDLICH );
      currentRay.getClosestIntersection().reset();
      projectPoint += projPlaneU;
      rgb = tl->trace ( currentRay ).getRGB();
      mem[offset++] = ( GLubyte ) ( rgb[0]*255 );
      mem[offset++] = ( GLubyte ) ( rgb[1]*255 );
      mem[offset++] = ( GLubyte ) ( rgb[2]*255 );
      #ifndef NDEBUG
        triri += currentRay.hittestcount;
      #endif
      }
    projectPoint += projPlaneV;
    projectPoint -= u;
  }
  #ifndef NDEBUG
    std::cout << "rrr " << ((double)triri/(cam.resolution[1]*cam.resolution[0])) << std::endl;
  #endif
}

void Renderer::renderrPackets (const Scene &scene, GLubyte *mem ) {
  const AccelerationStruct *tl = &scene.getGeometry();
  const Camera& cam = scene.getCamera();
  Vector3D u ( cam.u );
  Vector3D v ( cam.v );
  Vector3D projOrigin ( cam.projOrigin );
  Vector3D position ( cam.position );

  const Vector3D projPlaneU ( u / cam.resolution[0] );
  const Vector3D projPlaneV ( v / cam.resolution[1] );

  unsigned int packetsX = cam.resolution[0] / RayPacket::getPacketWidth();
  unsigned int packetsY = cam.resolution[1] / RayPacket::getPacketWidth();

  const Vector3D packetU ( u * ( 1.0f / packetsX ) );
  const Vector3D packetV ( v * ( 1.0f / packetsY ) );

  Vector3D projectPoint ( projOrigin );
  Vector3D currentU;
  Vector3D currentV;

  // Calculate the color for every single pixel
  RayPacket packet ( scene, cam.position );
  const float *rgb;
  unsigned int offset = 0;
  const int stride = 3 * cam.resolution[0];
  unsigned int packetIdx;
  memset ( mem, 0, sizeof ( GLubyte ) * cam.resolution[0] * cam.resolution[1] );
  // for tracing incoherent packets with single rays
  RadianceRay singleRay ( scene );
  singleRay.setStart ( position );

  Vector3D packetOrigin;
  for ( unsigned int y = 0 ; y < packetsY ; ++y ) {
    offset = y * packet.getPacketWidth() * stride;
    for ( unsigned int x = 0; x < packetsX; ++x ) {
      // sign test
      packetOrigin = projOrigin + y * packetV + x * packetU;

      if ( ! packet.set ( packetOrigin, projPlaneU , projPlaneV, packetU, packetV )) {
        packetIdx = 0; // For incoherent packets, trace single rays

        const Vector3D r4u ( 2.0f * projPlaneU );
        const Vector3D r4v ( 2.0f * projPlaneV );
        Vector3D d0 ( packetOrigin - position );

        Ray4 r4;
        unsigned int tileidx = 0;
        for ( unsigned int pv = 0; pv < packet.getPacketWidth() / 2; ++pv ) {
          for ( unsigned int pu = 0; pu < packet.getPacketWidth() / 2 ; ++pu ) {
            unsigned ray = 0;
            r4.set ( position, d0, projPlaneU, projPlaneV, &scene.getGeometry() );
            for ( unsigned ry = 0; ry < 2; ++ry ) {
              for ( unsigned rx = 0; rx < 2; ++rx ) {
//                   ray = 2*rx + ry;

//                 d0.normalize();
                singleRay.setDirection ( Vector3D( r4.direction.c[0].v.f[ray],
                                                        r4.direction.c[1].v.f[ray],
                                                        r4.direction.c[2].v.f[ray] ));

                  singleRay.setMin ( 0.0f );
                  singleRay.setMax ( UNENDLICH );
                  singleRay.getClosestIntersection().reset();
                  offset = ( y * packet.getPacketWidth()  + 2 * pv + ry ) * stride  + ( x * packet.getPacketWidth() + 2 * pu + rx ) * 3;
                  rgb = tl->trace ( singleRay ).getRGB();
                  mem[offset++] = ( GLubyte ) ( rgb[0]*255 );
                  mem[offset++] = ( GLubyte ) ( rgb[1]*255 );
                  mem[offset++] = ( GLubyte ) ( rgb[2]*255 );
                  d0 += projPlaneU;
                
                ++ray;
              }
              d0 -= r4u;
              d0 += projPlaneV;
            }
            d0 -= r4v;
            d0 += r4u;
            ++tileidx;
          }
          d0 += r4v;
          d0 -= packetU;
        }
      } else {
        if ( tl->trace ( packet ) ) {
          packet.shade ( mem + ( y * packet.getPacketWidth() * stride  +  x * packet.getPacketWidth() * 3 ), stride );
//               packetIdx = 0;
//               for( unsigned int pv = 0; pv < packet.getPacketWidth() ; ++pv ){
//                 offset = ( y * packet.getPacketWidth()  + pv) * stride  +  x * packet.getPacketWidth() * 3;
//                 for( unsigned int pu = 0; pu < packet.getPacketWidth() ; ++pu ){
//                   rgb = packet.getColor(packetIdx++).getRGB();
//                   mem[offset++] = ( GLubyte ) ( rgb[0]*255 );
//                   mem[offset++] = ( GLubyte ) ( rgb[1]*255 );
//                   mem[offset++] = ( GLubyte ) ( rgb[2]*255 );
//                 }
//               }
        } else {
          for ( unsigned int pv = 0; pv < packet.getPacketWidth() ; ++pv ) {
            offset = ( y * packet.getPacketWidth()  + pv ) * stride  +  x * packet.getPacketWidth() * 3;
            for ( unsigned int pu = 0; pu < packet.getPacketWidth() ; ++pu ) {
              mem[offset++] = ( GLubyte ) ( 255 );
              mem[offset++] = ( GLubyte ) ( 0 );
              mem[offset++] = ( GLubyte ) ( 0 );
            }
          }
        }
      }
    }
  }
}


void Renderer::directByPhoton (const Scene &scene, GLubyte *mem ) {
  const AccelerationStruct *tl = &scene.getGeometry();
  const Camera& cam = scene.getCamera();
  Vector3D u ( cam.u );
  Vector3D v ( cam.v );
  Vector3D projOrigin ( cam.projOrigin );
  Vector3D position ( cam.position );

  Vector3D projPlaneU;
  Vector3D projPlaneV;

  projPlaneU = u / cam.resolution[0];
  projPlaneV = v / cam.resolution[1];

  Vector3D projectPoint ( projOrigin );
  Vector3D currentU;
  Vector3D currentV;

  // Calculate the color for every single pixel
  RadianceRay currentRay ( scene );
  currentRay.setMin ( 0.0f );
  currentRay.setMax ( UNENDLICH );
  currentRay.setStart (  cam.position );
  float rgb[3];
  unsigned int offset = 0;
  for ( unsigned int y = 0 ; y < cam.resolution[1] ; ++y ) {
    for ( unsigned int x = 0; x < cam.resolution[0]; ++x ) {
      currentRay.setDirection ( ( projectPoint - position ).normal() );
      currentRay.setMin ( 0.0f );
      currentRay.setMax ( UNENDLICH );
      currentRay.getClosestIntersection().reset();
      projectPoint += projPlaneU;
      tl->getClosestIntersection(currentRay);
      
      
      rgb[0] = 0.0; 
      rgb[1] = 0.0; 
      rgb[2] = 0.0;
      const Intersection &ir = currentRay.getClosestIntersection();
      if ( currentRay.didHitSomething() ){
        const PhongMaterial mat = ir.triangle->getMaterial();
//         scene.causticsMap.irradiance_estimate(rgb, ir.intersectionPoint.value, ir.triangle->getNormalAt(ir).value, 0.05, 150);
        mem[offset++] = ( GLubyte ) ( (fminf(1.0,rgb[0]) * mat.diffuse[0]) * 255 );
        mem[offset++] = ( GLubyte ) ( (fminf(1.0,rgb[1]) * mat.diffuse[1]) * 255 );
        mem[offset++] = ( GLubyte ) ( (fminf(1.0,rgb[2]) * mat.diffuse[2]) * 255 );
      } else {
        mem[offset++] = ( GLubyte ) 25;// ( rgb[0]*255 );
        mem[offset++] = ( GLubyte ) ( rgb[1]*255 );
        mem[offset++] = ( GLubyte ) ( rgb[2]*255 );
      }
    }
    projectPoint += projPlaneV;
    projectPoint -= u;
  }

}
