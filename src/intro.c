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
#include "fog.h"

void intro_init() 
{
    init_key_frame();

    glutDisplayFunc( main_loop );
    glutIdleFunc( main_loop );
    glutReshapeFunc( reshape );
    glutMotionFunc( NULL );
    glutPassiveMotionFunc( NULL );
}

void intro_loop( scalar_t time_step )
{
    int width, height;
    player_data_t *plyr = get_player_data( local_player() );

    width = getparam_x_resolution();
    height = getparam_y_resolution();

    check_gl_error();

    new_frame_for_fps_calc();

    update_key_frame( plyr, time_step );

    clear_rendering_context();

    setup_fog();

    set_view_mode( plyr, ABOVE );
    update_view( plyr );

    set_course_clipping( True );
    set_course_eye_point( plyr->view.pos );
    render_course( );
    draw_background( getparam_fov(), (scalar_t)width/height );
    draw_trees( plyr );

    draw_tux();
    draw_tux_shadow();

    print_fps();

    reshape( width, height );
    glutSwapBuffers();
} 

void intro_register()
{
    register_loop_funcs( INTRO, intro_init, intro_loop );
    return;
}

