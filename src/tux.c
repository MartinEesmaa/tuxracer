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
#include "hier_cb.h"
#include "hier.h"
#include "gl_util.h"

#define MAX_ARM_ANGLE 30.0

static void register_tux_callbacks( Tcl_Interp *ip );

static bool_t    tuxLoaded = False;
static char*     tuxRootNode;
static char*     tuxLeftShoulderJoint;
static char*     tuxRightShoulderJoint;
static char*     tuxLeftHipJoint;
static char*     tuxRightHipJoint;
static char*     tuxNeck;
static char*     tuxHead;

void adjust_tux_joints( scalar_t turnFact, bool_t isBraking )
{
    scalar_t armAngle;
    /* move arms */

    reset_scene_node( tuxLeftShoulderJoint );
    reset_scene_node( tuxRightShoulderJoint );
    
    if ( isBraking ) {
        rotate_scene_node( tuxLeftShoulderJoint, 'z', -MAX_ARM_ANGLE );
        rotate_scene_node( tuxRightShoulderJoint, 'z', -MAX_ARM_ANGLE );
    } else {
        armAngle = -fabs(turnFact * MAX_ARM_ANGLE);
        if ( turnFact < 0 ) {
            rotate_scene_node( tuxLeftShoulderJoint, 'z', armAngle );
        } else if ( turnFact > 0) {
            rotate_scene_node( tuxRightShoulderJoint, 'z', armAngle );
        } 
    } 

    reset_scene_node( tuxNeck );
    rotate_scene_node( tuxNeck, 'z', 20 );
    reset_scene_node( tuxHead );
    rotate_scene_node( tuxHead, 'z', 20 );
}

void draw_tux()
{
    GLfloat light_position[4];
    GLfloat light_intensity[4];

    GLfloat dummy_colour[]  = { 0.0, 0.0, 0.0, 1.0 };

    /* XXX: For some reason, inserting this call here makes Tux render
     * with correct lighting under Mesa 3.1. I'm guessing it's a Mesa bug.
     */
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, dummy_colour );

    set_gl_options( TUX );

    /* Turn on lights
     */
    glEnable( GL_LIGHT0 );
    glEnable( GL_LIGHT1 );
    glEnable( GL_LIGHT2 );

    /* Set up light sources
     */
    light_position[0] = 20.;
    light_position[1] = 23.;
    light_position[2] = 40.;
    light_position[3] = 0;
    light_intensity[0] = 0.7;
    light_intensity[1] = 0.7;
    light_intensity[2] = 0.7;
    light_intensity[3] = 1.0;
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_intensity);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_intensity);

    light_position[0] = 25;
    light_position[1] = 50;
    light_position[2] = -50;
    light_position[3] = 0;
    light_intensity[0] = 0.3;
    light_intensity[1] = 0.3;
    light_intensity[2] = 0.3;
    light_intensity[3] = 1.0;
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_intensity);

    light_position[0] = 25;
    light_position[1] = 50;
    light_position[2] = -50;
    light_position[3] = 0;
    light_intensity[0] = 0.7;
    light_intensity[1] = 0.7;
    light_intensity[2] = 0.7;
    light_intensity[3] = 1.0;
    glLightfv(GL_LIGHT3, GL_POSITION, light_position);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, light_intensity);
    glLightfv(GL_LIGHT3, GL_SPECULAR, light_intensity);

    glShadeModel( GL_SMOOTH );

    draw_scene_graph( tuxRootNode );

    glShadeModel( GL_FLAT );
} 

void load_tux()
{
    char cwd[BUFF_LEN];

    if ( tuxLoaded == True ) 
        return;

    tuxLoaded = True;

    registerHierCallbacks( g_game.tcl_interp );
    register_tux_callbacks( g_game.tcl_interp );

    initialize_scene_graph();

    if ( getcwd( cwd, BUFF_LEN ) == NULL ) {
	perror( "getcwd" );
	exit(-1);
    }

    if ( chdir( get_data_dir() ) != 0 ) {
        fprintf( stderr, "Couldn't chdir to %s: %s\n", 
		 get_data_dir(), strerror(errno) );
        exit(-1);
    } 

    if ( Tcl_EvalFile( g_game.tcl_interp, "./tux.tcl") == TCL_ERROR ) {
        fprintf( stderr, "Error evalating %s/tux.tcl: %s\n", 
		 get_data_dir(), g_game.tcl_interp->result );
        exit(-1);
    } 

    assert( !Tcl_InterpDeleted( g_game.tcl_interp ) );

    if ( chdir( cwd ) != 0 ) {
        perror("chdir");
        exit(-1);
    } 
} 

char* get_tux_root_node() { return tuxRootNode; } 
char* get_tux_left_shoulder_joint() { return tuxLeftShoulderJoint; } 
char* get_tux_right_shoulder_joint() { return tuxRightShoulderJoint; } 
char* get_tux_left_hip_joint() { return tuxLeftHipJoint; } 
char* get_tux_right_hip_joint() { return tuxRightHipJoint; } 
char* get_tux_neck() { return tuxNeck; } 
char* get_tux_head() { return tuxHead; } 

static int head_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s <head joint>", argv[0] );
        return TCL_ERROR;
    } 

    tuxHead = string_copy( argv[1] );

    return TCL_OK;
} 

static int neck_cb ( ClientData cd, Tcl_Interp *ip, 
		     int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s <neck joint>", argv[0] );
        return TCL_ERROR;
    } 

    tuxNeck = string_copy( argv[1] );

    return TCL_OK;
} 

static int root_node_cb ( ClientData cd, Tcl_Interp *ip, 
			  int argc, char *argv[]) 
{
    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s <root node>", argv[0] );
        return TCL_ERROR;
    } 

    tuxRootNode = string_copy( argv[1] );

    return TCL_OK;
} 

static int left_shoulder_cb ( ClientData cd, Tcl_Interp *ip, 
			      int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s <l. shoulder joint>", argv[0] );
        return TCL_ERROR;
    } 

    tuxLeftShoulderJoint = string_copy( argv[1] );

    return TCL_OK;
} 

static int right_shoulder_cb ( ClientData cd, Tcl_Interp *ip, 
			       int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s <r. shoulder joint>", argv[0] );
        return TCL_ERROR;
    } 

    tuxRightShoulderJoint = string_copy( argv[1] );

    return TCL_OK;
} 

static int left_hip_cb ( ClientData cd, Tcl_Interp *ip, 
			 int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s <l. shoulder joint>", argv[0] );
        return TCL_ERROR;
    } 

    tuxLeftHipJoint = string_copy( argv[1] );

    return TCL_OK;
} 

static int right_hip_cb ( ClientData cd, Tcl_Interp *ip, 
			  int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s <r. shoulder joint>", argv[0] );
        return TCL_ERROR;
    } 

    tuxRightHipJoint = string_copy( argv[1] );

    return TCL_OK;
} 

static void register_tux_callbacks( Tcl_Interp *ip )
{
    Tcl_CreateCommand (ip, "tux_root_node", root_node_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_left_shoulder",  left_shoulder_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_right_shoulder",  right_shoulder_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_left_hip",  left_hip_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_right_hip",  right_hip_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_neck",  neck_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_head", head_cb,  0,0);
}

