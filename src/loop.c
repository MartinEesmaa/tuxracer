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
#include "loop.h"


/*
 * Types
 */

typedef struct {
    mode_init_func_ptr_t init_func;
    mode_loop_func_ptr_t loop_func;
} mode_funcs_t;


/*
 * Static Variables
 */

static scalar_t clock_time;
static mode_funcs_t mode_funcs[ NUM_GAME_MODES ] = 
{ 
    { NULL, NULL },   /* START */
    { NULL, NULL },   /* INTRO */
    { NULL, NULL },   /* RACING */
    { NULL, NULL }    /* GAME_OVER */
};


/*
 * Function definitions
 */

void register_loop_funcs( game_mode_t mode, mode_init_func_ptr_t init_func,
			  mode_loop_func_ptr_t loop_func )
{
    check_assertion( mode >= 0 && mode < NUM_GAME_MODES,
		     "invalid game mode" );
    mode_funcs[ mode ].init_func = init_func;
    mode_funcs[ mode ].loop_func = loop_func;
}

static scalar_t get_clock_time()
{
    struct timeval tv;
    gettimeofday( &tv, NULL );

    return (scalar_t) tv.tv_sec + (scalar_t) tv.tv_usec * 1.e-6;
} 

void reset_time_step_clock()
{
    clock_time = get_clock_time( );
} 

static scalar_t calc_time_step()
{
    scalar_t cur_time, time_step;

    cur_time = get_clock_time( );
    time_step = cur_time - clock_time;
    if (time_step < EPS ) {
        time_step = EPS;
    }  
    clock_time = cur_time;
    return time_step;
} 

void main_loop()
{
    static game_mode_t prev_mode = NO_MODE;
    game_mode_t cur_mode = g_game.mode;

    if ( getparam_warp_pointer() ) {
	glutWarpPointer( getparam_x_resolution()/2, 
			 getparam_y_resolution()/2 );
    }

    if ( prev_mode != cur_mode ) {
	if ( mode_funcs[ cur_mode ].init_func != NULL ) {
            reset_time_step_clock();
	    ( mode_funcs[ cur_mode ].init_func )( );
	}
	prev_mode = cur_mode;
    }

    if ( mode_funcs[ cur_mode ].loop_func != NULL ) {
        g_game.time_step = calc_time_step();
	( mode_funcs[ cur_mode ].loop_func )( g_game.time_step );
    }
}
