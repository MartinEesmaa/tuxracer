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
#include "fog.h"
#include "viewfrustum.h"

/* Time constant for automatic steering centering (s) */
#define TURN_DECAY_TIME_CONSTANT 0.5

static bool_t right_turn;
static bool_t left_turn;
static bool_t paddling;

void racing_init() 
{
    player_data_t *plyr = get_player_data( local_player() );

    glutDisplayFunc( main_loop );
    glutIdleFunc( main_loop );
    glutReshapeFunc( reshape );
    glutMotionFunc( NULL );
    glutPassiveMotionFunc( NULL );
    glutMouseFunc( NULL );

    if ( g_game.prev_mode != PAUSED ) {
	g_game.time = 0.0;

	init_physical_simulation();
	clear_particles();
	set_view_mode( plyr, ABOVE );

	left_turn = right_turn = paddling = False;
	plyr->control.is_paddling = False;

	plyr->health = 1.0;
    }
}

void racing_loop( scalar_t time_step )
{
    int width, height;
    player_data_t *plyr = get_player_data( local_player() );

    width = getparam_x_resolution();
    height = getparam_y_resolution();

    check_gl_error();

    new_frame_for_fps_calc();

    clear_rendering_context();

    setup_fog();

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

    if ( paddling && plyr->control.is_paddling == False ) {
	print_debug( DEBUG_CONTROL, "paddling on" );
	plyr->control.is_paddling = True;
	plyr->control.paddle_time = g_game.time;
    }

    update_player_pos( plyr, time_step );
	
    update_view( plyr );

    setup_view_frustum( plyr, NEAR_CLIP_DIST, 
			getparam_forward_clip_distance() );

    set_course_clipping( True );
    set_course_eye_point( plyr->view.pos );
    render_course();
    draw_background( getparam_fov(), (scalar_t)width/height );
    draw_trees();

    draw_tux();
    draw_tux_shadow();

    if ( getparam_draw_particles() ) {
	update_particles( time_step );
	if ( getparam_draw_particle_shadows() ) {
	    draw_particle_shadows( );
	}
	draw_particles( plyr );
    }

    print_time();
    print_fps();

    if ( debug_mode_is_active( DEBUG_HEALTH ) ) {
	print_health(plyr->health * 100);
    }

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

START_KEYBOARD_CB( paddle_cb )
{
    paddling = !release;
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

START_KEYBOARD_CB( pause_cb )
{
    if ( release ) return;
    g_game.mode = PAUSED;
}
END_KEYBOARD_CB

void racing_register()
{
    int status = 0;

    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
				"escape", getparam_quit_key, quit_racing_cb );
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
				"j", getparam_turn_left_key, turn_left_cb );
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
				"l", getparam_turn_right_key, turn_right_cb );
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
				"space", getparam_brake_key, brake_cb );
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
				"k", getparam_paddle_key, paddle_cb );
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
				"1", getparam_above_view_key, above_view_cb );
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
				"2", getparam_behind_view_key, 
				behind_view_cb );
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
				"3", getparam_eye_view_key, eye_view_cb );
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
				"=", getparam_screenshot_key, screenshot_cb );
    status |= add_keymap_entry( RACING, CONFIGURABLE_KEY, 
				"p", getparam_pause_key, pause_cb );

    check_assertion( status == 0, "out of keymap entries" );

    register_loop_funcs( RACING, racing_init, racing_loop );
}
