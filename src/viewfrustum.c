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
#include "viewfrustum.h"

static plane_t frustum_planes[6];

/* This will be used as a bitfield to select the "n" and "p" vertices
of the bounding boxes wrt to each plane.  
*/

static char p_vertex_code[6];

void setup_view_frustum( player_data_t *plyr, scalar_t near, scalar_t far )
{
    scalar_t aspect = (scalar_t) getparam_x_resolution() /
	getparam_y_resolution();

    matrixgl_t mat;
    vector_t view_x, view_y, view_z;
    int i;
    point_t pt;
    point_t origin = { 0., 0., 0. };
    scalar_t half_fov = getparam_fov() * M_PI/180.0 * 0.5;
    scalar_t half_fov_horiz = atan( tan( half_fov ) * aspect ); 


    /* create frustum in viewing coordinates */

    /* near */
    frustum_planes[0] = make_plane( 0, 0, 1, near );
    
    /* far */
    frustum_planes[1] = make_plane( 0, 0, -1, -far );

    /* left */
    frustum_planes[2] = make_plane( -cos(half_fov_horiz), 0, 
				    sin(half_fov_horiz), 0 );

    /* right */
    frustum_planes[3] = make_plane( cos(half_fov_horiz), 0, 
				    sin(half_fov_horiz), 0 );

    /* top */
    frustum_planes[4] = make_plane( 0, cos(half_fov),  
				    sin(half_fov), 0 );

    /* bottom */
    frustum_planes[5] = make_plane( 0, -cos(half_fov),  
				    sin(half_fov), 0 );


    /* We now transform frustum to world coordinates */
    view_z = scale_vector( -1, plyr->view.dir );
    view_x = cross_product( plyr->view.up, view_z );
    view_y = cross_product( view_z, view_x );
    normalize_vector( &view_z );
    normalize_vector( &view_x );
    normalize_vector( &view_y );

    make_identity_matrix( mat );

    mat[0][0] = view_x.x;
    mat[0][1] = view_x.y;
    mat[0][2] = view_x.z;

    mat[1][0] = view_y.x;
    mat[1][1] = view_y.y;
    mat[1][2] = view_y.z;

    mat[2][0] = view_z.x;
    mat[2][1] = view_z.y;
    mat[2][2] = view_z.z;

    mat[3][0] = plyr->view.pos.x;
    mat[3][1] = plyr->view.pos.y;
    mat[3][2] = plyr->view.pos.z;
    mat[3][3] = 1;

    for (i=0; i<6; i++) {
	pt = transform_point( mat, move_point( origin, scale_vector( 
	    -frustum_planes[i].d, frustum_planes[i].nml ) ) );

	frustum_planes[i].nml = transform_vector( mat, frustum_planes[i].nml );

	frustum_planes[i].d = -dot_product( 
	    frustum_planes[i].nml,
	    subtract_points( pt, origin ) );
    }

    for (i=0; i<6; i++) {
	p_vertex_code[i] = 0;

	if ( frustum_planes[i].nml.x > 0 ) {
	    p_vertex_code[i] |= 4;
	}
	if ( frustum_planes[i].nml.y > 0 ) {
	    p_vertex_code[i] |= 2;
	}
	if ( frustum_planes[i].nml.z > 0 ) {
	    p_vertex_code[i] |= 1;
	}
    }
}

/* View frustum clipping for AABB (axis-aligned bounding box). See
   Assarsson, Ulf and Tomas M\"oller, "Optimized View Frustum Culling
   Algorithms", unpublished, http://www.ce.chalmers.se/staff/uffe/ .  */
clip_result_t clip_aabb_to_view_frustum( point_t min, point_t max )
{
    vector_t n, p;
    int i;
    bool_t intersect = False;

    for (i=0; i<6; i++) {
	p = make_vector( min.x, min.y, min.z );
	n = make_vector( max.x, max.y, max.z );

	if ( p_vertex_code[i] & 4 ) {
	    p.x = max.x;
	    n.x = min.x;
	}

	if ( p_vertex_code[i] & 2 ) {
	    p.y = max.y;
	    n.y = min.y;
	}

	if ( p_vertex_code[i] & 1 ) {
	    p.z = max.z;
	    n.z = min.z;
	}

	if ( dot_product( n, frustum_planes[i].nml ) +
	     frustum_planes[i].d > 0 )
	{
	    return NotVisible;
	}

	if ( dot_product( p, frustum_planes[i].nml ) +
	     frustum_planes[i].d > 0 )
	{
	    intersect = True;
	}

    }

    if ( intersect ) {
	return SomeClip;
    }

    return NoClip;
}
