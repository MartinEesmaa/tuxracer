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

static const colour_t text_colour = { 0.0, 0.0, 0.0 };

void start_screen_init() 
{
    init_preview();
    
    glutDisplayFunc( main_loop );
    glutIdleFunc( main_loop );
    glutReshapeFunc( reshape );
    glutMotionFunc( NULL );
    glutPassiveMotionFunc( NULL );
}

void start_screen_loop()
{
    char buff[BUFF_LEN];

    check_gl_error();

    new_frame_for_fps_calc();
    calc_time_step();

    clear_rendering_context();

    update_preview();

    set_course_clipping( False );
    set_course_fog( False );
    draw_trees( );
    render_course( );

    flat_mode();

    draw_overlay();

    glColor3f( text_colour.r, text_colour.g, text_colour.b );

    glRasterPos2i( 240, 400 );
    print_string( GLUT_BITMAP_TIMES_ROMAN_24, "TUX RACER" );

    glRasterPos2i( 60, 320 );
    sprintf( buff, "1-9: Select Course" );
    print_string( GLUT_BITMAP_TIMES_ROMAN_24, buff );

    glRasterPos2i( 60, 272 );
    sprintf( buff, "N: Next Course" );
    print_string( GLUT_BITMAP_TIMES_ROMAN_24, buff );

    glRasterPos2i( 60, 224 );
    sprintf( buff, "P: Previous Course" );
    print_string( GLUT_BITMAP_TIMES_ROMAN_24, buff );

    glRasterPos2i( 60, 176 );
    print_string( GLUT_BITMAP_TIMES_ROMAN_24, "S: Start Race" );

    /*
    glRasterPos2i( 60, 128 );
    if ( get_control_mode() == MOUSE ) {
        print_string( GLUT_BITMAP_TIMES_ROMAN_24, 
            "C: Toggle Control (Now: Mouse)" );
    } else {
        print_string( GLUT_BITMAP_TIMES_ROMAN_24, 
            "C: Toggle Control (Now: Keyboard)" );
    } 
    */

    glRasterPos2i( 60, 80 );
    print_string( GLUT_BITMAP_TIMES_ROMAN_24, "Q:  Quit" );

    print_fps();

    reshape( get_x_resolution(), get_y_resolution() );

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
	if ( get_do_intro_animation() ) {
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

void start_screen_register()
{
    int status = 0;

    status |= 
	add_keymap_entry( START, DEFAULT_CALLBACK, NULL, NULL, start_menu_cb );

    assert( status == 0 );

    register_loop_funcs( START, start_screen_init, start_screen_loop );
}
