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
#include "course_load.h"

#define DTHETA 0.2
#define MAXTHETA  10
#define MINTHETA -10

static scalar_t theta;
static scalar_t sign;

void init_preview()
{
    theta = 0;
    sign = 1;
} 

void update_preview()
{
    scalar_t courseWidth, courseLength;
    point_t  eyePt;
    point_t  coursePt;
    scalar_t courseAngle;

    get_course_dimensions( &courseWidth, &courseLength );
    courseAngle = get_course_angle();

    eyePt = make_point( sin( theta * M_PI / 180.0 ) * courseLength,
			-courseLength * tan( courseAngle*M_PI/180.0 ) / 2.,
			-cos( theta * M_PI / 180.0 ) * courseLength );

    coursePt = make_point( 0.0, 
			   -courseLength * tan( courseAngle*M_PI/180.0 ) / 2.,
			   -courseLength / 2.);

    gluLookAt( eyePt.x, eyePt.y, eyePt.z, coursePt.x, coursePt.y, coursePt.z,
               0, 1, 0 );

    theta += sign*DTHETA;
    if (theta > MAXTHETA ) {
        theta = MAXTHETA;
        sign = -1;
    } else if ( theta < MINTHETA ) {
        theta = MINTHETA;
        sign = +1;
    } 
} 
