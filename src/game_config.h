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

#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include "tux_types.h"

void init_game_configuration();
void read_config_file();
void write_config_file();
void clear_config_cache();

char *getparam_data_dir();
void setparam_data_dir(char *);

bool_t getparam_draw_tux_shadow();
void setparam_draw_tux_shadow( bool_t );

bool_t getparam_draw_particles();
void setparam_draw_particles( bool_t );

bool_t getparam_draw_particle_shadows();
void setparam_draw_particle_shadows( bool_t );

int getparam_tux_sphere_divisions();
void setparam_tux_sphere_divisions( int );

int getparam_tux_shadow_sphere_divisions();
void setparam_tux_shadow_sphere_divisions( int );

bool_t getparam_nice_fog();
void setparam_nice_fog( bool_t );

bool_t getparam_compile_course();
void setparam_compile_course( bool_t );

bool_t getparam_use_sphere_display_list();
void setparam_use_sphere_display_list( bool_t );

bool_t getparam_display_fps();
void setparam_display_fps( bool_t );

int getparam_x_resolution();
void setparam_x_resolution( int );

int getparam_y_resolution();
void setparam_y_resolution( int );

bool_t getparam_do_intro_animation();
void setparam_do_intro_animation( bool_t );

int getparam_mipmap_type();
void setparam_mipmap_type( int );

bool_t getparam_fullscreen();
void setparam_fullscreen( bool_t );

bool_t getparam_force_window_position();
void setparam_force_window_position( bool_t );

bool_t getparam_warp_pointer();
void setparam_warp_pointer( bool_t );

int getparam_ode_solver();
void setparam_ode_solver( int );

int getparam_control_mode();
void setparam_control_mode( int );

char *getparam_quit_key();
void setparam_quit_key( char * );

char *getparam_turn_left_key();
void setparam_turn_left_key( char * );

char *getparam_turn_right_key();
void setparam_turn_right_key( char * );

char *getparam_brake_key();
void setparam_brake_key( char * );

char *getparam_above_view_key();
void setparam_above_view_key( char * );

char *getparam_behind_view_key();
void setparam_behind_view_key( char * );

char *getparam_eye_view_key();
void setparam_eye_view_key( char * );

char *getparam_screenshot_key();
void setparam_screenshot_key( char * );

int getparam_fov();
void setparam_fov( int );

char *getparam_debug();
void setparam_debug( char * );

int getparam_warning_level();
void setparam_warning_level( int );

#endif 


