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


#include <sys/stat.h>
#include "tuxracer.h"
#include "course_load.h"
#include "course_render.h"
#include "image.h"
#include "textures.h"
#include "phys_sim.h"
#include "tcl_util.h"
#include "keyframe.h"

#define MAX_TREES 256

#define MIN_ANGLE 5
#define MAX_ANGLE 80

#define ICE_IMG_VAL  0
#define ROCK_IMG_VAL 127
#define SNOW_IMG_VAL 255
#define TREE_IMG_THRESHOLD 128
#define NUM_TERRAIN_TYPES 3

static bool_t        courseLoaded = False;

static int           courseNumber;
static scalar_t     *elevation;
static scalar_t      elevScale;
static scalar_t      courseWidth, courseLength;
static scalar_t      courseAngle;
static int           nx, ny;
static tree_t        treeLocs[MAX_TREES];
static int           numtrees;
static scalar_t      treeDiam;
static scalar_t      treeHeight;
static polyhedron_t  treePolyhedron;
static terrain_t    *terrain;
static point2d_t     startPt;

scalar_t     *get_course_elev_data()    { return elevation; }
terrain_t    *get_course_terrain_data() { return terrain; }
scalar_t      get_course_angle()        { return courseAngle; } 
tree_t       *get_tree_locs()           { return treeLocs; }
int           get_num_trees()           { return numtrees; }
scalar_t      get_tree_diam()           { return treeDiam; }
scalar_t      get_tree_height()         { return treeHeight; }
polyhedron_t  get_tree_polyhedron()     { return treePolyhedron; }
point2d_t     get_start_pt()            { return startPt; }

void get_course_dimensions( scalar_t *width, scalar_t *length )
{
    *width = courseWidth;
    *length = courseLength;
} 

void get_course_divisions( int *x, int *y )
{
    *x = nx;
    *y = ny;
} 

static void reset_course()
{
    /*
     * Defaults
     */
    courseNumber = -1;
    numtrees     = 0;
    courseAngle  = 20.;
    courseWidth  = 50.;
    courseLength = 130.;
    treeDiam     = 2.0;
    treeHeight   = 4.5;
    treePolyhedron.num_polygons = 0;
    treePolyhedron.num_vertices = 0;
    nx = ny = -1;

    if ( courseLoaded == False ) return;

    free( elevation );
    free( terrain );
    free( treePolyhedron.polygons );
    free( treePolyhedron.vertices );

    courseLoaded = False;

    reset_course_list();
    reset_key_frame();
} 

bool_t course_exists( int num )
{
    char buff[BUFF_LEN];
    struct stat s;

    sprintf( buff, "%s/courses/%d", get_data_dir(), num );
    if ( stat( buff, &s ) != 0 ) {
	return False;
    }
    if ( ! S_ISDIR( s.st_mode) ) {
	return False;
    }
    return True;
}

void select_course( int num ) 
{
    char buff[BUFF_LEN];
    char cwd[BUFF_LEN];

    assert( 1 <= num );

    reset_course();

    courseNumber = num;

    if ( getcwd( cwd, BUFF_LEN ) == NULL ) {
	perror( "getcwd" );
	exit(-1);
    }

    sprintf( buff, "%s/courses/%d", get_data_dir(), num );
    if ( chdir( buff ) != 0 ) {
        fprintf( stderr, "Chdir to %s:\n", buff );
        perror("chdir");
        exit(-1);
    } 

    if ( Tcl_EvalFile( g_game.tcl_interp, "./course.tcl") == TCL_ERROR ) {
        fprintf( stderr, "Error evalating %s/course.tcl: %s\n", 
		 buff, g_game.tcl_interp->result );
        exit(-1);
    } 

    if ( chdir( cwd ) != 0 ) {
        perror("chdir");
        exit(-1);
    } 

    assert( !Tcl_InterpDeleted( g_game.tcl_interp ) );

    calc_normals();

    courseLoaded = True;
} 

static terrain_t intensity_to_terrain( int intensity )
{
    int dist[NUM_TERRAIN_TYPES];
    int min_idx, min;
    int i;
    
    dist[Ice]  = abs( intensity - ICE_IMG_VAL );
    dist[Snow] = abs( intensity - SNOW_IMG_VAL );
    dist[Rock] = abs( intensity - ROCK_IMG_VAL );

    min = dist[0];
    min_idx = 0;
    for (i=1; i<NUM_TERRAIN_TYPES; i++) {
	if ( dist[i] < min ) {
	    min = dist[i];
	    min_idx = i;
	}
    }
    return min_idx;
}

static int course_dim_cb ( ClientData cd, Tcl_Interp *ip, 
			   int argc, char *argv[]) 
{
    double width, length;

    if ( argc != 3 ) {
        fprintf( stderr, "Usage: %s <course width> <course length>", argv[0] );
        return TCL_ERROR;
    } 

    if ( Tcl_GetDouble( ip, argv[1], &width ) != TCL_OK ) {
        return TCL_ERROR;
    } 
    if ( Tcl_GetDouble( ip, argv[2], &length ) != TCL_OK ) {
        return TCL_ERROR;
    } 

    courseWidth = width;
    courseLength = length;

    return TCL_OK;
} 

static int angle_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{
    double angle;

    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s <angle>", argv[0] );
        return TCL_ERROR;
    } 

    if ( Tcl_GetDouble( ip, argv[1], &angle ) != TCL_OK ) {
        return TCL_ERROR;
    } 

    assert( MIN_ANGLE <= angle && angle <= MAX_ANGLE );

    courseAngle = angle;

    return TCL_OK;
} 


static int elev_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{
    IMAGE *elevImg;
    scalar_t slope;
    int   x,y;
    int   pad;

    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s <elevation file>.rgb\n", argv[0] );
        return TCL_ERROR;
    } 

    assert( !courseLoaded );

    elevImg = ImageLoad( argv[1] );
    assert( elevImg != NULL );

    nx = elevImg->sizeX;
    ny = elevImg->sizeY;

    elevation = (scalar_t *)malloc( sizeof(scalar_t)*nx*ny );
    slope = tan( courseAngle * M_PI/180.0 );

    pad = 0;    /* RGBA images rows are aligned on 4-byte boundaries */
    for (y=0; y<ny; y++) {
        for (x=0; x<nx; x++) {
            ELEV(nx-1-x, ny-1-y) = 
		( elevImg->data[ (x + nx * y) * elevImg->sizeZ + pad ] 
		  / 255.0 - 0.5 ) * elevScale
		- (scalar_t) (ny-1.-y)/ny * courseLength * slope;

            if ( x == 0 || x == nx-1 ) {
                ELEV(nx-1-x, ny-1-y) += 0.5 * elevScale;
            } 
        } 
        pad += (nx*elevImg->sizeZ) % 4;
    } 

    free( elevImg->data );
    free( elevImg );

    return TCL_OK;
} 

static int terrain_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{
    IMAGE *terrainImg;
    int   x,y;
    int   pad;
    int   idx;

    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s <terrain data>.rgb", argv[0] );
        return TCL_ERROR;
    } 

    terrainImg = ImageLoad( argv[1] );
    assert( terrainImg != NULL );

    assert( nx == terrainImg->sizeX );
    assert( ny == terrainImg->sizeY );

    terrain = (terrain_t *)malloc( sizeof(terrain_t)*nx*ny );

    pad = 0;
    for (y=0; y<ny; y++) {
        for (x=0; x<nx; x++) {
            idx = (nx-1-x) + nx*(ny-1-y);
	    terrain[idx] = intensity_to_terrain(
		terrainImg->data[(x+nx*y)*terrainImg->sizeZ+pad] );
        } 
        pad += (nx*terrainImg->sizeZ) % 4;
    } 

    free( terrainImg->data );
    free( terrainImg );

    return TCL_OK;
} 

static int bgnd_img_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s <bgnd image>.rgb", argv[0] );
        return TCL_ERROR;
    } 

    load_texture( BACKGROUND_TEX, argv[1] );

    return TCL_OK;
} 

static int tree_tex_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s <tree texture>.rgb", argv[0] );
        return TCL_ERROR;
    } 

    load_texture( TREE_TEX, argv[1] );

    return TCL_OK;
} 

static int ice_tex_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s <ice texture>.rgb", argv[0] );
        return TCL_ERROR;
    } 

    load_texture( ICE_TEX, argv[1] );

    return TCL_OK;
} 

static int rock_tex_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s <rock texture>.rgb", argv[0] );
        return TCL_ERROR;
    } 

    load_texture( ROCK_TEX, argv[1] );

    return TCL_OK;
} 

static int snow_tex_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{

    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s <snow texture>.rgb", argv[0] );
        return TCL_ERROR;
    } 

    load_texture( SNOW_TEX, argv[1] );

    return TCL_OK;
} 

static int start_pt_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{
    double xcd, ycd;

    if ( argc != 3 ) {
        fprintf( stderr, "Usage: %s <x coord> <y coord>", argv[0] );
        return TCL_ERROR;
    } 

    if ( Tcl_GetDouble( ip, argv[1], &xcd ) != TCL_OK ) {
        return TCL_ERROR;
    } 
    if ( Tcl_GetDouble( ip, argv[2], &ycd ) != TCL_OK ) {
        return TCL_ERROR;
    } 

    assert( xcd > 0 && xcd < courseWidth );
    assert( ycd > 0 && ycd < courseLength );

    startPt.x = xcd;
    startPt.y = -ycd;

    return TCL_OK;
} 

static int elev_scale_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{

    double scale;
    
    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s <scale>", argv[0] );
        return TCL_ERROR;
    } 

    if ( Tcl_GetDouble( ip, argv[1], &scale ) != TCL_OK ) {
        return TCL_ERROR;
    } 

    assert( scale > 0 );

    elevScale = scale;

    return TCL_OK;
} 

static bool_t is_tree( unsigned char pixel[] )
{
    return pixel[0] + pixel[1] + pixel[2] > TREE_IMG_THRESHOLD;
}

static int trees_cb ( ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{
    IMAGE *treeImg;
    int sx, sy, sz;
    int x,y;
    int pad;

    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s <tree loc>.rgb", argv[0] );
        return TCL_ERROR;
    } 

    treeImg = ImageLoad( argv[1] );
    assert( treeImg != NULL );

    sx = treeImg->sizeX;
    sy = treeImg->sizeY;
    sz = treeImg->sizeZ;

    numtrees = 0;
    pad = 0;
    for (y=0; y<sy; y++) {
        for (x=0; x<sx; x++) {
            if ( is_tree ( & treeImg->data[ (x + y*sx)*sz + pad ] ) ) {
                if (numtrees == MAX_TREES ) {
                    fprintf( stderr, "%s: maximum number of trees reached.\n", 
			     argv[0] );
                    break;
                }

                treeLocs[numtrees].ray.pt.x = 
		    (sx-x)/(scalar_t)(sx-1.)*courseWidth;

                treeLocs[numtrees].ray.pt.z = 
		    -(sy-y)/(scalar_t)(sy-1.)*courseLength;

                treeLocs[numtrees].ray.pt.y = 
		    find_y_coord( treeLocs[numtrees].ray.pt.x,
				treeLocs[numtrees].ray.pt.z );

                treeLocs[numtrees].ray.vec = make_vector( 0, 1, 0);
                treeLocs[numtrees].height = ((scalar_t)rand()/RAND_MAX+0.5)* treeHeight;
                treeLocs[numtrees].diam = treeLocs[numtrees].height/treeHeight 
                                          * treeDiam;
                numtrees++;
            } 
        } 
        pad += ( sx * sz ) % 4; /* to compensate for word-aligned rows */
    } 

    free( treeImg->data );
    free( treeImg );

    return TCL_OK;
} 

static int tree_size_cb( ClientData cd, Tcl_Interp *ip, 
			 int argc, char *argv[]) 
{
    double diam, height;

    if ( argc != 3 ) {
        fprintf( stderr, "Usage: %s <diam.> <height>", argv[0] );
        return TCL_ERROR;
    } 

    if ( Tcl_GetDouble( ip, argv[1], &diam ) != TCL_OK ) {
        return TCL_ERROR;
    } 
    if ( Tcl_GetDouble( ip, argv[2], &height ) != TCL_OK ) {
        return TCL_ERROR;
    } 

    treeDiam   = diam;
    treeHeight = height;

    return TCL_OK;
} 

/*
 * The following functions are used to get the parameters for 
 * tux_tree_poly
 */

/*
 * Gets the vertex list
 */
static int
get_polyhedron_vertices(Tcl_Interp *ip, char *arg, 
			int *num_vertices, point_t **vertex_list)
{
  int i;
  char **indices;
  int rtn;
  scalar_t pt[3];
  
  rtn = Tcl_SplitList(ip, arg, num_vertices, &indices);
  if( rtn != TCL_OK )
  {
    Tcl_AppendResult(ip, 
		     "A vertex list must be provided",
		     (char *) 0);
    free(indices);
    return (TCL_ERROR);
  }

  *vertex_list = (point_t*)malloc(sizeof(point_t)*(*num_vertices));
  for(i = 0; i < *num_vertices; i++) {
    get_tcl_tuple(ip, indices[i], pt, 3);
    (*vertex_list)[i] = make_point_from_array(pt);
  }

  return TCL_OK;
}

/*
 * gets the vertex-index-list of a single polygon
 */
static int
get_single_polygon(Tcl_Interp *ip, char *arg, 
		   polygon_t *polygon)
{
  int i;
  char **indices;
  int rtn;
  int num_vertices;
  
  rtn = Tcl_SplitList(ip, arg, &num_vertices, &indices);
  if( rtn != TCL_OK )
  {
    Tcl_AppendResult(ip, 
		     "a list of vertices must be provide for each polygon",
		     (char *) 0);
    free(indices);
    return (TCL_ERROR);
  }
  
  polygon->num_vertices = num_vertices;
  polygon->vertices = (int*)malloc(sizeof(int)*num_vertices);
  
  for(i = 0; i < num_vertices; i++)
    Tcl_GetInt(ip, indices[i], &(polygon->vertices[i]));

  return TCL_OK;
}

/*
 * gets the polygon list.
 */
static int
get_polyhedron_polygon(Tcl_Interp *ip, char *arg, 
		       int *num_polygons, polygon_t **polygon_list)
{
  int i;
  char **indices;
  int rtn;

  rtn = Tcl_SplitList(ip, arg, num_polygons, &indices);
  if( rtn != TCL_OK )
  {
    Tcl_AppendResult(ip, 
		     "A polygon list must be provided",
		     (char *) 0);
    free(indices);
    return TCL_ERROR;
  }

  *polygon_list = (polygon_t*)malloc(sizeof(polygon_t)*(*num_polygons));
  for(i = 0; i < *num_polygons; i++)
    get_single_polygon(ip, indices[i], &((*polygon_list)[i]));

  return TCL_OK;
}

/*
 * finally the polyhedron object.
 */
static int 
tree_poly_cb(ClientData cd, Tcl_Interp *ip, int argc, char *argv[]) 
{
  int           num_vertices;
  point_t     *vertices;
  int           num_polygons;
  polygon_t     *polygons;
    
  if (argc != 3) {
    Tcl_AppendResult(ip, 
		     argv[0], 
		     " needs 2 arguments\n"
		     " Usage: tux_tree_poly <vertex list> <face list>"
		     " <polygon list>",
		     (char *) 0 );
    return TCL_ERROR;
  }

  if( get_polyhedron_vertices(ip, argv[1], &num_vertices, &vertices) == TCL_OK )
    if( get_polyhedron_polygon(ip, argv[2], &num_polygons, &polygons) == TCL_OK ) {
        treePolyhedron.num_polygons = num_polygons;
        treePolyhedron.polygons     = polygons;
        treePolyhedron.num_vertices = num_vertices;
        treePolyhedron.vertices     = vertices;
        return TCL_OK;
    }

  return TCL_ERROR;
}

static int friction_cb ( ClientData cd, Tcl_Interp *ip, 
			 int argc, char *argv[]) 
{
    double fric[3];
    scalar_t fric_s[3];
    int i;

    if ( argc != 4 ) {
        fprintf( stderr, "Usage: %s <ice> <rock> <snow>", argv[0] );
        return TCL_ERROR;
    } 

    if ( Tcl_GetDouble( ip, argv[1], &fric[0] ) != TCL_OK ) {
        return TCL_ERROR;
    } 

    if ( Tcl_GetDouble( ip, argv[2], &fric[1] ) != TCL_OK ) {
        return TCL_ERROR;
    } 

    if ( Tcl_GetDouble( ip, argv[3], &fric[2] ) != TCL_OK ) {
        return TCL_ERROR;
    }

    for ( i=0; i<sizeof(fric)/sizeof(fric[0]); i++) {
	fric_s[i] = fric[i];
    }

    set_friction_coeff( fric_s );

    return TCL_OK;
} 

void register_course_load_tcl_callbacks( Tcl_Interp *ip )
{
    Tcl_CreateCommand (ip, "tux_course_dim", course_dim_cb,  0,0);
    Tcl_CreateCommand (ip, "tux_angle",      angle_cb,  0,0);
    Tcl_CreateCommand (ip, "tux_elev_scale", elev_scale_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_elev",       elev_cb,        0,0);
    Tcl_CreateCommand (ip, "tux_terrain",    terrain_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_trees",      trees_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_tree_size",  tree_size_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_tree_poly",  tree_poly_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_bgnd_img",   bgnd_img_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_tree_tex",   tree_tex_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_ice_tex",    ice_tex_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_rock_tex",   rock_tex_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_snow_tex",   snow_tex_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_start_pt",   start_pt_cb,   0,0);
    Tcl_CreateCommand (ip, "tux_friction",   friction_cb,   0,0);
}

