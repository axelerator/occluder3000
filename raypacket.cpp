//
// C++ Implementation: raypacket
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "raypacket.h"

RayPacket::RayPacket ( const Scene& scene, const Vector3D& eye ) : origin ( eye ), scene ( scene ) {}

RayPacket::~RayPacket() {}

bool RayPacket::set ( const Vector3D& origin, const Vector3D& u, const Vector3D& v, const Vector3D& packetu , const Vector3D& packetv ) {
  const Vector3D r4u ( 2.0f * u );
  const Vector3D r4v ( 2.0f * v );
  Vector3D d0 ( origin - this->origin );

  shaft.set ( this->origin, d0, packetu, packetv, &scene.getGeometry() );

//   int m1 = _mm_movemask_ps( rp->dx4 );
//   int m2 = _mm_movemask_ps( rp->dy4 );
//   int m3 = _mm_movemask_ps( rp->dz4 );
//   if (((m1 == 0) || (m1 == 15)) && ((m2 == 0) || (m2 == 15)) && ((m3 == 0) || (m3 == 15))) valid = true;
//

// TODO: this is still wrong:
  bool incoherent = (
                      ( ( shaft.direction.c[0].v.f[0] < 0 ) == ( shaft.direction.c[0].v.f[3] < 0 ) )
                      && ( ( shaft.direction.c[1].v.f[0] < 0 ) == ( shaft.direction.c[1].v.f[3] < 0 ) )
                      && ( ( shaft.direction.c[2].v.f[0] < 0 ) == ( shaft.direction.c[2].v.f[3] < 0 ) )
                    );

  unsigned int tileIdx = 0;
  for ( unsigned int y = 0; y < PACKET_WIDTH/2; ++y ) {
    for ( unsigned int x = 0 ; x < PACKET_WIDTH/2; ++x ) {
      r4[tileIdx].set ( this->origin, d0, u, v, &scene.getGeometry() );
      d0 += r4u;
      ++tileIdx;
    }
    d0 += r4v;
    d0 -= packetu ;
  }
  return incoherent;
}

Vector3D RayPacket::refractRay ( const Vector3D& e, const Vector3D& n, float nFrom, float nTo ) {
  float ne = e * n;
  float reflection = 1.0 - powf ( ( nFrom/nTo ), 2.0 ) * ( 1.0 - pow ( ne, 2.0 ) );

  if ( reflection < 0.0 ) {
    Vector3D vpar ( ne * n );
    Vector3D reflDir ( e - ( 2 * vpar ) );
    reflDir.normalize();
    return reflDir;
  }

  return Vector3D ( ( ( e - ( n * ne ) ) * ( nFrom/nTo ) - ( n * sqrt ( reflection ) ) ).normal() );
}


static float zero[] = {0.0, 0.0, 0.0};
void RayPacket::shade ( GLubyte *fbuffer, unsigned int stride, unsigned int depth ) {
  unsigned int mempos = 0;
  const unsigned int tilesPerRow = getPacketWidth() / 2,
                                   str0 = stride - 6,
                                          str1 = ( 2*stride ) - 6,
                                                 str2 = ( 2*stride ) - getPacketWidth() * 3;
  unsigned tileIdx  = 0;
  const std::vector<Light> lights = scene.getLights();
  std::vector<Light>::const_iterator it = lights.begin();
  SSEVec3D normals;
  SSEVec3D matdif;
  Intfloat triidx;
  SSEVec3D poi ( origin );


  unsigned int rayIdx = 0;

  for ( unsigned int tiley = 0; tiley < tilesPerRow; ++tiley ) {
    for ( unsigned int tilex = 0; tilex < tilesPerRow; ++tilex ) {

      rayIdx = 0;
      SSEVec3D direct, reflect;
      memset ( &direct, 0, sizeof ( SSEVec3D ) );
      memset ( &direct, 0, sizeof ( SSEVec3D ) );
      const Ray4& currTile = r4[tileIdx];
//         if ( currTile.mask){
      poi = origin;
      poi += currTile.direction * r4[tileIdx].t;
      for ( unsigned int i = 0; i < 4; ++i ) {
        if ( r4[tileIdx].mask & ( 1 << i ) ) {
          triidx.f = currTile.hitTriangle.v.f[i];
          const Triangle& tri = scene.getGeometry().getTriangle ( triidx.i );
          normals.setVec ( i, tri.getNormal() );
          matdif.setVec ( i, tri.getMaterial().diffuse );
        } else {
          normals.setVec ( i, zero );
        }
      }

      for ( it = lights.begin(); it!=lights.end(); ++it ) {
        const Light& light = *it;

        SSEVec3D l4 ( light.getPosition() );
        l4 -= poi;
        SSEVec3D l4unnorm = l4;
//             assert(_mm_movemask_ps(_mm_cmpngt_ps(normals.length(), ONE)));
        SSE4 tmax;
        tmax.v.sse = l4.normalizeRL();
        SSE4 dif =  _mm_max_ps ( _mm_setzero_ps (), normals * l4 );
        SSEVec3D ldir4 ( light.getDirection() );

        SSE4 cspot = _mm_max_ps ( ( ( l4 * -1.0f ) * ldir4 ),_mm_setzero_ps () );
        dif = dif * cspot;

        for ( unsigned int c = 0; c < 4 ; ++c ) {
          if ( dif.v.f[c] > 0.0f ) {
            triidx.f = currTile.hitTriangle.v.f[c];
            const Triangle& tri = scene.getGeometry().getTriangle ( triidx.i );
//             Ray intersectToLigth ( poi.get ( c ), l4.get ( c ), tmax.v.f[c], 0.00f, &tri );
            const Vector3D rtg = ( light.getPosition() - poi.get ( c ) );
            Ray intersectToLigth ( poi.get ( c ),rtg.normal(), rtg.length(), 0.00f, &tri );
            dif.v.f[c] = ( scene.getGeometry().isBlocked ( intersectToLigth ) ) ? 0.0 : dif.v.f[c];
          }
        }
        direct += ( light.getColor4() ^ matdif ) * dif;
      }

      for ( unsigned int c = 0; c < 4 ; ++c ) {
        if ( r4[tileIdx].mask & ( 1 << c ) ) {
          triidx.f = currTile.hitTriangle.v.f[c];
          const Triangle& tri = scene.getGeometry().getTriangle ( triidx.i );
          const PhongMaterial& mat = tri.getMaterial();
          if ( depth > 0 && mat.reflection > 0.0f ) {
            const Vector3D normal ( normals.get ( c ) );
            const Vector3D direction ( currTile.direction.get ( c ) );
            Vector3D vpar ( ( normal * direction ) * normal );
            Vector3D reflDir ( direction - ( 2 * vpar ) );
            reflDir.normalize();
            RadianceRay reflectedRay ( poi.get ( c ), reflDir, scene );
            reflectedRay.setIgnore ( &tri );
            RGBvalue reflected;
            reflected = scene.getGeometry().trace ( reflectedRay, depth - 1 );
            float rest = 1.0 -  mat.reflection;
            for ( unsigned int z = 0; z < 3; ++z ) {
              direct.c[z].v.f[c] *= rest;
              direct.c[z].v.f[c] +=  reflected.getRGB() [z] * mat.reflection;
            }
          }
        }
      }

      for ( unsigned int c = 0; c < 4 ; ++c ) {
        if ( r4[tileIdx].mask & ( 1 << c ) ) {
          triidx.f = currTile.hitTriangle.v.f[c];
          const Triangle& tri = scene.getGeometry().getTriangle ( triidx.i );
          const PhongMaterial& mat = tri.getMaterial();
          if ( depth > 0 && mat.alpha < 1.0f ) {
            const Vector3D n ( normals.get ( c ) );
            const Vector3D direction ( currTile.direction.get ( c ) );
            Vector3D refrDirIn ( refractRay ( direction, n, 1.0f, mat.refract ) );
            RadianceRay reflectedRay ( poi.get ( c ), refrDirIn, scene );
            reflectedRay.setIgnore ( &tri );

            const Intersection &inters = scene.getGeometry().getClosestIntersection ( reflectedRay );

            if ( inters.triangle != 0 ) {
              Vector3D refrDirOut = refractRay ( refrDirIn, inters.triangle->getNormalAt ( inters ) * -1.0, mat.refract, 1.0 );

              RadianceRay r2 = RadianceRay (
                                 inters.intersectionPoint,
                                 refrDirOut, scene
                               );
              r2.setIgnore ( inters.triangle );

              RGBvalue refracted = scene.getGeometry().trace ( r2, depth - 1 );
              float rest = 1.0 - mat.alpha;
              for ( unsigned int z = 0; z < 3; ++z ) {
                direct.c[z].v.f[c] *= mat.alpha;
                direct.c[z].v.f[c] +=  refracted.getRGB() [z] * rest;
              }
            }
          }
        }
      }
      SSE4 byte255;
      byte255.v.sse = _mm_set1_ps ( 255.0f );
      direct *= byte255;

      rayIdx = 0;
      for ( unsigned int y = 0; y < 2 ;  ++y ) {
        for ( unsigned int x = 0; x < 2 ;  ++x ) {
          if ( currTile.mask & ( 1 << rayIdx ) ) {
            fbuffer[mempos++] = ( unsigned char ) direct.c[0].v.f[rayIdx];
            fbuffer[mempos++] = ( unsigned char ) direct.c[1].v.f[rayIdx];
            fbuffer[mempos++] = ( unsigned char ) direct.c[2].v.f[rayIdx];
//             fbuffer[mempos++] = ( unsigned char ) 255;
//             fbuffer[mempos++] = ( unsigned char ) direct.c[1].v.f[rayIdx];
//             fbuffer[mempos++] = ( unsigned char ) direct.c[2].v.f[rayIdx];
          } else
            mempos += 3;
          ++rayIdx;
        }
        mempos += str0;
      }
      mempos -= str1;
      ++tileIdx;
    }
    mempos += str2;
  }
}

