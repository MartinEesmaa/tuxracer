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
#include "fonts.h"
#include "tux.h"
#include "phys_sim.h"
#include "part_sys.h"
#include "keyframe.h"
#include "preview.h"
#include "gl_util.h"
#include "game_config.h"
#include "loop.h"
#include "render_util.h"
#include "splash_screen.h"
#include "intro.h"
#include "racing.h"
#include "game_over.h"
#include "paused.h"
#include "reset.h"
#include "keyboard.h"
#include "fog.h"
#include "lights.h"
#include "multiplayer.h"
#include "audio_data.h"
#include "audio.h"
#include "ui_mgr.h"
#include "course_mgr.h"
#include "game_type_select.h"
#include "race_select.h"
#include "event_select.h"
#include "save.h"
#include "credits.h"
#include "joystick.h"


#define WINDOW_TITLE "Tux Racer " VERSION

#define GAME_INIT_SCRIPT "tuxracer_init.tcl"

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
    write_saved_games();
    if ( getparam_fullscreen() ) {
	glutLeaveGameMode();
    }
}


#if defined (HAVE_SDL) && defined (HAVE_SDL_MIXER)

void setup_sdl() 
{
    int hz, channels, buffer;
    Uint16 format;
    Uint32 flags;

    flags = 0;

    if ( getparam_no_audio() == False ) {
	flags |= SDL_INIT_AUDIO;
    }
 
#ifdef HAVE_SDL_JOYSTICKOPEN
    flags |= SDL_INIT_JOYSTICK;
#endif

    /*
     * Initialize SDL
     */
    if ( SDL_Init(flags) < 0 ) {
	handle_error(1, "Couldn't initialize SDL: %s\n",SDL_GetError());
    }

    if ( getparam_no_audio() == False ) {
	/* Open the audio device */
	switch (getparam_audio_freq_mode()) {
	case 0:
	    hz = 11025;
	    break;
	case 1:
	    hz = 22050;
	    break;
	case 2:
	    hz = 44100;
	    break;
	default:
	    hz = 22050;
	    setparam_audio_freq_mode(1);
	}

	switch ( getparam_audio_format_mode() ) {
	case 0:
	    format = AUDIO_U8;
	    break;
	case 1:
	    format = AUDIO_S16SYS;
	    break;
	default:
	    format = AUDIO_S16SYS;
	    setparam_audio_format_mode( 1 );
	}

	if ( getparam_audio_stereo() ) {
	    channels = 2;
	} else {
	    channels = 1;
	}

	buffer = getparam_audio_buffer_size();

	if ( Mix_OpenAudio(hz, format, channels, buffer) < 0 ) {
	    print_warning( 1,
			   "Warning: Couldn't set %d Hz %d-bit audio\n"
			   "  Reason: %s\n", 
			   hz,  
			   getparam_audio_format_mode() == 0 ? 8 : 16,
			   SDL_GetError());
	} else {
	    print_debug( DEBUG_SOUND,
			 "Opened audio device at %d Hz %d-bit audio",
			 hz, 
			 getparam_audio_format_mode() == 0 ? 8 : 16 );
	}
    }

    atexit(SDL_Quit);
}

#endif /* defined (HAVE_SDL) && defined (HAVE_SDL_MIXER) */

void read_game_init_script()
{
    char cwd[BUFF_LEN];

    if ( getcwd( cwd, BUFF_LEN ) == NULL ) {
	handle_system_error( 1, "getcwd failed" );
    }

    if ( chdir( getparam_data_dir() ) != 0 ) {
	/* Print a more informative warning since this is a common error */
	handle_system_error( 
	    1, "Can't find the tuxracer data "
	    "directory.  Please check the\nvalue of `data_dir' in "
	    "~/.tuxracer/options and set it to the location where you\n"
	    "installed the tuxracer-data files.\n\n"
	    "Couldn't chdir to %s", getparam_data_dir() );
    } 

    if ( Tcl_EvalFile( g_game.tcl_interp, GAME_INIT_SCRIPT) == TCL_ERROR ) {
        handle_error( 1, "error evalating %s/%s: %s\n"
		      "Please check the value of `data_dir' in ~/.tuxracer/options "
		      "and make sure it\npoints to the location of the "
		      "latest version of the tuxracer-data files.", 
		      getparam_data_dir(), GAME_INIT_SCRIPT, 
		      Tcl_GetStringResult( g_game.tcl_interp ) );
    } 

    check_assertion( !Tcl_InterpDeleted( g_game.tcl_interp ),
		     "Tcl interpreter deleted" );

    if ( chdir( cwd ) != 0 ) {
	handle_system_error( 1, "couldn't chdir to %s", cwd );
    } 
}

int main( int argc, char **argv ) 
{
    int width, height;

    /* Print copyright notice */
    fprintf( stderr, "Tux Racer " VERSION " -- a Sunspire Studios Production "
	     "(http://www.sunspirestudios.com)\n"
	     "(c) 1999-2000 Jasmin F. Patry "
	     "<jfpatry@sunspirestudios.com>\n"
	     "\"Tux Racer\" is a trademark of Jasmin F. Patry\n"
	     "Tux Racer comes with ABSOLUTELY NO WARRANTY. "
	     "This is free software,\nand you are welcome to redistribute "
	     "it under certain conditions.\n"
	     "See http://www.gnu.org/copyleft/gpl.html for details.\n\n" );

    /* Init the game clock */
    g_game.secs_since_start = 0;

    /* Seed the random number generator */
    srand( time(NULL) );


    /*
     * Set up the game configuration
     */

    /* Don't support multiplayer, yet... */
    g_game.num_players = 1;

    /* Create a Tcl interpreter */
    g_game.tcl_interp = Tcl_CreateInterp();

    if ( g_game.tcl_interp == NULL ) {
	handle_error( 1, "cannot create Tcl interpreter" ); 
    }

    /* Setup the configuration variables and read the ~/.tuxracer/options file */
    init_game_configuration();
    read_config_file();


    /* Set up the debugging modes */
    init_debug();



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
    if ( getparam_fullscreen() ) {
	glutInitWindowPosition( 0, 0 );
	glutEnterGameMode();
    } else {
	/* Set the initial window size */
	width = getparam_x_resolution();
	height = getparam_y_resolution();
	glutInitWindowSize( width, height );

	if ( getparam_force_window_position() ) {
	    glutInitWindowPosition( 0, 0 );
	}

	glutWindow = glutCreateWindow( WINDOW_TITLE );

	if ( glutWindow == 0 ) {
	    fprintf( stderr, "Couldn't create a window.\n" );
	    exit(1);
	} 
    }

    /* Ingore key-repeat messages */
    glutIgnoreKeyRepeat(1);


    /*
     * Initialize SDL
     */

#if defined(HAVE_SDL) && defined(HAVE_SDL_MIXER)
    setup_sdl();
#endif

    /* Set up a function to clean up when program exits */
    if ( atexit( cleanup ) != 0 ) {
	perror( "atexit" );
    }

    /* 
     * Initial OpenGL settings 
     */
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    init_opengl_extensions();


    /* 
     * Load the game data and initialize game state
     */
    register_game_config_callbacks( g_game.tcl_interp );
    register_course_load_tcl_callbacks( g_game.tcl_interp );
    register_key_frame_callbacks( g_game.tcl_interp );
    register_fog_callbacks( g_game.tcl_interp );
    register_course_light_callbacks( g_game.tcl_interp );
    register_particle_callbacks( g_game.tcl_interp );
    register_texture_callbacks( g_game.tcl_interp );
    register_font_callbacks( g_game.tcl_interp );
    register_sound_tcl_callbacks( g_game.tcl_interp );
    register_sound_data_tcl_callbacks( g_game.tcl_interp );
    register_course_manager_callbacks( g_game.tcl_interp );


    init_saved_games();
    load_tux();
    init_textures();
    init_fonts();
    init_audio_data();
    init_audio();
    init_ui_manager();
    init_course_manager();
    init_joystick();

    /* Read the tuxracer_init.tcl file */
    read_game_init_script();

    /* Need to set up an initial view position for select_course 
       (quadtree simplification)
    */

    g_game.player[local_player()].view.pos = make_point( 0., 0., 0. );

    /* Placeholder name until we give players way to enter name */
    g_game.player[local_player()].name = "noname";

    init_preview();

    splash_screen_register();
    intro_register();
    racing_register();
    game_over_register();
    paused_register();
    reset_register();
    game_type_select_register();
    event_select_register();
    race_select_register();
    credits_register();

    g_game.mode = NO_MODE;
    set_game_mode( SPLASH );

    g_game.difficulty = DIFFICULTY_LEVEL_NORMAL;

    init_keyboard();
    

    glutSetCursor( GLUT_CURSOR_NONE );

    /* We use this to "prime" the GLUT loop */
    glutIdleFunc( main_loop );

    
    /* 
     * ...and off we go!
     */
    glutMainLoop();

    return 0;
} 

