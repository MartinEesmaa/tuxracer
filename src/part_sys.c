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
#include "part_sys.h"
#include "phys_sim.h"
#include "gl_util.h"

/* This constant is here as part of a debugging check to prevent an infinite 
   number of particles from being created */
#define MAX_PARTICLES 500000

#define START_RADIUS 0.1
#define PART_SIZE    0.07
#define PART_SPEED   1.3
#define CONE_SIZE    0.7
#define MIN_AGE     -0.2
#define MAX_AGE      1.

typedef struct _Particle {
    point_t pt[3];
    scalar_t age;
    scalar_t death;
    vector_t vel;
    struct _Particle *next;
} Particle;

static colour_t partColour = { 0.69, 0.72, 0.906 };
static Particle* head = NULL;
static int num_particles = 0;

scalar_t frand() 
{
    return (scalar_t)rand()/RAND_MAX;
} 

void create_new_particles( point_t loc, vector_t vel, int num ) 
{
    Particle *newp;
    int i;

    /* Debug check to track down infinite particle bug */
    if ( num_particles + num > MAX_PARTICLES ) {
        fprintf( stderr, 
        "tuxracer: Maximum number of particles exceeded.\n"
        "*** Please help the Tux Racer developers by reporting this bug \n"
        "*** to <tuxracer-devel@lists.sourceforge.net>\n" );
        assert( 0 );
    } 

    for (i=0; i<num; i++) {

        newp = (Particle*)malloc( sizeof( Particle) );

        if ( newp == NULL ) {
            fprintf( stderr, "tuxracer: Out of memory.\n" );
            exit( -1 );
        } 

        num_particles += 1;

        newp->next = head;
        head = newp;

        newp->pt[0].x = loc.x + 2.*(frand() - 0.5) * START_RADIUS;
        newp->pt[0].y = loc.y;
        newp->pt[0].z = loc.z + 2.*(frand() - 0.5) * START_RADIUS;

        newp->pt[1].x = newp->pt[0].x + 2.*(frand() - 0.5) * PART_SIZE;
        newp->pt[1].y = newp->pt[0].y + 2.*(frand() - 0.5) * PART_SIZE;
        newp->pt[1].z = newp->pt[0].z + 2.*(frand() - 0.5) * PART_SIZE;

        newp->pt[2].x = newp->pt[0].x + 2.*(frand() - 0.5) * PART_SIZE;
        newp->pt[2].y = newp->pt[0].y + 2.*(frand() - 0.5) * PART_SIZE;
        newp->pt[2].z = newp->pt[0].z + 2.*(frand() - 0.5) * PART_SIZE;

        newp->age = frand() * MIN_AGE;
        newp->death = frand() * MAX_AGE;

        newp->vel = add_vectors( 
	    vel, make_vector( 2.*(frand() - 0.5) * CONE_SIZE, 
			      0.,
			      2.*(frand() - 0.5) * CONE_SIZE  ) );

        normalize_vector( &newp->vel );
        newp->vel = scale_vector( (frand() + 0.5 ) * PART_SPEED, newp->vel );
    }
} 

void update_particles( scalar_t time_step )
{
    Particle **p, *q;
    scalar_t ycoord;
    int i;

    for (p = &head; *p != NULL; ) {
        (**p).age += time_step;

        if ( (**p).age < 0 ) continue;

        for (i=0; i<3; i++) {
            (**p).pt[i] = move_point( (**p).pt[i], 
				      scale_vector( time_step, (**p).vel ) );

            ycoord = find_y_coord( (**p).pt[i].x, (**p).pt[i].z );

            if ( (**p).pt[i].y < ycoord - 3 ) {
                (**p).age = (**p).death + 1;
            } 
        } 

        if ( (**p).age >= (**p).death ) {
            q = *p;
            *p = q->next;
            free(q);
            num_particles -= 1;
            continue;
        } 

        (**p).vel.y += -EARTH_GRAV * time_step;
        p = &( (**p).next );
    } 
} 

void draw_particles()
{
    scalar_t alpha;
    Particle *p;
    int i;

    set_gl_options( PARTICLES );
    for (p=head; p!=NULL; p = p->next) {
        if ( p->age < 0 ) continue;

        alpha = ( p->death - p->age ) / p->death;

        glBegin( GL_TRIANGLES );
        glColor4f( partColour.r, partColour.g, partColour.b, alpha );
        for ( i=0; i<3; i++) {
            glVertex3f( p->pt[i].x, p->pt[i].y, p->pt[i].z );
        } 
        glEnd();
    } 

} 

void clear_particles()
{
    Particle *p, *q;

    p = head;
    for (;;) {
        if (p == NULL ) break;
        q=p;
        p=p->next;
        free(q);
    } 
    head = NULL;
    num_particles = 0;
} 
