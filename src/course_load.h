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


#ifndef _COURSE_LOAD_H_
#define _COURSE_LOAD_H_

/* Convenience macro for accessing terrain elevations */
#define ELEV(x,y) ( elevation[(x) + nx*(y)] )

bool_t course_exists( int num );
void select_course( int num );

scalar_t     *get_course_elev_data();
terrain_t    *get_course_terrain_data();
scalar_t      get_course_angle();
void          get_course_dimensions( scalar_t *width, scalar_t *length );
void          get_course_divisions( int *nx, int *ny );
tree_t       *get_tree_locs();
int           get_num_trees();
scalar_t      get_tree_diam();
scalar_t      get_tree_height();
void          set_start_pt( point2d_t p );
point2d_t     get_start_pt();
polyhedron_t  get_tree_polyhedron();
char         *get_course_author();
char         *get_course_name(); 
light_t      *get_course_lights();
void          set_course_mirroring( bool_t state );
bool_t        get_course_mirroring( );
void          fill_gl_arrays();
void          get_gl_arrays( GLfloat **vertex_normal_arr );

void          register_course_load_tcl_callbacks( Tcl_Interp *interp );

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
