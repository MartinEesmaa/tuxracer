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

void set_gl_options( RenderMode mode ) 
{
    /* Must set the following options:
         Enable/Disable:
	   GL_TEXTURE_2D
	   GL_DEPTH_TEST
	   GL_CULL_FACE
	   GL_LIGHTING
	   GL_NORMALIZE
	   GL_ALPHA_TEST
	   GL_BLEND
	   GL_STENCIL_TEST
	   GL_TEXTURE_GEN_S
	   GL_TEXTURE_GEN_T
           
	 Other Functions:
	   glDepthMask
    */

    /*
     * Modify defaults based on rendering mode
     */
    switch( mode ) {
    case TEXT:
        glDisable( GL_TEXTURE_2D );
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
        glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDepthMask( GL_TRUE );
        break;

    case COURSE:
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
	glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glEnable( GL_TEXTURE_GEN_S );
	glEnable( GL_TEXTURE_GEN_T );
	glDepthMask( GL_TRUE );

	glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
	glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
	break;

    case TREES:
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
        glEnable( GL_ALPHA_TEST );
	glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDepthMask( GL_TRUE );

        glAlphaFunc( GL_GEQUAL, 0.9 );
        break;
        
    case PARTICLES:
        glEnable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
        glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glEnable( GL_TEXTURE_GEN_S );
	glEnable( GL_TEXTURE_GEN_T );
	glDepthMask( GL_TRUE );

	glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
	glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );

        break;
        
    case PARTICLE_SHADOWS:
        glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
        glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDepthMask( GL_TRUE );

        break;

    case BACKGROUND:
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
        glDisable( GL_CULL_FACE ); 
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
	glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDepthMask( GL_TRUE );
        break;

    case TUX:
        glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
        glEnable( GL_LIGHTING );
	glEnable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
	glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDepthMask( GL_TRUE );

        break;

    case TUX_SHADOW:
#ifdef USE_STENCIL_BUFFER
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
	glEnable( GL_BLEND );
	glEnable( GL_STENCIL_TEST );
	glDepthMask( GL_FALSE );

	glStencilFunc( GL_EQUAL, 0, ~0 );
	glStencilOp( GL_KEEP, GL_KEEP, GL_INCR );
#else
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_ALPHA_TEST );
	glEnable( GL_BLEND );
	glDisable( GL_STENCIL_TEST );
	glDepthMask( GL_TRUE );
#endif
	break;

    default:
	code_not_reached();
    } 
} 

void check_gl_error()
{
    GLenum error;
    error = glGetError();
    if ( error != GL_NO_ERROR ) {
	print_warning( CRITICAL_WARNING, 
		       "OpenGL Error: %s", gluErrorString( error ) );
	fflush( stderr );
    }
}

void copy_to_glfloat_array( GLfloat dest[], scalar_t src[], int n )
{
    int i;
    for (i=0; i<n; i++) {
	dest[i] = src[i];
    }
}

void init_glfloat_array( int num, GLfloat arr[], ... )
{
    int i;
    va_list args;

    va_start( args, arr );

    for (i=0; i<num; i++) {
	arr[i] = va_arg(args, double);
    }

    va_end( args );
}
