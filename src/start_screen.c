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
#include "keyboard.h"
#include "keyframe.h"
#include "course_render.h"
#include "multiplayer.h"
#include "gl_util.h"
#include "fps.h"
#include "loop.h"
#include "render_util.h"
#include "view.h"
#include "preview.h"
#include "screenshot.h"
#include "course_load.h"
#include "fog.h"
#include "viewfrustum.h"

static const colour_t text_colour = { 0.0, 0.0, 0.0 };

static char * menu_text[] = {  "1-9: Course Select",
			       "N: Next Course",
			       "P: Previous Course",
			       "M: Mirror Course",
			       "R: Reload Course",
			       "S: Start Race",
			       "Q: Quit" };

static void print_menu( int x, int starty, int endy, 
			char *menu_text[], int n ) 
{
    int i;

    for ( i=0; i<n; i++ ) {
	glRasterPos2i( x, 
		       (int)( starty + (scalar_t)i/(n-1) * ( endy-starty ) ) );
	print_string( GLUT_BITMAP_TIMES_ROMAN_24, menu_text[i] );
    }
}

void start_screen_init() 
{
    init_preview();
    
    glutDisplayFunc( main_loop );
    glutIdleFunc( main_loop );
    glutReshapeFunc( reshape );
    glutMotionFunc( NULL );
    glutPassiveMotionFunc( NULL );
}

void start_screen_loop( scalar_t time_step )
{
    player_data_t *plyr = get_player_data( local_player() );

    char buff[BUFF_LEN];

    check_gl_error();

    new_frame_for_fps_calc();

    clear_rendering_context();

    update_preview( plyr );

    set_course_clipping( False );
    set_course_eye_point( get_preview_eye_pt() );

    setup_view_frustum( plyr, 1, 10000 );

    disable_fog();

    draw_trees( );
    render_course( );

    flat_mode();

    draw_overlay();

    glColor3f( text_colour.r, text_colour.g, text_colour.b );

    print_string_centered( 400, GLUT_BITMAP_TIMES_ROMAN_24, "TUX RACER" );

    print_menu( 60, 320, 100, menu_text, 
		sizeof( menu_text ) / sizeof( menu_text[0] ) );



    if ( get_course_name() != NULL ) {
	print_string_centered( 50, GLUT_BITMAP_HELVETICA_12, 
			       get_course_name() );
    }

    if ( get_course_author() != NULL ) {
	sprintf( buff, "Designed by %s", get_course_author() );
	print_string_centered( 35, GLUT_BITMAP_HELVETICA_12, buff );
    }

    print_string_centered( 10, GLUT_BITMAP_HELVETICA_12, 
			   "Hint: edit ~/.tuxracer to change game options" );

    print_fps();

    reshape( getparam_x_resolution(), getparam_y_resolution() );

    glutSwapBuffers();
} 

START_KEYBOARD_CB( start_menu_cb )
{
    if ( release ) {
	/* We don't care about key releases */
	return;
    }

    if ( !special ) {
	key = (int) tolower( (char) key );
    }

    switch (key) {
    case 'q':
        exit(0);
        break;
    case '+':
        set_course_lighting( !get_course_lighting() );
        break;
    case 's':
	if ( getparam_do_intro_animation() ) {
	    g_game.mode = INTRO;
	} else {
	    g_game.mode = RACING;
	}
        break;
	/*
    case 'c':
	if ( get_control_mode() == MOUSE ) {
	    set_control_mode( KEYBOARD );
	} else {
	    set_control_mode( MOUSE );
	}
        break;
	*/
    case '=':
        screenshot();
        break;
    case 'm':
	set_course_mirroring( !get_course_mirroring() );
	break;
    case 'n':
	g_game.course.num++;
	if ( ! course_exists( g_game.course.num ) )
	    g_game.course.num = 1;
	select_course( g_game.course.num );
	break;
    case 'p':
	g_game.course.num--;
	if ( ! course_exists( g_game.course.num ) ) {
	    g_game.course.num = 1;
	    while ( course_exists( ++g_game.course.num ) ) ;
	    g_game.course.num--;
	}
	select_course( g_game.course.num );
	break;
    case 'r':
	select_course( g_game.course.num );
	break;
    }

    if ( '1' <= key  && key <= '9' ) {
	if ( course_exists( key - '0' ) ) {
	    g_game.course.num = key - '0';
	    reset_key_frame();
	    select_course( g_game.course.num );
	}
    } 
 
    glutPostRedisplay();
}
END_KEYBOARD_CB

START_KEYBOARD_CB( wireframe_cb )
{
    static bool_t wireframe = False;
    if ( release ) return;

    if ( wireframe ) {
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    } else {
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }
    wireframe = !wireframe;
}
END_KEYBOARD_CB

START_KEYBOARD_CB( increase_detail_cb )
{
    int cur_level;
    int incr;
    if ( release ) return;

    cur_level = getparam_course_detail_level();
    incr = (int) ( cur_level * 0.10 );

    if ( incr < 1 ) {
	incr = 1;
    }

    if ( cur_level <= INT_MAX - incr ) {
	setparam_course_detail_level( cur_level + incr );
    }

    print_debug( DEBUG_QUADTREE, "Current detail level: %d", 
		 getparam_course_detail_level() );
}
END_KEYBOARD_CB

START_KEYBOARD_CB( decrease_detail_cb )
{
    int cur_level;
    int decr;
    if ( release ) return;

    cur_level = getparam_course_detail_level();
    decr = (int) ( cur_level * ( 1.0 - 1./1.10 ) );

    if ( decr < 1 ) {
	decr = 1;
    }

    if ( cur_level > 1 ) {
	setparam_course_detail_level( cur_level - decr );
	
    }

    print_debug( DEBUG_QUADTREE, "Current detail level: %d", 
		 getparam_course_detail_level() );
}
END_KEYBOARD_CB

void start_screen_register()
{
    int status = 0;

    status |= 
	add_keymap_entry( START, DEFAULT_CALLBACK, NULL, NULL, start_menu_cb );
    status |= 
	add_keymap_entry( ALL_MODES, FIXED_KEY, "f11", NULL, wireframe_cb );
    status |= add_keymap_entry( ALL_MODES, FIXED_KEY, "f9", NULL, 
			  decrease_detail_cb );
    status |= add_keymap_entry( ALL_MODES, FIXED_KEY, "f10", NULL, 
			  increase_detail_cb );

    check_assertion( status == 0, "out of keymap entries" );

    register_loop_funcs( START, start_screen_init, start_screen_loop );
}
