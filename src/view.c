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
#include "view.h"
#include "phys_sim.h"
#include "tux.h"
#include "hier.h"

/* Minimum height of camera above terrain */
#define MIN_CAMERA_HEIGHT 1.5 

/* The distance of the "BEHIND" camera from Tux (m) */
#define BEHIND_CAMERA_DISTANCE 2.3

/* In "BEHIND" camera mode, camera position is updated using a first order
   filter; this is the time constant of that filter */
#define BEHIND_CAMERA_TIME_CONSTANT 2.0

static point_t   tux_eye_pts[2];
static point_t   tux_view_pt;

void set_view_mode( player_data_t *plyr, view_mode_t mode )
{
    plyr->view.mode = mode;
} 

view_mode_t get_view_mode( player_data_t *plyr )
{
    return plyr->view.mode;
} 

void traverse_dag_for_view_point( scene_node_t *node, matrixgl_t trans )
{
    matrixgl_t new_trans;
    scene_node_t *child;

    check_assertion( node != NULL, "node is NULL" );

    multiply_matrices( new_trans, trans, node->trans );

    if ( node->eye == True ) {
	set_tux_eye( node->which_eye,
		     transform_point( new_trans, make_point( 0., 0., 0. ) ) );
    }

    child = node->child;
    while (child != NULL) {
        traverse_dag_for_view_point( child, new_trans );
        child = child->next;
    } 
}

point_t get_tux_view_pt( player_data_t *plyr ) 
{ 
    matrixgl_t trans;
    char *tux_root_node_name;
    scene_node_t *tux_root_node;

    make_identity_matrix( trans );

    tux_root_node_name = get_tux_root_node();

    if ( get_scene_node( tux_root_node_name, &tux_root_node ) != TCL_OK ) {
	check_assertion(0, "couldn't load tux's root node" );
    } 

    traverse_dag_for_view_point( tux_root_node, trans );

    tux_view_pt = move_point( tux_view_pt, 
			      scale_vector( 0.2, plyr->plane_nml ) );

    return tux_view_pt; 
}

void set_tux_eye( tux_eye_t which_eye, point_t pt ) 
{
    tux_eye_pts[ which_eye ] = pt;

    tux_view_pt.x = ( tux_eye_pts[0].x + tux_eye_pts[1].x ) / 2.0; 
    tux_view_pt.y = ( tux_eye_pts[0].y + tux_eye_pts[1].y ) / 2.0; 
    tux_view_pt.z = ( tux_eye_pts[0].z + tux_eye_pts[1].z ) / 2.0; 
}

void update_view( player_data_t *plyr )
{
    point_t view_pt;
    vector_t view_dir, up_dir, vel_dir, view_vec;
    scalar_t ycoord;

    up_dir = make_vector( 0, 1, 0 );

    vel_dir = plyr->vel;
    normalize_vector( &vel_dir );

    glLoadIdentity();

    switch( plyr->view.mode ) {
    case PENGUIN_EYE:
        view_pt = get_tux_view_pt( plyr );
        view_dir = plyr->direction;
	up_dir = plyr->plane_nml;
        break;

    case BEHIND:
	up_dir = make_vector( 0, 1, 0 );
        view_pt = move_point( 
	    plyr->pos, scale_vector( -BEHIND_CAMERA_DISTANCE, vel_dir ) );
        ycoord = find_y_coord( view_pt.x, view_pt.z );

        if ( view_pt.y < ycoord + MIN_CAMERA_HEIGHT ) {
            view_pt.y = ycoord + MIN_CAMERA_HEIGHT;
	    view_vec = subtract_points( view_pt, plyr->pos );
	    normalize_vector( &view_vec );
	    view_pt = move_point( plyr->pos, 
				  scale_vector( BEHIND_CAMERA_DISTANCE,
						view_vec ) );
        } 

        view_dir = subtract_points( 
	    move_point( plyr->pos, scale_vector( 1.5, vel_dir ) ), 
	    view_pt );
        break;

    case ABOVE:
	up_dir = make_vector( 0, 1, 0 );
        view_pt = plyr->pos;
        view_pt.z += 3.5;
        view_pt.y += 3.5;
        ycoord = find_y_coord( view_pt.x, view_pt.z );

        if ( view_pt.y < ycoord + MIN_CAMERA_HEIGHT ) {
            view_pt.y = ycoord + MIN_CAMERA_HEIGHT;
        } 

        view_dir = subtract_points( plyr->pos, view_pt );
	view_dir.z -= 1.5;

        break;

    default:
	code_not_reached();
    } 

    gluLookAt( view_pt.x, view_pt.y, view_pt.z, 
	       view_pt.x + view_dir.x, 
	       view_pt.y + view_dir.y, 
	       view_pt.z + view_dir.z,
	       up_dir.x, up_dir.y, up_dir.z);
    plyr->view.pos = view_pt;
    plyr->view.dir = view_dir;
    plyr->view.up = up_dir;
} 
