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


#ifndef _PHYS_SIM_H_
#define _PHYS_SIM_H_

#include "course_load.h"

/* Acceleration due to gravity m/s^2 */
#define EARTH_GRAV 9.81

void      increment_turn_fact( player_data_t *plyr, scalar_t amt );
void      set_gravity( bool_t low );
void      set_friction_coeff( scalar_t fric[3] ); 
void      set_friction( bool_t useFriction );
scalar_t  get_min_y_coord();
vector_t  find_course_normal( scalar_t x, scalar_t z );
scalar_t  find_y_coord( scalar_t x, scalar_t z );
terrain_t get_surface_type( scalar_t x, scalar_t z );
void      set_tux_pos( player_data_t *plyr, point_t newPos );
void      update_player_pos( player_data_t *plyr, scalar_t dtime );
void      init_physical_simulation();

#endif /* _PHYS_SIM_H_ */
