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
#include "game_config.h"
#include "multiplayer.h"
#include "gl_util.h"
#include "fps.h"
#include "render_util.h"
#include "phys_sim.h"
#include "view.h"
#include "course_render.h"
#include "tux.h"
#include "tux_shadow.h"
#include "keyboard.h"
#include "loop.h"
#include "fog.h"

static const colour_t text_colour = { 0.0, 0.0, 0.0 };

void game_over_init() 
{
    glutDisplayFunc( main_loop );
    glutIdleFunc( NULL );
    glutReshapeFunc( reshape );
    glutMotionFunc( NULL );
    glutPassiveMotionFunc( NULL );
}

void game_over_loop( scalar_t time_step )
{
    player_data_t *plyr = get_player_data( local_player() );
    int width, height;
    width = getparam_x_resolution();
    height = getparam_y_resolution();

    check_gl_error();

    new_frame_for_fps_calc();

    clear_rendering_context();

    setup_fog();

    update_player_pos( plyr, EPS );
    update_view( plyr );

    set_course_clipping( True );
    set_course_eye_point( plyr->view.pos );
    render_course();
    draw_background( getparam_fov(), (scalar_t)width/height );
    draw_trees( plyr );

    draw_tux();
    draw_tux_shadow();

    flat_mode();
    draw_overlay();

    print_time();
    print_fps();

    glColor3f( text_colour.r, text_colour.g, text_colour.b );
    print_string_centered( 300, GLUT_BITMAP_TIMES_ROMAN_24, 
			   "GAME OVER" );
    
    print_string_centered( 150, GLUT_BITMAP_TIMES_ROMAN_24, 
			   "Press any key to continue" );

    reshape( width, height );

    glutSwapBuffers();
} 

START_KEYBOARD_CB( game_over_cb )
{
    if ( release ) return;
    g_game.mode = START;
    glutPostRedisplay();
}
END_KEYBOARD_CB

void game_over_register()
{
    int status = 0;

    status |= add_keymap_entry(
	GAME_OVER, DEFAULT_CALLBACK, NULL, NULL, game_over_cb );

    check_assertion( status == 0, "out of keymap entries" );

    register_loop_funcs( GAME_OVER, game_over_init, game_over_loop );
}


