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

const colour_t white = { 1.0, 1.0, 1.0 };
const colour_t grey  = { 0.7, 0.7, 0.7 };
const colour_t red   = { 1.0, 0. , 0.  };
const colour_t green = { 0. , 1.0, 0.  };
const colour_t blue  = { 0. , 0. , 1.0 };
const colour_t light_blue = { 0.5, 0.5, 0.8 };
const colour_t black = { 0., 0., 0. };
const colour_t sky   = { 0.82, 0.86, 0.88 };

/* XXX: this will eventually replaced with nicer texture-mapped chars */
/* This routine taken from Mesa Demos */
void print_string( void *font, char *string )
{
    int len, i;

    len = (int) strlen(string);
    for (i=0; i<len; i++) 
        glutBitmapCharacter( font, string[i] );
} 

void print_string_centered( scalar_t y, void *font, char *string )
{
    scalar_t width;

    width = glutBitmapLength( font, (unsigned char*) string );
    width = width / getparam_x_resolution() * 640.;
    glRasterPos2i( 640. / 2. - width / 2., y );
    print_string( font, string );
}

void reshape( int w, int h )
{
    setparam_x_resolution( w );
    setparam_y_resolution( h );
    glViewport( 0, 0, (GLint) w, (GLint) h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( getparam_fov(), (scalar_t)w/h, 0.1, 10000 );

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


/* 
 * Sets the material properties
 */
void set_material_alpha( colour_t diffuse_colour, colour_t specular_colour,
			 scalar_t specular_exp, scalar_t alpha )
{
  GLfloat mat_amb_diff[4];
  GLfloat mat_specular[4];

  /* Set material colour (used when lighting is on) */
  mat_amb_diff[0] = diffuse_colour.r;
  mat_amb_diff[1] = diffuse_colour.g;
  mat_amb_diff[2] = diffuse_colour.b;
  mat_amb_diff[3] = alpha; 
  glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff );

  mat_specular[0] = specular_colour.r;
  mat_specular[1] = specular_colour.g;
  mat_specular[2] = specular_colour.b;
  mat_specular[3] = alpha;
  glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular );

  glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, specular_exp );

  /* Set standard colour */
  glColor4f( diffuse_colour.r, diffuse_colour.g, diffuse_colour.b, alpha );
} 

void set_material( colour_t diffuse_colour, colour_t specular_colour,
                  double specular_exp )
{
    set_material_alpha( diffuse_colour, specular_colour, specular_exp, 1.0 );
} 
