#include <iostream>
#include <SDL/SDL.h>

#include "scene.h"
#include "gldisplay.h"
#include "singlerayrenderer.h"
#include "sserenderer.h"
using namespace Occluder;

char done = 0;

void userEvents(Scene &scene, Camera& cam) {
  // Poll for events, and handle the ones we care about.
  SDL_Event event;
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


int main ( int argc, char *argv[] ) {
  if ( argc < 2 ) {
    std::cerr << "No scenefile specified." << std::endl;
    exit(1);
  }
    
  Scene scene;
  if ( !scene.loadFromFile(argv[1])) {
    std::cerr << "Failed to load " << argv[1] << std::endl;
    exit(2);
  }
  Camera& cam = scene.getCamera();

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
  SDL_Surface *screen = SDL_SetVideoMode (  cam.getResolution()[0],  cam.getResolution()[1], info->vfmt->BitsPerPixel, SDL_OPENGL );

  // If we fail, return error.
  if ( screen == NULL ) {
    fprintf ( stderr, "Unable to set XRESxYRES video: %s\n", SDL_GetError() );
    exit ( 1 );
  }


  SSERenderer renderer1;
  SingleRayRenderer renderer2;
  GLDisplay display(renderer2, cam.getResolution()[0], cam.getResolution()[1]);
  std::cout << "construction: " << scene.construct() << "ms\n";
  std::cout << "rendering: " << display.display(scene) << "ms\n";
  
  std::cout << "paff!" << std::endl;
  while ( !done ) {
    userEvents( scene, cam);
    cam.ani();
    display.display(scene);
  }

  return 0;
}
