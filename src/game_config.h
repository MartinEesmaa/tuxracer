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

char *get_data_dir();
void set_data_dir(char *);
bool_t get_draw_tux_shadow();
void set_draw_tux_shadow( bool_t );
int get_tux_sphere_divisions();
void set_tux_sphere_divisions( int );
int get_tux_shadow_sphere_divisions();
void set_tux_shadow_sphere_divisions( int );
bool_t get_compile_course();
void set_compile_course( bool_t );
bool_t get_use_sphere_display_list();
void set_use_sphere_display_list( bool_t );
bool_t get_display_fps();
void set_display_fps( bool_t );
int get_x_resolution();
void set_x_resolution( int );
int get_y_resolution();
void set_y_resolution( int );
bool_t get_do_intro_animation();
void set_do_intro_animation( bool_t );
int get_mipmap_type();
void set_mipmap_type( int );
bool_t get_fullscreen();
void set_fullscreen( bool_t );
int get_ode_solver();
void set_ode_solver( int );
int get_control_mode();
void set_control_mode( int );
char *get_quit_key();
void set_quit_key( char * );
char *get_turn_left_key();
void set_turn_left_key( char * );
char *get_turn_right_key();
void set_turn_right_key( char * );
char *get_brake_key();
void set_brake_key( char * );
char *get_above_view_key();
void set_above_view_key( char * );
char *get_behind_view_key();
void set_behind_view_key( char * );
char *get_eye_view_key();
void set_eye_view_key( char * );
char *get_screenshot_key();
void set_screenshot_key( char * );
int get_fov();
void set_fov( int );

#endif 


