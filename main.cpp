
#include <stdlib.h>
#include <string>
#include <vector>

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
#include "regulargrid.h"
#include "camera.h"
#include "objectloader.h"
//#define DEBUG_ENABLE
#include "bih2.h"
#include "kdtree.h"
#include "raypacket.h"
#include "stats.h"
#include "renderer.h"

float *output;
unsigned char *result = 0;
char done = 0;

float zoom = 3.2;
float eyeYpos =2.31;
int turn = 0;
unsigned char mode = 2;
int perm = 0;
void ( *renderFunc ) ( const Scene &scene, GLubyte *mem ) = Renderer::renderr/*Packets*/;

void userEvents(Scene &scene) {
  // Poll for events, and handle the ones we care about.
  SDL_Event event;
  Camera& cam(scene.getCamera());
  while ( SDL_PollEvent ( &event ) ) {
    switch ( event.type ) {
      case SDL_KEYDOWN:
        switch ( event.key.keysym.sym ) {
          case SDLK_PAGEUP:
            cam.setTurnSpeed( -0.03, 1);
            break;
          case SDLK_PAGEDOWN:
            cam.setTurnSpeed( 0.03, 1);
            break;
          case SDLK_LEFT:
            cam.setTurnSpeed( -0.03, 0);
            break;
          case SDLK_RIGHT:
            cam.setTurnSpeed( 0.03, 0);
            break;
          case SDLK_UP:
            cam.setMovementSpeed(0.06);
            break;
          case SDLK_DOWN:
            cam.setMovementSpeed(-0.06);
            break;
          default:;
        }
        break;
      case SDL_KEYUP:
        switch ( event.key.keysym.sym ) {
          case SDLK_PAGEUP:
            cam.setTurnSpeed( 0.0, 1);
            break;
          case SDLK_PAGEDOWN:
            cam.setTurnSpeed( 0.0, 1);
            break;
          case SDLK_UP:
            cam.setMovementSpeed(0.0);
            break;
          case SDLK_DOWN:
            cam.setMovementSpeed(0.0);
            break;
          case SDLK_LEFT:
            cam.setTurnSpeed( 0.0, 0);
            break;
          case SDLK_RIGHT:
            cam.setTurnSpeed( 0.0, 0);
            break;
          case SDLK_F1:
            renderFunc = Renderer::renderr;
            std::cout << "Switching to single ray tracer."  << std:: endl;
            break;
          case SDLK_F2:
            renderFunc = Renderer::renderrPackets;
            std::cout << "Switching to packet ray tracer."  << std:: endl;
            break;
          case SDLK_F3:
            renderFunc = Renderer::directByPhoton;
            std::cout << "Switching to photon map renderer."  << std:: endl;
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
      break;
      case SDL_QUIT:
        done = 1;
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


  if ( argc < 2 ) {
    printUsage();
    exit ( 1 );
  }
  if ( argc > 2 ) {
    int currArg = 1;
    while ( ++currArg < argc ) {
      std::string arg ( argv[currArg] );
      if ( arg == "-p" || arg == "--profile" )
        profile = true;
      else if ( arg == "-s" || arg == "--single" )
        renderFunc = Renderer::renderr;
    }
  }
  std::string filename ( argv[1] );

  Scene scene;
  if ( !scene.loadFromFile(filename)) {
    std::cerr << "Failed to load " << filename << std::endl;
    exit(1);
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
  RGBvalue cwhite ( 0.8, 0.8, 0.8 );

  Light red ( Vector3D ( -1.5, 1.5,  0.0 ), cred );
  Light blue ( Vector3D ( 1.5, 1.5, 0.0 ), cblue );
  Light green ( Vector3D ( 0.0, 1.5, -1.5 ), cgreen );
//   Light white ( Vector3D (-4.798394, 3.239082, 2.072176 ), cwhite );
  Light white ( Vector3D ( 0.0, 7.8, 0.0), cwhite );
//   scene.addLight ( red );
//   scene.addLight ( blue );
//   scene.addLight ( green );
//    scene.addLight ( white );


//   AccelerationStruct *structure = 0;
//   switch ( accellStruc ) {
//     case 0: structure = new Trianglelist ( scene );break;
//     case 1: structure = new RegularGrid ( scene, 0.2 );break;
//     case 2: structure = new BIH ( scene );break;
//     case 3: structure = new KdTree ( scene );break;
//   }
//   scene.setGeometry ( structure );

//   scene.addMaterial ( "red", PhongMaterial ( 1.0, 0.0, 0.0, 1.0, 0.0, 0.0 ) );
//   scene.addMaterial ( "blue", PhongMaterial ( 0.0, 0.0, 1.0, 1.0, 0.0, 0.0 ) );
//   scene.addMaterial ( "green", PhongMaterial ( 0.0, 1.0, 0.0, 1.0, 0.0, 0.0 ) );
//   scene.addMaterial ( "mirror", PhongMaterial ( 1.0, 1.0, 1.0, 1.0, 0.0, 1.0 ) );
//   scene.addMaterial ( "glass", PhongMaterial ( 0.2, 0.5, 1.0, 0.01, 1.03, 0.0 ) );

//   if ( filename.find ( ".obj" ) != std::string::npos )
//     ObjectLoader::loadOBJ ( filename, scene );
//   else if ( filename.find ( ".ra2" ) != std::string::npos )
//     ObjectLoader::loadRA2 ( filename, scene );
//   else
//     std::cerr << "Unknown file extension";
//   std::cout << "Triangles in scene: " << structure->getTriangleCount() << std::endl;

/*  Vector3D position ( -4.798394, -3.239082, 4.072176);
  Vector3D target ( -3.924189, -2.775814, 3.926753 );
  Vector3D lookUp ( 0.0, 0.0, 1.0 );*/
  Vector3D position ( 0, 0.239082, 4.072176);
  Vector3D target ( 0.0, 1.5, 0.0 );
  Vector3D lookUp ( 0.0, 1.0, 0.0 );

// // //   Camera cam ( position, target, lookUp, 0.9, width, height );
  double start, total;

    start = gettime();
    scene.getGeometry().construct();
    statsset("Construction time:", gettime() - start);
    scene.getGeometry().analyze();
    start = gettime();
//   scene.updatePhotonMap();

  std::cout << "start rendering..." << std::endl;
  float angle = 0.0;

  unsigned int frame = 0;
  glBindBuffer ( GL_PIXEL_UNPACK_BUFFER_ARB, id );
  GLubyte *mem = 0;
  const std::string constrtime("Construction Time");
  const std::string rndrtime("Render Time");
  do {
    start = gettime();
//  cam.setPosition ( Vector3D ( sin(angle), 1.0, 1.5));
    



//     cam.setPosition ( Vector3D ( sin ( angle ) * ( zoom ), eyeYpos, cos ( angle ) * ( zoom ) ) );
//     cam.setPosition ( Vector3D ( sin ( angle ) * ( 0.3 ), 1.0 + sin ( angle ) *0.2, cos ( angle ) * ( 2.1 ) ) );
//     l.setPosition( sin ( angle*0.33 ) * ( 2.1 ), 4.0 , cos ( angle*0.33 ) * ( 2.1 ) );
    mem = ( GLubyte * ) glMapBuffer ( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY );
    memset ( mem,0, width*height*3 );
    renderFunc ( scene, mem );



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
    userEvents(scene);
    scene.getCamera().ani();
    angle += turn * 0.03;
    ++frame;
    total = gettime() - start;
    statsset(rndrtime,total );
//     std::cout << "total : " << total << std::endl;
    Stats::getInstance().nextFrame();
  } while ( !profile && !done );
  std::cout << Stats::getInstance();
//   std::cout << "\tIntersections per Ray:\t\t\t" << (Stats::getInstance().get("Intersections per Frame") / (cam.resolution[0]*cam.resolution[1])) << "\n";
  std::cout << "\tTrianglecount:\t\t\t" << scene.getGeometry().getTriangleCount() << "\n";
//   std::cout << "\tResolution:\t\t\t" << cam.resolution[0] << "x" <<cam.resolution[1]<< "\n";
//   std::cout << "\tRays per second:\t\t\t" << ((cam.resolution[0]*cam.resolution[1])/(Stats::getInstance().get("Render Time"))) << "\n";
  return 0;
}
