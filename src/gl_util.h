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

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _GL_UTIL_H_
#define _GL_UTIL_H_

#include "tuxracer.h"

/* Shouldn't need to include this if gl.h is recent, but alas we can't
 * count on that...
 */
#include <GL/glext.h>

#if !defined(GL_GLEXT_VERSION) || GL_GLEXT_VERSION < 6
#   error "*** You need a more recent copy of glext.h.  You can get one at http://oss.sgi.com/projects/ogl-sample/ABI/glxext.h ; it goes in /usr/include/GL. ***"
#endif

extern PFNGLLOCKARRAYSEXTPROC glLockArraysEXT_p;
extern PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT_p;

typedef enum {
    GUI,
    GAUGE_BARS,
    TEXFONT,
    TEXT,
    COURSE,
    TREES,
    PARTICLES,
    PARTICLE_SHADOWS,
    BACKGROUND,
    TUX,
    TUX_SHADOW,
    SKY,
    FOG_PLANE,
    TRACK_MARKS,
    OVERLAYS, 
    SPLASH_SCREEN
} RenderMode;

void set_gl_options( RenderMode mode );

void check_gl_error();

void copy_to_glfloat_array( GLfloat dest[], scalar_t src[], int n );

void init_glfloat_array( int num, GLfloat arr[], ... );

void init_opengl_extensions();

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
