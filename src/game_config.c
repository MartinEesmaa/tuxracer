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

#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "tuxracer.h"

#define CONFIG_FILE ".tuxracer"

#ifndef DATA_DIR
#  define DATA_DIR "/usr/local/share/tuxracer"
#endif

/* Identifies the parameter type */
typedef enum {
    PARAM_STRING,
    PARAM_CHAR,
    PARAM_INT,
    PARAM_BOOL
} param_type;

/* Stores the value for all types */
typedef union {
    char* string_val;
    char  char_val;
    int   int_val;
    bool_t bool_val;
} param_val;

/* Stores state for each parameter */
struct param {
    int loaded;
    char *name;
    param_type type;
    param_val val;
    param_val deflt;
};

/*
 * These macros are used to initialize parameter values
 */

#define INIT_PARAM( nam, val, typename ) \
   Params. ## nam ## .loaded = False; \
   Params. ## nam ## .name = #nam; \
   Params. ## nam ## .deflt. ## typename ## _val  = val;

#define INIT_PARAM_STRING( nam, val ) \
   INIT_PARAM( nam, val, string ); \
   Params. ## nam ## .type = PARAM_STRING;

#define INIT_PARAM_CHAR( nam, val ) \
   INIT_PARAM( nam, val, char ); \
   Params. ## nam ## .type = PARAM_CHAR;

#define INIT_PARAM_INT( nam, val ) \
   INIT_PARAM( nam, val, int ); \
   Params. ## nam ## .type = PARAM_INT;

#define INIT_PARAM_BOOL( nam, val ) \
   INIT_PARAM( nam, val, bool ); \
   Params. ## nam ## .type = PARAM_BOOL;


/*
 * These functions are used to get and set parameter values
 */

void fetch_param_string( struct param *p )
{
    char *val;
    val = Tcl_GetVar( g_game.tcl_interp, p->name, TCL_GLOBAL_ONLY );
    if ( val == NULL ) {
	p->val.string_val = string_copy( p->deflt.string_val );
    } else {
	p->val.string_val = string_copy( val );
    }
    p->loaded = True;

}

void set_param_string( struct param *p, char *new_val )
{
    char *ret;
    if ( p->loaded ) {
	free( p->val.string_val );
    }
    ret = Tcl_SetVar( g_game.tcl_interp, p->name, new_val, TCL_GLOBAL_ONLY );
    if ( ret == NULL ) {
	p->val.string_val = string_copy( p->deflt.string_val );
    } else {
	p->val.string_val = string_copy( new_val );
    }
    p->loaded = True;

}

void fetch_param_char( struct param *p )
{
    char *str_val;
    str_val = Tcl_GetVar( g_game.tcl_interp, p->name, TCL_GLOBAL_ONLY );
    
    if ( str_val == NULL || str_val[0] == '\0' ) {
	p->val.char_val = p->deflt.char_val;
    } else {
	p->val.char_val = str_val[0];
    }
    p->loaded = True;
}

void set_param_char( struct param *p, char new_val )
{
    char buff[2];
    char *ret;

    buff[0] = new_val;
    buff[1] = '\0';

    ret = Tcl_SetVar( g_game.tcl_interp, p->name, buff, TCL_GLOBAL_ONLY );
    if ( ret == NULL ) {
	p->val.char_val = p->deflt.char_val;
    } else {
	p->val.char_val = new_val;
    }
    p->loaded = True;

}

void fetch_param_int( struct param *p )
{
    char *str_val;
    int val;
    str_val = Tcl_GetVar( g_game.tcl_interp, p->name, TCL_GLOBAL_ONLY );
    
    if ( str_val == NULL 
	 || Tcl_GetInt( g_game.tcl_interp, str_val, &val) == TCL_ERROR  ) 
    {
	p->val.int_val = p->deflt.int_val;
    } else {
	p->val.int_val = val;
    }
    p->loaded = True;
}

void set_param_int( struct param *p, int new_val )
{
    char buff[30];
    char *ret;

    sprintf( buff, "%d", new_val );

    ret = Tcl_SetVar( g_game.tcl_interp, p->name, buff, TCL_GLOBAL_ONLY );
    if ( ret == NULL ) {
	p->val.int_val = p->deflt.int_val;
    } else {
	p->val.int_val = new_val;
    }
    p->loaded = True;

}

void fetch_param_bool( struct param *p )
{
    char *str_val;
    int val;
    bool_t no_val = False;

    str_val = Tcl_GetVar( g_game.tcl_interp, p->name, TCL_GLOBAL_ONLY );
    
    if ( str_val == NULL ) {
	no_val = True;
    } else if ( string_cmp_no_case( str_val, "false" ) == 0 ) {
	p->val.bool_val = False;
    } else if ( string_cmp_no_case( str_val, "true" ) == 0 ) {
	p->val.bool_val = True;
    } else if ( Tcl_GetInt( g_game.tcl_interp, str_val, &val) == TCL_ERROR ) {
	no_val = True;
    } else {
	p->val.bool_val = (val == 0) ? False : True ;
    }

    if ( no_val ) {
	p->val.bool_val = p->deflt.bool_val;
    }

    p->loaded = True;
}

void set_param_bool( struct param *p, bool_t new_val )
{
    char buff[2];
    char *ret;

    sprintf( buff, "%d", new_val ? 1 : 0 );

    ret = Tcl_SetVar( g_game.tcl_interp, p->name, buff, TCL_GLOBAL_ONLY );
    if ( ret == NULL ) {
	p->val.bool_val = p->deflt.bool_val;
    } else {
	p->val.bool_val = new_val;
    }
    p->loaded = True;
}


/*
 * Creates set/get functions for each parameter
 */
#define FN_PARAM( name, typename, type ) \
    type getparam_ ## name() { \
        if ( !Params. ## name ## .loaded ) { \
            fetch_param_ ## typename( &( Params. ## name ) ); \
        } \
        return Params. ## name ## .val. ## typename ## _val; \
    } \
    void setparam_ ## name( type val) { \
        set_param_ ## typename( &( Params. ## name ), val ); } 

#define FN_PARAM_STRING( name ) \
    FN_PARAM( name, string, char* )

#define FN_PARAM_CHAR( name ) \
    FN_PARAM( name, char, char )

#define FN_PARAM_INT( name ) \
    FN_PARAM( name, int, int )

#define FN_PARAM_BOOL( name ) \
    FN_PARAM( name, bool, bool_t )


/*
 * Main parameter struct
 */
struct params {
    struct param data_dir;
    struct param fullscreen;
    struct param x_resolution;
    struct param y_resolution;
    struct param force_window_position;
    struct param warp_pointer;
    struct param control_mode; /* 0 = keyboard, 
				  1 = mouse,
				  2 = joystick
			       */
    struct param quit_key;
    struct param turn_left_key;
    struct param turn_right_key;
    struct param brake_key;
    struct param above_view_key;
    struct param behind_view_key;
    struct param eye_view_key;
    struct param screenshot_key;
    struct param display_fps;
    struct param draw_tux_shadow;
    struct param tux_sphere_divisions;
    struct param tux_shadow_sphere_divisions;
    struct param draw_particles;
    struct param draw_particle_shadows;
    struct param nice_fog;
    struct param compile_course;
    struct param use_sphere_display_list;
    struct param do_intro_animation;
    struct param mipmap_type; /* 0 = GL_NEAREST,
				 1 = GL_LINEAR,
				 2 = GL_NEAREST_MIPMAP_NEAREST,
				 3 = GL_LINEAR_MIPMAP_NEAREST,
				 4 = GL_NEAREST_MIPMAP_LINEAR,
				 5 = GL_LINEAR_MIPMAP_LINEAR
			      */
    struct param ode_solver; /* 0 = Euler,
				1 = ODE23,
				2 = ODE45
			     */
    struct param fov; 
    struct param debug; 
    struct param warning_level; 
};

static struct params Params;


/*
 * Initialize parameter data
 */

void init_game_configuration()
{
    INIT_PARAM_STRING( data_dir, DATA_DIR );
    INIT_PARAM_BOOL( draw_tux_shadow, False );
    INIT_PARAM_BOOL( draw_particles, True );
    INIT_PARAM_BOOL( draw_particle_shadows, False );
    INIT_PARAM_INT( tux_sphere_divisions, 6 );
    INIT_PARAM_INT( tux_shadow_sphere_divisions, 3 );
    INIT_PARAM_BOOL( nice_fog, True );
    INIT_PARAM_BOOL( compile_course, False );
    INIT_PARAM_BOOL( use_sphere_display_list, True );
    INIT_PARAM_BOOL( display_fps, False );
    INIT_PARAM_INT( x_resolution, 640 );
    INIT_PARAM_INT( y_resolution, 480 );
    INIT_PARAM_BOOL( do_intro_animation, True );
    INIT_PARAM_INT( mipmap_type, 3 );
    INIT_PARAM_BOOL( fullscreen, False );
    INIT_PARAM_BOOL( force_window_position, True );
    INIT_PARAM_BOOL( warp_pointer, True );
    INIT_PARAM_INT( ode_solver, 1 );
    INIT_PARAM_INT( control_mode, 0 );
    INIT_PARAM_STRING( quit_key, "q escape" );
    INIT_PARAM_STRING( turn_left_key, "j" );
    INIT_PARAM_STRING( turn_right_key, "l" );
    INIT_PARAM_STRING( brake_key, "space" );
    INIT_PARAM_STRING( above_view_key, "1" );
    INIT_PARAM_STRING( behind_view_key, "2" );
    INIT_PARAM_STRING( eye_view_key, "3" );
    INIT_PARAM_STRING( screenshot_key, "=" );
    INIT_PARAM_INT( fov, 60 );
    INIT_PARAM_STRING( debug, "" );
    INIT_PARAM_INT( warning_level, 100 );
}


/* 
 * Create the set/get functions for parameters
 */

FN_PARAM_STRING( data_dir )
FN_PARAM_BOOL( draw_tux_shadow )
FN_PARAM_BOOL( draw_particles )
FN_PARAM_BOOL( draw_particle_shadows )
FN_PARAM_INT( tux_sphere_divisions )
FN_PARAM_INT( tux_shadow_sphere_divisions )
FN_PARAM_BOOL( nice_fog )
FN_PARAM_BOOL( compile_course )
FN_PARAM_BOOL( use_sphere_display_list )
FN_PARAM_BOOL( display_fps )
FN_PARAM_INT( x_resolution )
FN_PARAM_INT( y_resolution )
FN_PARAM_BOOL( do_intro_animation )
FN_PARAM_INT( mipmap_type )
FN_PARAM_BOOL( fullscreen )
FN_PARAM_BOOL( force_window_position )
FN_PARAM_BOOL( warp_pointer )
FN_PARAM_INT( ode_solver )
FN_PARAM_STRING( quit_key )
FN_PARAM_STRING( turn_left_key )
FN_PARAM_STRING( turn_right_key )
FN_PARAM_STRING( brake_key )
FN_PARAM_STRING( above_view_key )
FN_PARAM_STRING( behind_view_key )
FN_PARAM_STRING( eye_view_key )
FN_PARAM_STRING( screenshot_key )
FN_PARAM_INT( fov )
FN_PARAM_STRING( debug )
FN_PARAM_INT( warning_level )


/*
 * Functions to read and write the configuration file
 */

int get_config_file_name( char *buff, int len )
{
    struct passwd *pwent;

    pwent = getpwuid( getuid() );
    if ( pwent == NULL ) {
	perror( "getpwuid" );
	return 1;
    }

    if ( strlen( pwent->pw_dir ) + strlen( CONFIG_FILE ) + 2 > len ) {
	return 1;
    }

    sprintf( buff, "%s/%s", pwent->pw_dir, CONFIG_FILE );
    return 0;
}

void clear_config_cache()
{
    struct param *parm;
    int i;

    for (i=0; i<sizeof(Params)/sizeof(struct param); i++) {
	parm = (struct param*)&Params + i;
	parm->loaded = False;
    }
}

void read_config_file()
{
    char config_file[BUFF_LEN];
    struct stat config_stat;

    clear_config_cache();

    if ( get_config_file_name( config_file, sizeof( config_file ) ) != 0 )
	return;

    if ( stat( config_file, &config_stat ) != 0 ) {
	if ( errno != ENOENT ) 
	    perror( "stat" );
	return;
    }

    /* File is there, so let's try to evaluate it. */
    if ( Tcl_EvalFile( g_game.tcl_interp, config_file ) != TCL_OK ) {
        fprintf( stderr, "Error evalating %s\n", config_file );
	exit( 1 );
    }
}

void write_config_file()
{
    FILE *config_stream;
    char config_file[BUFF_LEN];
    struct param *parm;
    int i;

    if ( get_config_file_name( config_file, sizeof( config_file ) ) != 0 ) {
	return;
    }

    config_stream = fopen( config_file, "w" );

    if ( config_stream == NULL ) {
	print_warning( CRITICAL_WARNING, 
		       "couldn't open %s for writing: %s", 
		       config_file, strerror(errno) );
	return;
    }

    for (i=0; i<sizeof(Params)/sizeof(struct param); i++) {
	parm = (struct param*)&Params + i;
	switch ( parm->type ) {
	case PARAM_STRING:
	    fetch_param_string( parm );
	    fprintf( config_stream, "set %s \"%s\"\n",
		     parm->name, parm->val.string_val );
	    break;
	case PARAM_CHAR:
	    fetch_param_char( parm );
	    fprintf( config_stream, "set %s %c\n",
		     parm->name, parm->val.char_val );
	    break;
	case PARAM_INT:
	    fetch_param_int( parm );
	    fprintf( config_stream, "set %s %d\n",
		     parm->name, parm->val.int_val );
	    break;
	case PARAM_BOOL:
	    fetch_param_bool( parm );
	    fprintf( config_stream, "set %s %s\n",
		     parm->name, parm->val.bool_val ? "true" : "false" );
	    break;
	default:
	    code_not_reached();
	}
    }

    if ( fclose( config_stream ) != 0 ) {
	perror( "fclose" );
    }
}
