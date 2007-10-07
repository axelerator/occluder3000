#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <time.h>



#include "SDL/SDL.h"
#include "vector3d.h"
#include "ray.h"
#include "triangle.h"
#include "intersectionresult.h"
#include <boost/regex.hpp>
#include "accelerationstruct.h"
#include "trianglelist.h"
#include "debug.h"
#include "regulargrid.h"
#include "camera.h"
#include "objectloader.h"
//#define DEBUG_ENABLE


class Tile {
public:
  
  Tile(unsigned int x, unsigned int y, SDL_Surface *screen) {
    surface = SDL_CreateRGBSurface(
      screen->flags,
      screen->w,
      screen->h,
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
    SDL_FreeSurface( surface);
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
unsigned int Tile::width = 40;
unsigned int Tile::height = 40;

SDL_Surface *screen;
float *output;
unsigned char *result = 0;
char done = 0;


void userEvents() {
     // Poll for events, and handle the ones we care about.
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_KEYDOWN:
                break;
            case SDL_KEYUP:
                // If escape is pressed, return (and thus, quit)
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    done = 1;
                break;
            case SDL_QUIT:
                done = 1;
            }
        }
}

void DrawPixel(SDL_Surface *screen, int x, int y,Uint8 R, Uint8 G,Uint8 B) {
    Uint32 color = SDL_MapRGB(screen->format, R, G, B);

    if ( SDL_MUSTLOCK(screen) ) {
        if ( SDL_LockSurface(screen) < 0 ) {
            return;
        }
    }

    switch (screen->format->BytesPerPixel) {
    case 1: { /* vermutlich 8 Bit */
        Uint8 *bufp;

        bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
        *bufp = color;
    }
    break;

    case 2: { /* vermutlich 15 Bit oder 16 Bit */
        Uint16 *bufp;

        bufp = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
        *bufp = color;
    }
    break;

    case 3: { /* langsamer 24-Bit-Modus, selten verwendet */
        Uint8 *bufp;

        bufp = (Uint8 *)screen->pixels + y*screen->pitch + x * 3;
        if (SDL_BYTEORDER == SDL_LIL_ENDIAN) {
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

        bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
        *bufp = color;
    }
    break;
    }

    if ( SDL_MUSTLOCK(screen) ) {
        SDL_UnlockSurface(screen);
    }
}



void updateDisplay(Tile& t , int totalWidth, int totalHeight,
                   SDL_Surface *screen, unsigned int bytesPerPixel) {

    // Lock surface if needed
    if (SDL_MUSTLOCK(screen))
        if (SDL_LockSurface(screen) < 0)
            return;

    // Ask SDL for the time in milliseconds

    // Declare a couple of variables
//     int i, j, yofs, ofs;
    // Draw to screen
//     yofs = 0;
//     for (i = left; i < t.getWidth(); i++) {
//         for (j = top, ofs = yofs; j < t.getHeight(); j++, ofs++) {
//             DrawPixel(screen,j, i, 255, 255, 255);
//         }
//         yofs += screen->pitch / 4;
//     }


    SDL_BlitSurface(t.getSurface(), &Tile::getCompleteTileRect(), screen, &t.getRect());

    
    // Unlock if needed
    if (SDL_MUSTLOCK(screen))
        SDL_UnlockSurface(screen);

//    SDL_UpdateRect(screen, t.getRect().x, t.getRect().y, t.getWidth(), t.getHeight());
}


void renderr(AccelerationStruct *tl, const Camera& cam, Tile &t) {
    unsigned int resolution[2] = { cam.resolution[0],  cam.resolution[1]};
    if (result == 0)
        result = new unsigned char[resolution[0]*resolution[1]*3];

    Vector3D u(cam.u);
    Vector3D v(cam.v);
    Vector3D projOrigin(cam.projOrigin);
    Vector3D position(cam.position);

    Vector3D projPlaneU;
    Vector3D projPlaneV;

    projPlaneU = u / resolution[0];
    projPlaneV = v / resolution[1];

    Vector3D projectPoint;
    Vector3D currentU;
    Vector3D currentV;

    // Calculate the color for every single pixel
    Ray currentRay;
    IntersectionResult ir(true);
//     {{
//     int x = 113;
//     int y = 144;
   
//     Uint8 *p = (Uint8 *)(t.getSurface()->pixels);
    SDL_Rect r = t.getRect();
    for (unsigned int y = 0 ; y < r.h ; ++y ) {
        for (unsigned int x = 0; x < r.w; ++x ) {
            // Calculate the current ray based on the current pixel and the camera
            //std::cout << "(" << x << "|" << y << ") ";
            currentU = projPlaneU * (x + r.x);
            currentV = projPlaneV * (y + r.y);
            projectPoint = projOrigin + currentU + currentV;
//             currentRay.setDirection((position - projectPoint));
            currentRay.setDirection((projectPoint - position).normal());
            currentRay.setStart(projectPoint);
            const float *rgb = tl->trace(currentRay).getRGB();
//             result[y*stride + 3*x] = (unsigned char)(rgb[0]*255);
//             result[y*stride + 3*x+1] = (unsigned char)(rgb[1]*255);
//             result[y*stride + 3*x+2] = (unsigned char)(rgb[2]*255);
//              p[(y - r.y) * t.getSurface()->pitch + (x - r.x) * t.getSurface()->format->BytesPerPixel] = (unsigned char)(rgb[0]*255) | (unsigned char)(rgb[1]*255) << 8  | (unsigned char)(rgb[2]*255) << 16;
//              DrawPixel(t.getSurface(), x, y, x*25,y*25,255);
//              DrawPixel(t.getSurface(),x,  y, (Uint8)rgb[0]*255, (Uint8)rgb[1]*255, (Uint8)rgb[2]*255);
             DrawPixel(t.getSurface(),x,  y, (Uint8)(rgb[0]*255), (Uint8)(rgb[1]*255), (Uint8)(rgb[2]*255));
//              std::cout << (rgb[0]) << ","  << (rgb[1]) << "," << (rgb[2]) << std::endl;
      }
    }

}

void printUsage() {
      std::cout << "Usage: pmrrt OBJ-FILE [OPTIONS]\n\n";
      std::cout << "\tOptions:\n\t\t -p, --profile\t\t render only one frame for profiling" << std::endl;
      std::cout << "\tOptions:\n\t\t -r, --resolution <width>x<height>\t\t render with the given screen resolution" << std::endl;
}

// Entry point
int main(int argc, char *argv[]) {
    bool profile = false;
    int width = 320;
    int height = 240;
    
    
    
    if (argc < 2) {
      printUsage();
      exit(1);
    }
    if (argc > 2) {
      int currArg = 1;
      while (++currArg < argc) {
        std::string arg(argv[currArg]);
        if (arg == "-p" || arg == "--profile" )
          profile = true;
        else if (( arg == "-r" ) || (arg == "resolution") ) {
          if ( currArg+1 >= argc) {
            printUsage();
            exit(0);
          } else {
            boost::regex resreg;
            resreg.assign("([0-9]+)x([0-9]+)");
            boost::cmatch matches;
            if (boost::regex_match(argv[++currArg], matches, resreg)) {
               std::string widthstr(matches[1].first, matches[1].second);
               std::string heightstr(matches[2].first, matches[2].second);
               width = atoi(widthstr.c_str());
               height = atoi(heightstr.c_str());
            } else {
              std::cout << "Give resolution in form <height>x<widht> , i.e. 800x600\n --help for more information" << std::endl;
            }
          }
        }
        
      }
    }
    if (argc > 2)
      profile = ! ( strcmp("-p", argv[2]) && strcmp("--profile", argv[2]));

    // Initialize SDL's subsystems - in this case, only video.
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }

    // Register SDL_Quit to be called at exit; makes sure things are
    // cleaned up when we quit.
    atexit(SDL_Quit);

    // Attempt to create a XRESxYRES window with 32bit pixels.
    screen = SDL_SetVideoMode(width, height, 24, SDL_SWSURFACE);

    // If we fail, return error.
    if ( screen == NULL ) {
        fprintf(stderr, "Unable to set XRESxYRES video: %s\n", SDL_GetError());
        exit(1);
    }
    std::cout << "x";
    Scene scene;
    RGBvalue cred(0.6, 0.2, 0.2);
    RGBvalue cgreen(0.2, 0.6, 0.2);
    RGBvalue cblue(0.2, 0.2, 0.6);
    
    Light red(Vector3D(-2.0, 3.0,  1.0), cred);
    Light blue(Vector3D(0.0, -1.0, 3.0), cblue);
    Light green(Vector3D(2.0, 3.0, 1.0), cgreen);
    scene.addLight(red);
    scene.addLight(blue);
    scene.addLight(green);

//     AccelerationStruct *structure =  new Trianglelist(scene);
    AccelerationStruct *structure =  new RegularGrid(scene, 0.2);
    ObjectLoader::loadMonkey(argv[1], structure);


    std::cout << "Triangles in scene: " << structure->getTriangleCount() << std::endl;
    std::cout << "constructing acceleration structure" << std::endl;
    
    structure->construct();
    
    std::cout << " (done in )";
    Vector3D position(-0.5001, 0.25001, 4.1);
    Vector3D target (0.0);
    Vector3D lookUp (0.0, 1.0, 0.0);
    
    Camera cam(position, target, lookUp, 1.5, width, height);
    

    std::cout << "done rendering" << std::endl;
    // Main loop: loop forever.
    fliess angle = 0;
    while (!profile && !done) {
        // Render stuff
    cam.setPosition(Vector3D(sin(angle)*(4.1+sin(angle)), 0.25, cos(angle)*(4.1+sin(angle))));
    unsigned int txmax = width / Tile::getWidth();
    unsigned int tymax = height / Tile::getHeight();
    for (unsigned int tx = 0; tx < txmax; ++tx)
      for (unsigned int ty = 0; ty < tymax; ++ty) {
        Tile tile(tx * Tile::getWidth(), ty * Tile::getHeight(), screen);
        renderr(structure, cam, tile);
        SDL_BlitSurface(tile.getSurface(), &Tile::getCompleteTileRect(), screen, &(tile.getRect()));
        SDL_UpdateRect(screen, tile.getRect().x,tile.getRect().y,tile.getRect().w,tile.getRect().h);
    }
    userEvents();
    angle += 0.1;
    }
    return 0;
}
