/* 
 * Tux Racer 
 * Copyright (C) 1999-2000 Jasmin F. Patry
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "tuxracer.h"
#include "gl_util.h"

/*
 * Constants 
 */

static const colour_t clear_colour = { 0.545, 0.70, 0.90 };
static const colour_t text_colour = { 0.0, 0.0, 0.0 };
static const float fog_colour[4] = { 1.0, 1.0, 1.0, 1.0 };


/* XXX: this will eventually replaced with nicer texture-mapped chars */
/* This routine taken from Mesa Demos */
void print_string( void *font, char *string )
{
    int len, i;

    len = (int) strlen(string);
    for (i=0; i<len; i++) 
        glutBitmapCharacter( font, string[i] );
} 

void reshape( int w, int h )
{
    set_x_resolution( w );
    set_y_resolution( h );
    glViewport( 0, 0, (GLint) w, (GLint) h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( get_fov(), (scalar_t)w/h, 0.1, 10000 );

    glMatrixMode( GL_MODELVIEW );
} 

void flat_mode()
{
    set_gl_options( TEXT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -0.5, 639.5, -0.5, 479.5, -1.0, 1.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

void draw_overlay() {
    glColor4f( 0.0, 0.0, 1.0, 0.1 );
    glRecti( 0, 0, 640, 480 );
} 

void print_time()
{
    char buff[30];

    flat_mode();
    sprintf( buff, "%.1f", g_game.time );

    glColor3f( text_colour.r, text_colour.g, text_colour.b );
    glRasterPos2i( 5, 5 ); 
    print_string( GLUT_BITMAP_HELVETICA_10, buff );
}

void clear_rendering_context()
{
    glClearColor( clear_colour.r, clear_colour.g, clear_colour.b, 1.0 );
    glClearStencil( 0 );
    glClear( GL_COLOR_BUFFER_BIT 
	     | GL_DEPTH_BUFFER_BIT 
	     | GL_STENCIL_BUFFER_BIT );
}

void configure_fog()
{
    glFogi( GL_FOG_MODE, GL_EXP );
    glFogfv( GL_FOG_COLOR, fog_colour );
    glFogf( GL_FOG_DENSITY, 0.005 );
}
