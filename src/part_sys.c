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
#include "tcl_util.h"
#include "course_render.h"
#include "render_util.h"
#include "textures.h"

/* This constant is here as part of a debugging check to prevent an infinite 
   number of particles from being created */
#define MAX_PARTICLES 500000

#define START_RADIUS 0.1
#define PART_SIZE    0.07
#define PART_SPEED   1.3
#define CONE_SIZE    0.7
#define MIN_AGE     -0.2
#define MAX_AGE      1.

#define PARTICLE_SHADOW_HEIGHT 0.05
#define PARTICLE_SHADOW_ALPHA 0.1

typedef struct _Particle {
    point_t pt[3];
    scalar_t terrain_height[3];
    scalar_t age;
    scalar_t death;
    scalar_t alpha;
    vector_t vel;
    struct _Particle *next;
} Particle;

static GLfloat particle_colour_diffuse[4];
static GLfloat particle_colour_specular[4];
static GLfloat particle_shininess;

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
	check_assertion( 0, "maximum number of particles exceeded" );
    } 

    for (i=0; i<num; i++) {

        newp = (Particle*)malloc( sizeof( Particle) );

        if ( newp == NULL ) {
            handle_system_error( 1, "out of memory" );
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

	    (**p).terrain_height[i] = ycoord;
        } 

        if ( (**p).age >= (**p).death ) {
            q = *p;
            *p = q->next;
            free(q);
            num_particles -= 1;
            continue;
        } 

        (**p).alpha = ( (**p).death - (**p).age ) / (**p).death;

        (**p).vel.y += -EARTH_GRAV * time_step;
        p = &( (**p).next );
    } 
} 

void draw_particle_shadows( )
{
    Particle *p;
    int i;

    set_gl_options( PARTICLE_SHADOWS );


    for (p=head; p!=NULL; p = p->next) {
        if ( p->age < 0 ) continue;

	set_material_alpha( black, black, 0.0, 
			    PARTICLE_SHADOW_ALPHA * p->alpha  );

        glBegin( GL_TRIANGLES );

        for ( i=0; i<3; i++) {
            glVertex3f( p->pt[i].x, 
			min( p->pt[i].y, 
			     p->terrain_height[i] + PARTICLE_SHADOW_HEIGHT ), 
			p->pt[i].z );
        } 

        glEnd();
    } 

} 

void draw_particles( player_data_t *plyr )
{
    Particle *p;
    int i;
    vector_t normal;
    GLuint   *tex_names;
    GLfloat tmp;

    set_gl_options( PARTICLES );

    tex_names = get_tex_names();

    glBindTexture( GL_TEXTURE_2D, tex_names[SNOW_TEX] );

    if ( get_course_lighting() ) {
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	glEnable( GL_LIGHTING );

	setup_course_lighting();
	glShadeModel( GL_SMOOTH );
    } else {
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
	glShadeModel( GL_FLAT );
    }

    for (p=head; p!=NULL; p = p->next) {
        if ( p->age < 0 ) continue;

        glBegin( GL_TRIANGLES );

	tmp = particle_colour_diffuse[3];
	particle_colour_diffuse[3] *= p->alpha;
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, 
		      particle_colour_diffuse );
	particle_colour_diffuse[3] = tmp;

	tmp = particle_colour_specular[3];
	particle_colour_specular[3] *= p->alpha;
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, 
		      particle_colour_specular );
	particle_colour_specular[3] = tmp;

	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, particle_shininess );

	normal = cross_product( subtract_points( p->pt[1], p->pt[0] ),
				subtract_points( p->pt[2], p->pt[1] ) );

	if ( dot_product( subtract_points( p->pt[0], plyr->view.pos ),
			  normal ) > 0 ) 
	{
	    normal = scale_vector( -1., normal );
	}
	normalize_vector( &normal );

        for ( i=0; i<3; i++) {
            glNormal3f( normal.x, normal.y, normal.z );
	    glTexCoord2f( p->pt[i].x / TEX_SCALE, p->pt[i].z / TEX_SCALE );
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

void reset_particles() {
    particle_colour_diffuse[0] = light_blue.r;
    particle_colour_diffuse[1] = light_blue.g;
    particle_colour_diffuse[2] = light_blue.b;
    particle_colour_diffuse[3] = 1.0; /* alpha */

    particle_colour_specular[0] = 0.0;
    particle_colour_specular[1] = 0.0;
    particle_colour_specular[2] = 0.0;
    particle_colour_specular[3] = 1.0; /* alpha */

    particle_shininess = 0;
} 

static int particle_colour_cb(ClientData cd, Tcl_Interp *ip, 
			      int argc, char *argv[]) 
{
    double tmp_dbl;
    scalar_t tmp_arr[4];
    bool_t error = False;
    
    if (argc < 3) {
	error = True;
    }

    NEXT_ARG;

    while ( !error && argc > 0 ) {

	if ( strcmp( "-ambient_and_diffuse", *argv ) == 0 ||
	     strcmp( "-diffuse",  *argv ) == 0 ) 
	{
	    NEXT_ARG;
	    if ( argc == 0 ) {
		error = True;
		break;
	    }
	    if ( get_tcl_tuple ( ip, *argv, tmp_arr, 4 ) == TCL_ERROR ) {
		error = True;
		break;
	    }
	    copy_to_glfloat_array( particle_colour_diffuse, tmp_arr, 4 );
	} else if ( strcmp( "-specular", *argv ) == 0 ) {
	    NEXT_ARG;
	    if ( argc == 0 ) {
		error = True;
		break;
	    }
	    if ( get_tcl_tuple ( ip, *argv, tmp_arr, 4 ) == TCL_ERROR ) {
		error = True;
		break;
	    }
	    copy_to_glfloat_array( particle_colour_specular, tmp_arr, 4 );
	} else if ( strcmp( "-shininess", *argv ) == 0 ) {
	    NEXT_ARG;
	    if ( argc == 0 ) {
		error = True;
		break;
	    }
	    if ( Tcl_GetDouble ( ip, *argv, &tmp_dbl ) == TCL_ERROR ) {
		error = True;
		break;
	    }
	    particle_shininess = tmp_dbl;
	} else {
	    print_warning( TCL_WARNING, "tux_particle_colour: unrecognized "
			   "parameter `%s'", *argv );
	}

	NEXT_ARG;
    }

    if ( error ) {
	print_warning( TCL_WARNING, "error in call to tux_particle_colour" );
	Tcl_AppendResult(
	    ip, 
	    "\nUsage: tux_particle_colour "
	    "[-diffuse { r g b a }] "
	    "[-specular { r g b a }] "
	    "[-shininess <value>] "
            , (char *) 0 );
	return TCL_ERROR;
    }

    return TCL_OK;
}

void register_particle_callbacks( Tcl_Interp *ip )
{
    Tcl_CreateCommand (ip, "tux_particle_colour", particle_colour_cb,  0,0);
    Tcl_CreateCommand (ip, "tux_particle_color", particle_colour_cb,  0,0);
}
