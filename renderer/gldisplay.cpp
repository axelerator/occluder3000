//
// C++ Implementation: gldisplay
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "gldisplay.h"
#include <SDL/SDL.h>

#include "renderer.h"

using namespace Occluder;

GLDisplay::GLDisplay(const Renderer& renderer, unsigned int width, unsigned int height):
renderer(renderer), width(width), height(height) {
  init();
}


GLDisplay::~GLDisplay() {

}

void Occluder::GLDisplay::init() {
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
    glBindBuffer ( GL_PIXEL_UNPACK_BUFFER_ARB, id );

}


void Occluder::GLDisplay::display(const Scene& scene) {
    mem = ( GLubyte * ) glMapBuffer ( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY );
    memset ( mem,0, width*height*3 );
    renderer.render(scene, (unsigned char *)mem);



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
}
