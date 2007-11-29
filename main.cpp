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
#include "ray.h"
#include "triangle.h"
#include "intersectionresult.h"
#include <boost/regex.hpp>
#include "accelerationstruct.h"
#include "trianglelist.h"
#include "bihlist.h"
#include "debug.h"
#include "regulargrid.h"
#include "camera.h"
#include "objectloader.h"
//#define DEBUG_ENABLE
#include "bih2.h"

class Tile {
  public:

    Tile ( unsigned int x, unsigned int y, SDL_Surface *screen ) {
      surface = SDL_CreateRGBSurface (
                  screen->flags,
                  width,
                  height,
                  screen->format->BitsPerPixel,
                  screen->format->Rmask,
                  screen->format->Gmask,
                  screen->format->Bmask,
                  screen->format->Amask
                );
      pos.x = x;
      pos.y = y;
      pos.w = width;
      pos.h = height;
    }

    ~Tile() {
      SDL_FreeSurface ( surface );
    }

    static unsigned int getWidth() { return Tile::width;}
    static unsigned int getHeight() { return Tile::height;}
    SDL_Surface* getSurface() { return surface; }
    SDL_Rect &getRect() { return pos;}
    unsigned char getBytesPerPixel() const {return bytesPerPixel;}
    static SDL_Rect& getCompleteTileRect() { return Tile::completeRect;}

  private:

    SDL_Surface *surface;
    SDL_Rect pos;
    static unsigned char bytesPerPixel;
    static unsigned int width;
    static unsigned int height;
    static SDL_Rect completeRect;
};

unsigned char Tile::bytesPerPixel = 3;
SDL_Rect Tile::completeRect = {0,0,Tile::width,Tile::height};
unsigned int Tile::width = 320;
unsigned int Tile::height = 240;

SDL_Surface *screen;
float *output;
unsigned char *result = 0;
char done = 0;


void userEvents() {
  // Poll for events, and handle the ones we care about.
  SDL_Event event;
  while ( SDL_PollEvent ( &event ) ) {
    switch ( event.type ) {
      case SDL_KEYDOWN:
        break;
      case SDL_KEYUP:
        // If escape is pressed, return (and thus, quit)
        if ( event.key.keysym.sym == SDLK_ESCAPE )
          done = 1;
        break;
      case SDL_QUIT:
        done = 1;
    }
  }
}

void DrawPixel ( SDL_Surface *screen, int x, int y,Uint8 R, Uint8 G,Uint8 B ) {
  Uint32 color = SDL_MapRGB ( screen->format, R, G, B );

  switch ( screen->format->BytesPerPixel ) {
    case 1: { /* vermutlich 8 Bit */
      Uint8 *bufp;

      bufp = ( Uint8 * ) screen->pixels + y*screen->pitch + x;
      *bufp = color;
    }
    break;

    case 2: { /* vermutlich 15 Bit oder 16 Bit */
      Uint16 *bufp;

      bufp = ( Uint16 * ) screen->pixels + y*screen->pitch/2 + x;
      *bufp = color;
    }
    break;

    case 3: { /* langsamer 24-Bit-Modus, selten verwendet */
      Uint8 *bufp;

      bufp = ( Uint8 * ) screen->pixels + y*screen->pitch + x * 3;
      if ( SDL_BYTEORDER == SDL_LIL_ENDIAN ) {
        bufp[0] = color;
        bufp[1] = color >> 8;
        bufp[2] = color >> 16;
      } else {
        bufp[2] = color;
        bufp[1] = color >> 8;
        bufp[0] = color >> 16;
      }
    }
    break;

    case 4: { /* vermutlich 32 Bit */
      Uint32 *bufp;

      bufp = ( Uint32 * ) screen->pixels + y*screen->pitch/4 + x;
      *bufp = color;
    }
    break;
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

  Vector3D projectPoint;
  Vector3D currentU;
  Vector3D currentV;

  // Calculate the color for every single pixel
  Ray currentRay;

  unsigned int stride = resolution[0] * 3;
  unsigned int offset = 0;
  for ( unsigned int y = 0 ; y < resolution[1] ; ++y ) {
    for ( unsigned int x = 0; x < resolution[0]; ++x ) {
      // Calculate the current ray based on the current pixel and the camera
      currentU = projPlaneU * ( x );
      currentV = projPlaneV * ( y );
      projectPoint = projOrigin + currentU + currentV;

      currentRay.setDirection ( ( projectPoint - position ).normal() );
      currentRay.setStart ( projectPoint );
      const float *rgb = tl->trace ( currentRay ).getRGB();
      mem[offset + 3*x] = ( Uint8 ) ( rgb[0]*255 );
      mem[offset + 3*x+1] = ( Uint8 ) ( rgb[1]*255 );
      mem[offset + 3*x+2] = ( Uint8 ) ( rgb[2]*255 );
    }
    offset += stride;
  }

}

void printUsage() {
  std::cout << "Usage: pmrrt OBJ-FILE [OPTIONS]\n\n";
  std::cout << "\tOptions:\n\t\t -p, --profile\t\t render only one frame for profiling" << std::endl;
  std::cout << "\t\t\t -r, --resolution <width>x<height>\t\t render with the given screen resolution" << std::endl;
  std::cout << "\t\t\t -as={grid,bih}\t\t Use regular grid or bih-tree to acellerate rendering" << std::endl;

}

// Entry point
int main ( int argc, char *argv[] ) {
  bool profile = false;
  int width = 320;
  int height = 240;
  int accellStruc = 0;


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
      else if ( arg == "-p" || arg == "--profile" )
        profile = true;
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
  screen = SDL_SetVideoMode(width, height, info->vfmt->BitsPerPixel, SDL_OPENGL);

  // If we fail, return error.
  if ( screen == NULL ) {
    fprintf ( stderr, "Unable to set XRESxYRES video: %s\n", SDL_GetError() );
    exit ( 1 );
  }
  
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    /* Problem: glewInit failed, something is seriously wrong. */
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err) );
  }  
  glClearColor (0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glEnable(GL_TEXTURE_2D);

  GLuint buf_size = width * height * 3;
  GLuint id;
  
  glGenBuffers(1, &id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, id);
  glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, buf_size, 0, GL_STREAM_DRAW);
  
  
  Scene scene;
  RGBvalue cred ( 0.6, 0.2, 0.2 );
  RGBvalue cgreen ( 0.2, 0.6, 0.2 );
  RGBvalue cblue ( 0.2, 0.2, 0.6 );
  RGBvalue cwhite ( 1.0, 1.0, 1.0 );

  Light red ( Vector3D ( -2.0, 3.0,  1.0 ), cred );
  Light blue ( Vector3D ( 0.0, -1.0, 3.0 ), cblue );
  Light green ( Vector3D ( 2.0, 3.0, 1.0 ), cgreen );
  scene.addLight ( red );
  scene.addLight ( blue );
  scene.addLight ( green );

  AccelerationStruct *structure = 0;
  switch ( accellStruc ) {
    case 0: structure = new Trianglelist ( scene );break;
    case 1: structure = new RegularGrid ( scene, 0.2 );break;
    case 2: structure = new BIH2 ( scene );break;
  }

  std::string filename ( argv[1] );
  if ( filename.find ( ".obj" ) != std::string::npos )
    ObjectLoader::loadOBJ ( filename, structure );
  else if ( filename.find ( ".ra2" ) != std::string::npos )
    ObjectLoader::loadRA2 ( filename, structure );
  else
    std::cerr << "Unknown file extension";

  std::cout << "Triangles in scene: " << structure->getTriangleCount() << std::endl;
  std::cout << "constructing acceleration structure" << std::endl;

  structure->construct();

  std::cout << " done \n";
  Vector3D position ( -0.5001, 0.25001, 4.1 );
  Vector3D target ( 0.0 );
  Vector3D lookUp ( 0.0, 1.0, 0.0 );

  Camera cam ( position, target, lookUp, 1.0, width, height );

  std::cout << "start rendering..." << std::endl;
  fliess angle = 0;

  unsigned int frame = 0;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, id);
  GLubyte *mem = 0;
  while ( !profile && !done ) {
    cam.setPosition ( Vector3D ( sin ( angle ) * ( 3.1 ), 0.25 + sin ( angle ) *0.5, cos ( angle ) * ( 3.1 ) ) );

    mem = (GLubyte *)glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY);
    renderr ( structure, cam, mem );
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, width, height, GL_RGB, GL_UNSIGNED_BYTE, 0); 
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_QUADS);
      glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, 1.0, 0.0);
      glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, -1.0, 0.0);
      glTexCoord2f(1.0, 1.0); glVertex3f( 1.0, -1.0, 0.0);
      glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, 1.0, 0.0);
    glEnd();
    SDL_GL_SwapBuffers();
    userEvents();
    angle += 0.1;
    ++frame;
  }
  return 0;
}
