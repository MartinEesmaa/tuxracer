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


#ifndef _TEXTURES_H_
#define _TEXTURES_H_

#define BACKGROUND_TEX 0
#define TREE_TEX       1
#define ICE_TEX        2
#define ROCK_TEX       3
#define SNOW_TEX       4

#define NUM_TEXTURES   5

#define TEX_SCALE 6

GLuint* get_tex_names();
void init_textures();
void load_texture( int texnum, char *filename );

#endif /* _TEXTURES_H_ */

