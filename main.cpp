#include <stdlib.h>
#include <string>
#include <vector>
#include <boost/regex.hpp>

#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <time.h>
#include <GL/glew.h>
#include <SDL/SDL.h>

#include "vector3d.h"
#include "radianceray.h"
#include "triangle.h"
#include "accelerationstruct.h"
#include "trianglelist.h"
#include "debug.h"
#include "regulargrid.h"
#include "camera.h"
#include "objectloader.h"
//#define DEBUG_ENABLE
#include "bih2.h"
#include "kdtree.h"
#include "raypacket.h"


float *output;
unsigned char *result = 0;
char done = 0;
Scene scene;
float zoom = 2.0;
float eyeYpos = 1.0;
int turn = 0;
unsigned char mode = 1;
int perm = 0;
void userEvents() {
  // Poll for events, and handle the ones we care about.
  SDL_Event event;
  while ( SDL_PollEvent ( &event ) ) {
    switch ( event.type ) {
      case SDL_KEYDOWN:
        switch ( event.key.keysym.sym ) {
          case SDLK_LEFT:
            turn += 1;
            break;
          case SDLK_RIGHT:
            turn -= 1;
            break;
          default:;
        }
        break;
      case SDL_KEYUP:
        switch ( event.key.keysym.sym ) {
          case SDLK_PAGEUP:
            eyeYpos += 0.03;
            break;
          case SDLK_PAGEDOWN:
            eyeYpos -= 0.03;
            break;
          case SDLK_UP:
            zoom *= 0.8;
            break;
          case SDLK_DOWN:
            zoom *= 1.2;
            break;
          case SDLK_LEFT:
            turn -= 1;
            break;
          case SDLK_RIGHT:
            turn += 1;
            break;
          case SDLK_F1:
            mode = 1;
            break;
          case SDLK_F2:
            mode = 2;
            break;
          case SDLK_F3:
            mode = 3;
            break;
          case SDLK_F4:
            perm = ( perm + 1 ) % 24;
            break;
          case SDLK_F5:
            perm = ( perm - 1 ) % 24;
            break;
            // If escape is pressed, return (and thus, quit)
          case SDLK_ESCAPE:
            done = 1;
            break;
          default:;
        }
      case SDL_QUIT:
        ;//         done = 1;
    }
  }
}
void renderr ( AccelerationStruct *tl, const Camera& cam, GLubyte *mem ) {
  unsigned int resolution[2] = { cam.resolution[0],  cam.resolution[1]};
  if ( result == 0 )
    result = new unsigned char[resolution[0]*resolution[1]*3];

  Vector3D u ( cam.u );
  Vector3D v ( cam.v );
  Vector3D projOrigin ( cam.projOrigin );
  Vector3D position ( cam.position );

  Vector3D projPlaneU;
  Vector3D projPlaneV;

  projPlaneU = u / resolution[0];
  projPlaneV = v / resolution[1];

  Vector3D projectPoint ( projOrigin );
  Vector3D currentU;
  Vector3D currentV;

  // Calculate the color for every single pixel
  RadianceRay currentRay ( scene );
  currentRay.setMin ( 0.0f );
  currentRay.setMax ( UNENDLICH );
  const float *rgb;
  unsigned int offset = 0;
  for ( unsigned int y = 0 ; y < resolution[1] ; ++y ) {
    for ( unsigned int x = 0; x < resolution[0]; ++x ) {

      currentRay.setDirection ( ( projectPoint - position ).normal() );
      currentRay.setStart ( projectPoint );
      currentRay.setMin ( 0.0f );
      currentRay.setMax ( UNENDLICH );
      currentRay.getClosestIntersection().reset();
      projectPoint += projPlaneU;

//             if ( x < 27 || x > 32 || y < 37 | y > 50) {
//               offset += 3;
//               continue;
//               }
//       if ( x == 147 && y == 116 )
//         rgb = 0;
      rgb = tl->trace ( currentRay ).getRGB();
      mem[offset++] = ( GLubyte ) ( rgb[0]*255 );
      mem[offset++] = ( GLubyte ) ( rgb[1]*255 );
      mem[offset++] = ( GLubyte ) ( rgb[2]*255 );
    }
    projectPoint += projPlaneV;
    projectPoint -= u;
  }

}

int perms[][4] = {
                   {0,1,2,3},
                   {0,1,3,2},
                   {0,2,1,3},
                   {0,2,3,1},
                   {0,3,2,1},
                   {0,3,1,2},

                   {1,0,2,3},
                   {1,0,3,2},
                   {1,2,0,3},
                   {1,2,3,0},
                   {1,3,2,0},
                   {1,3,0,2},

                   {2,1,2,3},
                   {2,1,3,2},
                   {2,2,1,3},
                   {2,2,3,1},
                   {2,3,2,1},
                   {2,3,1,2},

                   {3,1,2,0},
                   {3,1,0,2},
                   {3,2,1,0},
                   {3,2,0,1},
                   {3,0,2,1},
                   {3,0,1,2},
                 };

void renderrPackets ( AccelerationStruct *tl, const Camera& cam, GLubyte *mem ) {
  unsigned int resolution[2] = { cam.resolution[0],  cam.resolution[1]};
  if ( result == 0 )
    result = new unsigned char[resolution[0]*resolution[1]*3];

  Vector3D u ( cam.u );
  Vector3D v ( cam.v );
  Vector3D projOrigin ( cam.projOrigin );
  Vector3D position ( cam.position );

  const Vector3D projPlaneU ( u / resolution[0] );
  const Vector3D projPlaneV ( v / resolution[1] );

  unsigned int packetsX = resolution[0] / RayPacket::getPacketWidth();
  unsigned int packetsY = resolution[1] / RayPacket::getPacketWidth();

  const Vector3D packetU ( u * ( 1.0f / packetsX ) );
  const Vector3D packetV ( v * ( 1.0f / packetsY ) );

  Vector3D projectPoint ( projOrigin );
  Vector3D currentU;
  Vector3D currentV;

  // Calculate the color for every single pixel
  RayPacket packet ( scene, cam.position );
  const float *rgb;
  unsigned int offset = 0;
  const int stride = 3 * resolution[0];
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
//         if ( x == 21 && y == 15) {
//             packet.set ( packetOrigin, projPlaneU , projPlaneV, packetU, packetV )   ;
//             if ( tl->trace ( packet ) )
//               packet.shade ( mem + ( y * packet.getPacketWidth() * stride  +  x * packet.getPacketWidth() * 3 ), stride );
// /*            for ( unsigned int pv = 0; pv < packet.getPacketWidth() ; ++pv ) {
//               offset = ( y * packet.getPacketWidth()  + pv ) * stride  +  x * packet.getPacketWidth() * 3;
//               for ( unsigned int pu = 0; pu < packet.getPacketWidth() ; ++pu ) {
//                 mem[offset++] = ( GLubyte ) ( 0 );
//                 mem[offset++] = ( GLubyte ) ( 0 );
//                 mem[offset++] = ( GLubyte ) ( 255 );
//               }
//             }*/
//         } else
      if ( ! packet.set ( packetOrigin, projPlaneU , projPlaneV, packetU, packetV ) || ( mode != 3 ) ) {
        tl->trace ( packet );
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

                d0.normalize();
                if ( mode == 1 ) {
                  offset = ( y * packet.getPacketWidth()  + 2 * pv + ry ) * stride  + ( x * packet.getPacketWidth() + 2 * pu + rx ) * 3;
                  if ( packet.r4[tileidx].mask & ( 1 << ray )) {
                    mem[offset++] = ( GLubyte ) ( 55 );
                    mem[offset++] = ( GLubyte ) ( 155);
                    mem[offset++] = ( GLubyte ) ( 74 );
                  }
                } else {

                  if ( mode == 3 )
                    singleRay.setDirection ( Vector3D ( r4.direction.c[0].v.f[ray],
                                                        r4.direction.c[1].v.f[ray],
                                                        r4.direction.c[2].v.f[ray] )
                                           );
                  else if ( mode == 2 )

                    singleRay.setDirection ( Vector3D ( packet.r4[tileidx].direction.c[0].v.f[perms[perm][ray]],
                                                        packet.r4[tileidx].direction.c[1].v.f[perms[perm][ray]],
                                                        packet.r4[tileidx].direction.c[2].v.f[perms[perm][ray]] )
                                           );

                  singleRay.setMin ( 0.0f );
                  singleRay.setMax ( UNENDLICH );
                  singleRay.getClosestIntersection().reset();
                  offset = ( y * packet.getPacketWidth()  + 2 * pv + ry ) * stride  + ( x * packet.getPacketWidth() + 2 * pu + rx ) * 3;
                  rgb = tl->trace ( singleRay ).getRGB();
                  mem[offset++] = ( GLubyte ) ( rgb[0]*255 );
                  mem[offset++] = ( GLubyte ) ( rgb[1]*255 );
                  mem[offset++] = ( GLubyte ) ( rgb[2]*255 );
                  d0 += projPlaneU;
                }
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

void printUsage() {
  std::cout << "Usage: pmrrt OBJ-FILE [OPTIONS]\n\n";
  std::cout << "\tOptions:\n\t\t -p, --profile\t\t render only one frame for profiling" << std::endl;
  std::cout << "\t\t\t -r, --resolution <width>x<height>\t\t render with the given screen resolution" << std::endl;
  std::cout << "\t\t\t -as={grid,bih}\t\t Use regular grid or bih-tree to acellerate rendering" << std::endl;

}

double gettime() {
  struct timeval t;
  gettimeofday ( &t,0 );
  return t.tv_sec+t.tv_usec/1000000.0;
}

// Entry point
int main ( int argc, char *argv[] ) {
  bool profile = false;
  /*  int width = 64;
    int height = 48;*/
  int width = 320;
  int height = 240;
  int accellStruc = 0;
  void ( *renderFunc ) ( AccelerationStruct *tl, const Camera& cam, GLubyte *mem ) = renderrPackets;

  if ( argc < 2 ) {
    printUsage();
    exit ( 1 );
  }
  if ( argc > 2 ) {
    int currArg = 1;
    while ( ++currArg < argc ) {
      std::string arg ( argv[currArg] );
      if ( arg == "-as=grid" )
        accellStruc = 1;
      else if ( arg == "-as=bih" )
        accellStruc = 2;
      else if ( arg == "-as=kd" )
        accellStruc = 3;
      else if ( arg == "-p" || arg == "--profile" )
        profile = true;
      else if ( arg == "-s" || arg == "--single" )
        renderFunc = renderr;
      else if ( ( arg == "-r" ) || ( arg == "resolution" ) ) {
        if ( currArg+1 >= argc ) {
          printUsage();
          exit ( 0 );
        } else {
          boost::regex resreg;
          resreg.assign ( "([0-9]+)x([0-9]+)" );
          boost::cmatch matches;
          if ( boost::regex_match ( argv[++currArg], matches, resreg ) ) {
            std::string widthstr ( matches[1].first, matches[1].second );
            std::string heightstr ( matches[2].first, matches[2].second );
            width = atoi ( widthstr.c_str() );
            height = atoi ( heightstr.c_str() );
          } else {
            std::cout << "Give resolution in form <height>x<widht> , i.e. 800x600\n --help for more information" << std::endl;
          }
        }
      }

    }
  }


  // Initialize SDL's subsystems - in this case, only video.
  if ( SDL_Init ( SDL_INIT_VIDEO ) < 0 ) {
    fprintf ( stderr, "Unable to init SDL: %s\n", SDL_GetError() );
    exit ( 1 );
  }

  // Register SDL_Quit to be called at exit; makes sure things are
  // cleaned up when we quit.
  atexit ( SDL_Quit );

  const SDL_VideoInfo* info = SDL_GetVideoInfo();
  // Attempt to create a XRESxYRES window with 32bit pixels.
  SDL_Surface *screen = SDL_SetVideoMode ( width, height, info->vfmt->BitsPerPixel, SDL_OPENGL );

  // If we fail, return error.
  if ( screen == NULL ) {
    fprintf ( stderr, "Unable to set XRESxYRES video: %s\n", SDL_GetError() );
    exit ( 1 );
  }

  GLenum err = glewInit();
  if ( GLEW_OK != err ) {
    /* Problem: glewInit failed, something is seriously wrong. */
    fprintf ( stderr, "Error: %s\n", glewGetErrorString ( err ) );
  }
  glClearColor ( 0.0, 0.0, 0.0, 0.0 );
  glShadeModel ( GL_FLAT );
  glDisable ( GL_DEPTH_TEST );
  glEnable ( GL_TEXTURE_2D );

  GLuint buf_size = width * height * 3;
  GLuint id;

  glGenBuffers ( 1, &id );
  glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0 );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

  glBindBuffer ( GL_PIXEL_UNPACK_BUFFER_ARB, id );
  glBufferData ( GL_PIXEL_UNPACK_BUFFER_ARB, buf_size, 0, GL_STREAM_DRAW );

  RGBvalue cred ( 0.6, 0.2, 0.2 );
  RGBvalue cgreen ( 0.2, 0.6, 0.2 );
  RGBvalue cblue ( 0.2, 0.2, 0.6 );
  RGBvalue cwhite ( 1.0, 1.0, 1.0 );

  Light red ( Vector3D ( -1.5, 1.5,  0.0 ), cred );
  Light blue ( Vector3D ( 1.5, 1.5, 0.0 ), cblue );
  Light green ( Vector3D ( 0.0, 1.5, -1.5 ), cgreen );
  Light white ( Vector3D ( 0.0, 0.9, 0.0 ), cwhite );
  scene.addLight ( red );
  scene.addLight ( blue );
  scene.addLight ( green );
//    scene.addLight ( white );


  AccelerationStruct *structure = 0;
  switch ( accellStruc ) {
    case 0: structure = new Trianglelist ( scene );break;
    case 1: structure = new RegularGrid ( scene, 0.2 );break;
    case 2: structure = new BIH ( scene );break;
    case 3: structure = new KdTree ( scene );break;
  }
  scene.setGeometry ( structure );

  scene.addMaterial ( "red", PhongMaterial ( 1.0, 0.0, 0.0, 1.0, 0.0, 0.0 ) );
  scene.addMaterial ( "blue", PhongMaterial ( 0.0, 0.0, 1.0, 1.0, 0.0, 0.0 ) );
  scene.addMaterial ( "green", PhongMaterial ( 0.0, 1.0, 0.0, 1.0, 0.0, 0.0 ) );
  scene.addMaterial ( "mirror", PhongMaterial ( 1.0, 1.0, 1.0, 1.0, 0.0, 0.8 ) );
  scene.addMaterial ( "glass", PhongMaterial ( 0.2, 0.5, 1.0, 0.4, 0.3, 0.0 ) );
  std::string filename ( argv[1] );
  if ( filename.find ( ".obj" ) != std::string::npos )
    ObjectLoader::loadOBJ ( filename, scene );
  else if ( filename.find ( ".ra2" ) != std::string::npos )
    ObjectLoader::loadRA2 ( filename, scene );
  else
    std::cerr << "Unknown file extension";
  std::cout << "Triangles in scene: " << structure->getTriangleCount() << std::endl;

  Vector3D position ( -0.5001, 0.25001, 4.1 );
  Vector3D target ( 0.0, 0.5, 0.0 );
  Vector3D lookUp ( 0.0, 1.0, 0.0 );

  Camera cam ( position, target, lookUp, 0.9, width, height );

  std::cout << "start rendering..." << std::endl;
  float angle = 0.51;

  unsigned int frame = 0;
  glBindBuffer ( GL_PIXEL_UNPACK_BUFFER_ARB, id );
  GLubyte *mem = 0;

  double start, total;
  do {
    start = gettime();
    structure->construct();
//  cam.setPosition ( Vector3D ( sin(angle), 1.0, 1.5));

    cam.setPosition ( Vector3D ( sin ( angle ) * ( zoom ), eyeYpos, cos ( angle ) * ( zoom ) ) );
//     cam.setPosition ( Vector3D ( sin ( angle ) * ( 0.3 ), 1.0 + sin ( angle ) *0.2, cos ( angle ) * ( 2.1 ) ) );
//     l.setPosition( sin ( angle*0.33 ) * ( 2.1 ), 4.0 , cos ( angle*0.33 ) * ( 2.1 ) );
    mem = ( GLubyte * ) glMapBuffer ( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY );
    memset ( mem,0, width*height*3 );
    renderFunc ( structure, cam, mem );

    glUnmapBuffer ( GL_PIXEL_UNPACK_BUFFER_ARB );
    glTexSubImage2D ( GL_TEXTURE_2D, 0, 0,0, width, height, GL_RGB, GL_UNSIGNED_BYTE, 0 );

    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glBegin ( GL_QUADS );
    glTexCoord2f ( 0.0, 0.0 ); glVertex3f ( -1.0, 1.0, 0.0 );
    glTexCoord2f ( 0.0, 1.0 ); glVertex3f ( -1.0, -1.0, 0.0 );
    glTexCoord2f ( 1.0, 1.0 ); glVertex3f ( 1.0, -1.0, 0.0 );
    glTexCoord2f ( 1.0, 0.0 ); glVertex3f ( 1.0, 1.0, 0.0 );
    glEnd();
    SDL_GL_SwapBuffers();
    userEvents();
    angle += turn * 0.03;
    ++frame;
    total = gettime() - start;
    std::cout << "total : " << total << std::endl;
  } while ( !profile && !done );
  return 0;
}
