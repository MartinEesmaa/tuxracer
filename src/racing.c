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
#include "tux.h"
#include "tux_shadow.h"
#include "phys_sim.h"
#include "part_sys.h"
#include "screenshot.h"

/* Time constant for automatic steering centering (s) */
#define TURN_DECAY_TIME_CONSTANT 0.5

static bool_t right_turn;
static bool_t left_turn;

void racing_init() 
{
    player_data_t *plyr = get_player_data( local_player() );

    g_game.time = 0.0;

    glutDisplayFunc( main_loop );
    glutIdleFunc( main_loop );
    glutReshapeFunc( reshape );
    glutMotionFunc( NULL );
    glutPassiveMotionFunc( NULL );
    glutMouseFunc( NULL );

    init_physical_simulation();
    clear_particles();
    set_view_mode( plyr, ABOVE );

    left_turn = right_turn = False;
}

void racing_loop( scalar_t time_step )
{
    int width, height;
    player_data_t *plyr = get_player_data( local_player() );

    width = get_x_resolution();
    height = get_y_resolution();

    check_gl_error();

    new_frame_for_fps_calc();

    clear_rendering_context();

    if ( left_turn ^ right_turn ) {
	increment_turn_fact( plyr, 
			     ( left_turn ? -1 : 1 ) * 0.2 * time_step / 0.05 );
    } else {
        /* Automatically centre steering */
        if ( time_step < TURN_DECAY_TIME_CONSTANT ) {
	    plyr->control.turn_fact *= 1.0 - time_step/TURN_DECAY_TIME_CONSTANT;
        } else {
	    plyr->control.turn_fact = 0.0;
        }
    }

    update_player_pos( plyr, time_step );
	
    update_view( plyr );

    set_course_clipping( True );
    set_course_eye_point( plyr->view.pos );
    set_course_fog( True );
    render_course();
    draw_background( get_fov(), (scalar_t)width/height );
    draw_trees();

    draw_tux();
    draw_tux_shadow();

    update_particles( time_step );
    draw_particles();

    print_time();

    print_fps();

    reshape( width, height );

    glutSwapBuffers();

    g_game.time += time_step;
} 


START_KEYBOARD_CB( quit_racing_cb )
{
    if ( release ) return;
    g_game.mode = GAME_OVER;
}
END_KEYBOARD_CB


START_KEYBOARD_CB( turn_left_cb )
{
    left_turn = !release;
}
END_KEYBOARD_CB


START_KEYBOARD_CB( turn_right_cb )
{
    right_turn = !release;
}
END_KEYBOARD_CB


START_KEYBOARD_CB( brake_cb )
{
    plyr->control.is_braking = !release;
}
END_KEYBOARD_CB

START_KEYBOARD_CB( above_view_cb )
{
    if ( release ) return;
    set_view_mode( plyr, ABOVE );
}
END_KEYBOARD_CB

START_KEYBOARD_CB( behind_view_cb )
{
    if ( release ) return;
    set_view_mode( plyr, BEHIND );
}
END_KEYBOARD_CB

START_KEYBOARD_CB( eye_view_cb )
{
    if ( release ) return;
    set_view_mode( plyr, PENGUIN_EYE );
}
END_KEYBOARD_CB

START_KEYBOARD_CB( screenshot_cb )
{
    if ( release ) return;
    screenshot();
}
END_KEYBOARD_CB

void racing_register()
{
    int status = 0;

    status |= add_keymap_entry( 
	RACING, CONFIGURABLE_KEY, "escape", get_quit_key, quit_racing_cb );
    status |= add_keymap_entry( 
	RACING, CONFIGURABLE_KEY, "j", get_turn_left_key, turn_left_cb );
    status |= add_keymap_entry( 
	RACING, CONFIGURABLE_KEY, "l", get_turn_right_key, turn_right_cb );
    status |= add_keymap_entry(
	RACING, CONFIGURABLE_KEY, "space", get_brake_key, brake_cb );
    status |= add_keymap_entry(
	RACING, CONFIGURABLE_KEY, "1", get_above_view_key, above_view_cb );
    status |= add_keymap_entry(
	RACING, CONFIGURABLE_KEY, "2", get_behind_view_key, behind_view_cb );
    status |= add_keymap_entry(
	RACING, CONFIGURABLE_KEY, "3", get_eye_view_key, eye_view_cb );
    status |= add_keymap_entry(
	RACING, CONFIGURABLE_KEY, "=", get_screenshot_key, screenshot_cb );

    assert( status == 0 );

    register_loop_funcs( RACING, racing_init, racing_loop );
}
