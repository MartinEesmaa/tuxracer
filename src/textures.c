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
#include "textures.h"
#include "image.h"

static GLuint tex_names[NUM_TEXTURES];
static bool_t initialized = False;

GLuint* get_tex_names() { return tex_names; }

void init_textures() 
{
    int i;
    initialized = True;
    for (i=0; i<NUM_TEXTURES; i++) {
        tex_names[i] = 0;
    } 
} 

int get_min_filter()
{
    switch( getparam_mipmap_type() ) {
    case 0: 
	return GL_NEAREST;
    case 1:
	return GL_LINEAR;
    case 2: 
	return GL_NEAREST_MIPMAP_NEAREST;
    case 3: 
	return GL_LINEAR_MIPMAP_NEAREST;
    case 4: 
	return GL_NEAREST_MIPMAP_LINEAR;
    case 5: 
	return GL_LINEAR_MIPMAP_LINEAR;
    default:
	return GL_LINEAR_MIPMAP_NEAREST;
    }
}

bool_t load_texture( int texnum, char *filename )
{
    IMAGE *texImage;

    if ( initialized == False ) {
        check_assertion( 0, "texture module not initialized" );
    } 

    if ( glIsTexture( tex_names[texnum] ) ) {
        glDeleteTextures( 1, &tex_names[texnum] );
    } 

    glGenTextures( 1, &tex_names[texnum] );
    glBindTexture( GL_TEXTURE_2D, tex_names[texnum] );

    texImage = ImageLoad( filename );

    if ( texImage == NULL ) {
	print_warning( IMPORTANT_WARNING, 
		       "couldn't load image %s", filename );
	return False;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    gluBuild2DMipmaps( GL_TEXTURE_2D, texImage->sizeZ, texImage->sizeX,
        texImage->sizeY, texImage->sizeZ == 3 ? GL_RGB : GL_RGBA, 
        GL_UNSIGNED_BYTE, texImage->data );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                     get_min_filter() );

    switch ( texnum ) {
    case TREE_TEX:
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        break;
    case ICE_TEX:
    case ROCK_TEX:
    case SNOW_TEX:
    case BACKGROUND_TEX:
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
        break;
    default:
        code_not_reached();
    } 

    free( texImage->data );
    free( texImage );

    return True;
} 

