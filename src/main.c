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
#include "course_load.h"
#include "course_render.h"
#include "textures.h"
#include "tux.h"
#include "phys_sim.h"
#include "view.h"
#include "part_sys.h"
#include "keyframe.h"
#include "preview.h"
#include "gl_util.h"
#include "screenshot.h"
#include "fps.h"
#include "tux_shadow.h"
#include "game_config.h"
#include "loop.h"
#include "multiplayer.h"
#include "render_util.h"
#include "start_screen.h"
#include "intro.h"
#include "racing.h"
#include "game_over.h"
#include "keyboard.h"

#define WINDOW_TITLE "Tux Racer (c) 1999-2000 Jasmin F. Patry"

/*
 * Globals 
 */

game_data_t g_game;


/*
 * Static Variables
 */

static int glutWindow;


/* 
 * Function definitions
 */


/* This function is called on exit */
void cleanup(void)
{
    write_config_file();
    if ( get_fullscreen() ) {
	glutLeaveGameMode();
    }
}

int main( int argc, char **argv ) 
{
    int width, height;

    /* Seed the random number generator */
    srand( time(NULL) );


    /*
     * Set up the game configuration
     */

    /* Don't support multiplayer, yet... */
    g_game.num_players = 1;

    /* Create a Tcl interpreter */
    g_game.tcl_interp = Tcl_CreateInterp();

    /* Setup the configuration variables and read the ~/.tuxracer file */
    init_game_configuration();
    read_config_file();

    /* Set up a function to clean up when program exits */
    if ( atexit( cleanup ) != 0 ) {
	perror( "atexit" );
    }


    /* 
     * Initialize GLUT 
     */

    /* Let GLUT process its command-line options */
    glutInit( &argc, argv );

#ifdef USE_STENCIL_BUFFER
    glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STENCIL );
#else
    glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
#endif

    /* Create a window */
    if ( get_fullscreen() ) {
	glutInitWindowPosition( 0, 0 );
	glutEnterGameMode();
    } else {
	/* Set the initial window size */
	width = get_x_resolution();
	height = get_y_resolution();
	glutInitWindowSize( width, height );

	glutWindow = glutCreateWindow( WINDOW_TITLE );

	if ( glutWindow == 0 ) {
	    fprintf( stderr, "Couldn't create a window.\n" );
	    exit(1);
	} 
    }


    /* 
     * Initial OpenGL settings 
     */

    /* Maximum quality. */
    glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    glHint( GL_FOG_HINT, GL_NICEST );

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    /* Fog settings */
    configure_fog();


    /* 
     * Load the game data and initialize game state
     */
    register_course_load_tcl_callbacks( g_game.tcl_interp );
    register_key_frame_callbacks( g_game.tcl_interp );

    load_tux();
    init_textures();

    select_course( 1 );
    init_preview();

    g_game.mode = START;

    start_screen_register();
    intro_register();
    racing_register();
    game_over_register();

    init_keyboard();
    
    /* We use this to "prime" the GLUT loop */
    glutIdleFunc( main_loop );

    
    /* 
     * ...and off we go!
     */
    glutMainLoop();

    return 0;
} 

