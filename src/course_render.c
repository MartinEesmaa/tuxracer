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
#include "course_render.h"
#include "textures.h"
#include "phys_sim.h"
#include "hier_util.h"
#include "gl_util.h"
#include "render_util.h"
#include "fog.h"
#include "course_quad.h"

/* 
 *  Constants 
 */

#define FLAT_SEGMENT_FRACTION 0.2

#define BACKGROUND_TEXTURE_ASPECT 3.0


/*
 * Statics 
 */

/* The course normal vectors */
static vector_t *nmls = NULL;

/* Should we activate clipping when drawing the course? */
static bool_t clip_course = False;

/* If clipping is active, it will be based on a camera located here */
static point_t eye_pt;

/* Should we activate lighting when drawing the course? */
static bool_t light_course = True;


/* Macros for converting indices in height map to world coordinates */
#define XCD(x) (  (scalar_t)(x) / (nx-1.) * courseWidth )
#define ZCD(y) ( -(scalar_t)(y) / (ny-1.) * courseLength )

#define NORMAL(x, y) ( nmls[ (x) + nx * (y) ] )


/*
 * Function definitions
 */

void set_course_clipping( bool_t state ) { clip_course = state; }
void set_course_eye_point( point_t pt ) { eye_pt = pt; }
void set_course_lighting( bool_t state ) { light_course = state; }
bool_t get_course_lighting() { return light_course; }

vector_t* get_course_normals() { return nmls; } 

void calc_normals()
{
    scalar_t *elevation;
    scalar_t courseWidth, courseLength;
    int nx, ny;
    int x,y;
    point_t p0, p1, p2;
    vector_t n, nml, v1, v2;

    elevation = get_course_elev_data();
    get_course_dimensions( &courseWidth, &courseLength );
    get_course_divisions( &nx, &ny );

    if ( nmls != NULL ) {
        free( nmls );
    } 

    nmls = (vector_t *)malloc( sizeof(vector_t)*nx*ny ); 
    if ( nmls == NULL ) {
	handle_system_error( 1, "malloc failed" );
    }

    for ( y=0; y<ny; y++) {
        for ( x=0; x<nx; x++) {
            nml = make_vector( 0., 0., 0. );

            p0 = make_point( XCD(x), ELEV(x,y), ZCD(y) );

	    /* The terrain is meshed as follows:
	             ...
	          +-+-+-+-+            x<---+
	          |\|/|\|/|                 |
	       ...+-+-+-+-+...              V
	          |/|\|/|\|                 y
	          +-+-+-+-+
		     ...

	       So there are two types of vertices: those surrounded by
	       four triangles (x+y is odd), and those surrounded by
	       eight (x+y is even).
	    */

#define POINT(x,y) make_point( XCD(x), ELEV(x,y), ZCD(y) )

	    if ( (x + y) % 2 == 0 ) {
		if ( x > 0 && y > 0 ) {
		    p1 = POINT(x,  y-1);
		    p2 = POINT(x-1,y-1);
		    v1 = subtract_points( p1, p0 );
		    v2 = subtract_points( p2, p0 );
		    n = cross_product( v2, v1 );

		    check_assertion( n.y > 0, "course normal points down" );

		    normalize_vector( &n );
		    nml = add_vectors( nml, n );

		    p1 = POINT(x-1,y-1);
		    p2 = POINT(x-1,y  );
		    v1 = subtract_points( p1, p0 );
		    v2 = subtract_points( p2, p0 );
		    n = cross_product( v2, v1 );

		    check_assertion( n.y > 0, "course normal points down" );

		    normalize_vector( &n );
		    nml = add_vectors( nml, n );
		} 
		if ( x > 0 && y < ny-1 ) {
		    p1 = POINT(x-1,y  );
		    p2 = POINT(x-1,y+1);
		    v1 = subtract_points( p1, p0 );
		    v2 = subtract_points( p2, p0 );
		    n = cross_product( v2, v1 );

		    check_assertion( n.y > 0, "course normal points down" );

		    normalize_vector( &n );
		    nml = add_vectors( nml, n );

		    p1 = POINT(x-1,y+1);
		    p2 = POINT(x  ,y+1);
		    v1 = subtract_points( p1, p0 );
		    v2 = subtract_points( p2, p0 );
		    n = cross_product( v2, v1 );

		    check_assertion( n.y > 0, "course normal points down" );

		    normalize_vector( &n );
		    nml = add_vectors( nml, n );
		} 
		if ( x < nx-1 && y > 0 ) {
		    p1 = POINT(x+1,y  );
		    p2 = POINT(x+1,y-1);
		    v1 = subtract_points( p1, p0 );
		    v2 = subtract_points( p2, p0 );
		    n = cross_product( v2, v1 );

		    check_assertion( n.y > 0, "course normal points down" );

		    normalize_vector( &n );
		    nml = add_vectors( nml, n );

		    p1 = POINT(x+1,y-1);
		    p2 = POINT(x  ,y-1);
		    v1 = subtract_points( p1, p0 );
		    v2 = subtract_points( p2, p0 );
		    n = cross_product( v2, v1 );

		    check_assertion( n.y > 0, "course normal points down" );

		    normalize_vector( &n );
		    nml = add_vectors( nml, n );
		} 
		if ( x < nx-1 && y < ny-1 ) {
		    p1 = POINT(x+1,y  );
		    p2 = POINT(x+1,y+1);
		    v1 = subtract_points( p1, p0 );
		    v2 = subtract_points( p2, p0 );
		    n = cross_product( v1, v2 );

		    check_assertion( n.y > 0, "course normal points down" );

		    normalize_vector( &n );
		    nml = add_vectors( nml, n );

		    p1 = POINT(x+1,y+1);
		    p2 = POINT(x  ,y+1);
		    v1 = subtract_points( p1, p0 );
		    v2 = subtract_points( p2, p0 );
		    n = cross_product( v1, v2 );

		    check_assertion( n.y > 0, "course normal points down" );

		    normalize_vector( &n );
		    nml = add_vectors( nml, n );

		} 
	    } else {
		/* x + y is odd */
		if ( x > 0 && y > 0 ) {
		    p1 = POINT(x,  y-1);
		    p2 = POINT(x-1,y  );
		    v1 = subtract_points( p1, p0 );
		    v2 = subtract_points( p2, p0 );
		    n = cross_product( v2, v1 );

		    check_assertion( n.y > 0, "course normal points down" );

		    normalize_vector( &n );
		    nml = add_vectors( nml, n );
		} 
		if ( x > 0 && y < ny-1 ) {
		    p1 = POINT(x-1,y  );
		    p2 = POINT(x  ,y+1);
		    v1 = subtract_points( p1, p0 );
		    v2 = subtract_points( p2, p0 );
		    n = cross_product( v2, v1 );

		    check_assertion( n.y > 0, "course normal points down" );

		    normalize_vector( &n );
		    nml = add_vectors( nml, n );
		} 
		if ( x < nx-1 && y > 0 ) {
		    p1 = POINT(x+1,y  );
		    p2 = POINT(x  ,y-1);
		    v1 = subtract_points( p1, p0 );
		    v2 = subtract_points( p2, p0 );
		    n = cross_product( v2, v1 );

		    check_assertion( n.y > 0, "course normal points down" );

		    normalize_vector( &n );
		    nml = add_vectors( nml, n );
		} 
		if ( x < nx-1 && y < ny-1 ) {
		    p1 = POINT(x+1,y  );
		    p2 = POINT(x  ,y+1);
		    v1 = subtract_points( p1, p0 );
		    v2 = subtract_points( p2, p0 );
		    n = cross_product( v1, v2 );

		    check_assertion( n.y > 0, "course normal points down" );

		    normalize_vector( &n );
		    nml = add_vectors( nml, n );
		} 
	    }

            normalize_vector( &nml );
            NORMAL(x,y) = nml;
            continue;
        } 
#undef POINT
    } 
} 

void setup_course_tex_gen()
{
    static GLfloat xplane[4] = { 1.0 / TEX_SCALE, 0.0, 0.0, 0.0 };
    static GLfloat zplane[4] = { 0.0, 0.0, 1.0 / TEX_SCALE, 0.0 };
    glTexGenfv( GL_S, GL_OBJECT_PLANE, xplane );
    glTexGenfv( GL_T, GL_OBJECT_PLANE, zplane );
}

int select_tex( int x, int y, int nx,  terrain_t *terrain )
{
    switch ( terrain[ x + nx*y ] ) {
    case Ice:
        return ICE_TEX;
    case Rock:
        return ROCK_TEX;
    case Snow:
        return SNOW_TEX;
    default:
        code_not_reached();
    } 
    return 0;    /* to suppress warnings */
} 



#define DRAW_POINT \
    glNormal3f( nml.x, nml.y, nml.z ); \
    glVertex3f( pt.x, pt.y, pt.z ); 

void render_course()
{
    set_gl_options( COURSE );

    setup_course_tex_gen();

    if ( light_course ) {
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        set_material( white, black, 1.0 );
        glEnable( GL_LIGHTING );

	setup_course_lighting();

        glShadeModel( GL_SMOOTH );
    } else {
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
        set_material( white, black, 0.0 );
        glShadeModel( GL_FLAT );
    } 

    update_course_quadtree( eye_pt, getparam_course_detail_level() );

    /* Locking arrays is only useful for multipass rendering
#if defined(HAVE_GLLOCKARRAYSEXT) && HAVE_GLLOCKARRAYSEXT
    glLockArraysEXT(0, nx*ny); 
#endif
    */

    render_course_quadtree( );

    /* 
#if defined(HAVE_GLLOCKARRAYSEXT) && HAVE_GLLOCKARRAYSEXT
    glUnlockArraysEXT();
#endif
    */
}

void draw_background(scalar_t fov, scalar_t aspect ) 
{
    scalar_t courseWidth, courseLength;
    scalar_t bgndWidth, bgndHeight;
    scalar_t miny;
    scalar_t x0, x1, z0;
    scalar_t total_length;
    GLuint   *tex_names;

    set_gl_options( BACKGROUND );

    get_course_dimensions( &courseWidth, &courseLength );
    miny = get_min_y_coord();
    tex_names = get_tex_names();

    total_length = ( 1.0 + FLAT_SEGMENT_FRACTION ) * courseLength;

    /* make background fill field of view */
    bgndWidth  = 2 * tan(fov/2.*M_PI/180.) * aspect * total_length;

    if ( bgndWidth < total_length ) {
	bgndWidth = total_length;
    }

    bgndHeight = bgndWidth / BACKGROUND_TEXTURE_ASPECT;

    if (bgndHeight < 1.5 * (-miny)) {
        bgndHeight =  1.5 * (-miny);
	bgndWidth = BACKGROUND_TEXTURE_ASPECT * bgndHeight;
    } 

    x0 = courseWidth / 2. - bgndWidth / 2.;
    x1 = courseWidth / 2. + bgndWidth / 2.;
    z0 = -total_length + bgndWidth;

    glBindTexture( GL_TEXTURE_2D, tex_names[BACKGROUND_TEX] );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );

    glBegin(GL_QUAD_STRIP);

    glTexCoord2f( 0.01, 0.99 );
    glVertex3f( x0, bgndHeight + miny, z0);
    glTexCoord2f( 0.01, 0.01 );
    glVertex3f( x0, miny, z0);

    glTexCoord2f( 0.99, 0.99 );
    glVertex3f( x0, bgndHeight + miny, -total_length);
    glTexCoord2f( 0.99, 0.01 );
    glVertex3f( x0, miny, -total_length);

    glTexCoord2f( 0.01, 0.99 );
    glVertex3f( x1, bgndHeight + miny, -total_length);
    glTexCoord2f( 0.01, 0.01 );
    glVertex3f( x1, miny, -total_length);

    glTexCoord2f( 0.99, 0.99 );
    glVertex3f( x1, bgndHeight + miny, z0);
    glTexCoord2f( 0.99, 0.01 );
    glVertex3f( x1, miny, z0);

    glTexCoord2f( 0.01, 0.99 );
    glVertex3f( x0, bgndHeight + miny, z0);
    glTexCoord2f( 0.01, 0.01 );
    glVertex3f( x0, miny, z0);

    glEnd();

    set_material( sky, black, 0.0 );
    glDisable( GL_TEXTURE_2D );

    glBegin( GL_QUADS );
    glVertex3f( x0, bgndHeight + miny, z0 );
    glVertex3f( x0, bgndHeight + miny, -total_length );
    glVertex3f( x1, bgndHeight + miny, -total_length );
    glVertex3f( x1, bgndHeight + miny, z0 );
    glEnd();

    /* Draw bottom rectangle, light it the same way as the course */
    if ( light_course ) {
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        set_material( white, black, 1.0 );
        glEnable( GL_LIGHTING );

	setup_course_lighting();

        glShadeModel( GL_SMOOTH );
    } else {
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
        set_material( white, black, 0.0 );
        glShadeModel( GL_FLAT );
    } 

    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, tex_names[SNOW_TEX] );
    glNormal3f( 0, 1., 0 );

    glBegin( GL_QUADS );

    glTexCoord2f( x0/TEX_SCALE, z0/TEX_SCALE );
    glVertex3f( x0, miny, z0 );

    glTexCoord2f( x1/TEX_SCALE, z0/TEX_SCALE );
    glVertex3f( x1, miny, z0 );

    glTexCoord2f( x1/TEX_SCALE, -total_length/TEX_SCALE );
    glVertex3f( x1, miny, -total_length );

    glTexCoord2f( x0/TEX_SCALE, -total_length/TEX_SCALE );
    glVertex3f( x0, miny, -total_length );

    glEnd();

} 

void draw_trees() 
{
    tree_t    *treeLocs;
    int       numTrees;
    scalar_t  treeRadius;
    scalar_t  treeHeight;
    int       i;
    GLuint    *tex_names;
    vector_t  normal;
    scalar_t  fwd_clip_limit, bwd_clip_limit, fwd_tree_detail_limit;

    treeLocs = get_tree_locs();
    numTrees = get_num_trees();
    tex_names = get_tex_names();

    fwd_clip_limit = getparam_forward_clip_distance();
    bwd_clip_limit = getparam_backward_clip_distance();
    fwd_tree_detail_limit = getparam_tree_detail_distance();

    set_gl_options( TREES );

    glBindTexture( GL_TEXTURE_2D, tex_names[TREE_TEX] );

    if ( light_course ) {
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        set_material( white, black, 1.0 );
        glEnable( GL_LIGHTING );

	setup_course_lighting();

        glShadeModel( GL_SMOOTH );
    } else {
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
        set_material( white, black, 0.0 );
        glShadeModel( GL_FLAT );
    } 

    for (i = 0; i< numTrees; i++ ) {

	if ( clip_course ) {
	    if ( eye_pt.z - treeLocs[i].ray.pt.z > fwd_clip_limit ) 
		continue;
	    
	    if ( treeLocs[i].ray.pt.z - eye_pt.z > bwd_clip_limit )
		continue;
	}
	
        glPushMatrix();
        glTranslatef( treeLocs[i].ray.pt.x, treeLocs[i].ray.pt.y, 
                      treeLocs[i].ray.pt.z );

        treeRadius = treeLocs[i].diam/2.;
        treeHeight = treeLocs[i].height;

	normal = subtract_points( eye_pt, treeLocs[i].ray.pt );
	normalize_vector( &normal );

	glNormal3f( normal.x, normal.y, normal.z );

        glBegin( GL_QUADS );
        glTexCoord2f( 0., 0. );
        glVertex3f( -treeRadius, 0.0, 0.0 );
        glTexCoord2f( 1., 0. );
        glVertex3f( treeRadius, 0.0, 0.0 );
        glTexCoord2f( 1., 1. );
        glVertex3f( treeRadius, treeHeight, 0.0 );
        glTexCoord2f( 0., 1. );
        glVertex3f( -treeRadius, treeHeight, 0.0 );

	if ( clip_course ) {
	    if ( eye_pt.z - treeLocs[i].ray.pt.z > fwd_tree_detail_limit ) 
		goto End_Quads;
	}

        glTexCoord2f( 0., 0. );
        glVertex3f( 0.0, 0.0, -treeRadius );
        glTexCoord2f( 1., 0. );
        glVertex3f( 0.0, 0.0, treeRadius );
        glTexCoord2f( 1., 1. );
        glVertex3f( 0.0, treeHeight, treeRadius );
        glTexCoord2f( 0., 1. );
        glVertex3f( 0.0, treeHeight, -treeRadius );

    End_Quads:
        glEnd();
        glPopMatrix();
    } 

} 
