/* 
 * Tux Racer 
 * Copyright (C) 1999-2001 Jasmin F. Patry
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

#ifndef _TUXRACER_H_
#define _TUXRACER_H_

#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#if defined ( __MWERKS__ ) || defined( _MSC_VER )
#   define NATIVE_WIN32_COMPILER 1
#else
/* Assume UNIX compatible by default */
#   define COMPILER_IS_UNIX_COMPATIBLE 1
#endif

#if defined( WIN32 ) || defined( __CYGWIN__ ) || \
    defined ( NATIVE_WIN32_COMPILER )
#  ifndef WIN32
#     define WIN32
#  endif
#  include <windows.h>
#endif 

#if defined( NATIVE_WIN32_COMPILER )
/* Need to manually define some things that autoconf defines for
   us in config.h */
#   define VERSION "0.61pre"
#   define HAVE_SDL 1
#   define HAVE_SDL_MIXER 1
#   define HAVE_SDL_JOYSTICKOPEN 1
#   define TCL_HEADER <tcl.h>

#   define HAVE__ISNAN

#   ifdef _MSC_VER
#       pragma warning (disable:4244) /* conversion from double to float */
#       pragma warning (disable:4305) /* truncation from const dbl to float */
#       pragma warning (disable:4761) /* integral size mismatch */
#   endif /* _MSC_VER */

#   ifdef __MWERKS__
        /* Codewarrior 4 seems to need this... */
        int _isnan( double x );
#   endif /* __MWERKS__ */
#endif

/* Include all (or most) system include files here.  This slows down
   compilation but makes maintenance easier since system-dependent
   #ifdefs are centralized. */
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>

/* Macros and include files for non-standard math routines */
#ifdef HAVE_IEEEFP_H
#   include <ieeefp.h>
#endif
#include <float.h>

/* System-dependent includes */
#if defined( NATIVE_WIN32_COMPILER )
#  include <io.h>
#  include <direct.h>
#endif

#if defined( COMPILER_IS_UNIX_COMPATIBLE )
#   include <unistd.h>
#   include <sys/types.h>
#   include <pwd.h>
#   include <dirent.h>
#   include <sys/time.h>
#   include <sys/types.h>
#   include <dirent.h>
#endif

/* OpenGL */
#include <GL/gl.h>
#include <GL/glu.h>

#ifdef HAVE_GL_GLX_H
#   include <GL/glx.h>
#endif

/* Tcl -- name of header is system-dependent :( */
#include TCL_HEADER

#ifndef M_PI
#   define M_PI 3.1415926535
#endif

#define EPS 1e-13

/* Some versions of sys/stat.h don't define S_ISDIR */
#ifndef S_ISDIR
#   define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif /* S_ISDIR */

/* Header files to include by default */
#include "winsys.h"
#include "string_util.h"
#include "file_util.h"
#include "game_config.h"
#include "tux_types.h"
#include "alglib.h"
#include "debug.h"
#include "error_util.h"

#define PROG_NAME "tuxracer"

#if defined( HAVE_FINITE )
#   define FINITE(x) (finite(x))
#elif defined( HAVE__FINITE )
#   define FINITE(x) (_finite(x))
#elif defined( HAVE_ISNAN )
#   define FINITE(x) (!isnan(x))
#elif defined( HAVE__ISNAN )
#   define FINITE(x) (!_isnan(x))
#else
#   error "You don't have finite(), _finite(), isnan(), or _isnan() on your system!"
#endif


/* Macros for swapping bytes */
#define SWAP_WORD(x) \
{ \
unsigned long tmp; \
tmp  = ((x) >> 24) & 0x000000ff; \
tmp |= ((x) >> 8)  & 0x0000ff00; \
tmp |= ((x) << 8)  & 0x00ff0000; \
tmp |= ((x) << 24) & 0xff000000; \
(x) = tmp; \
}

#define SWAP_SHORT(x) \
{ \
unsigned short tmp; \
tmp  = ((x) << 8)  & 0xff00; \
tmp |= ((x) >> 8)  & 0x00ff; \
(x) = tmp; \
}


/* define this to turn off all debugging assertions/checks */
/* #define TUXRACER_NO_ASSERT */

/* Directory separator */
#ifdef WIN32
#   define DIR_SEPARATOR "\\"
#else
#   define DIR_SEPARATOR "/"
#endif

#define BUFF_LEN 512

/* Multiplayer is not yet supported */
#define MAX_PLAYERS 1

/* Number of lives players get to complete a cup */
#define INIT_NUM_LIVES 4

#ifdef __cplusplus
extern "C"
{
#endif

/* Game state */
typedef enum {
    ALL_MODES = -2,
    NO_MODE = -1,
    SPLASH = 0,
    GAME_TYPE_SELECT,
    EVENT_SELECT,
    RACE_SELECT,
    LOADING,
    INTRO,
    RACING,
    GAME_OVER,
    PAUSED,
    RESET,
    CREDITS,
    NUM_GAME_MODES
} game_mode_t;

/* Difficulty levels */
typedef enum {
    DIFFICULTY_LEVEL_EASY,
    DIFFICULTY_LEVEL_NORMAL,
    DIFFICULTY_LEVEL_HARD,
    DIFFICULTY_LEVEL_INSANE,
    DIFFICULTY_NUM_LEVELS
} difficulty_level_t;

/* Race conditions */
typedef enum {
    RACE_CONDITIONS_SUNNY,
    RACE_CONDITIONS_CLOUDY,
    RACE_CONDITIONS_NIGHT,
    RACE_CONDITIONS_NUM_CONDITIONS
} race_conditions_t;

/* Race data */
typedef struct {
    char     *course;                               /* course directory */
    char     *name;                                 /* name of race */
    char     *description;                          /* description of course */
    int       herring_req[DIFFICULTY_NUM_LEVELS];   /* reqd number of herring */
    scalar_t  time_req[DIFFICULTY_NUM_LEVELS];      /* required time */
    int       score_req[DIFFICULTY_NUM_LEVELS];     /* required score */
    bool_t    mirrored;                             /* is course mirrored? */
    race_conditions_t conditions;                   /* race conditions */
    bool_t    windy;                                /* is it windy? */
    bool_t    snowing;                              /* is it snowing? */
} race_data_t;

/* Course data */
typedef struct {
    int num;                            /* current course number */
    char elev_file[BUFF_LEN];           /* elevation data file */
    char terrain_file[BUFF_LEN];        /* terrain data file */
    char tree_file[BUFF_LEN];           /* tree data file */
} course_data_t;

/* View mode */
typedef enum {
    BEHIND,
    FOLLOW,
    ABOVE,
    NUM_VIEW_MODES
} view_mode_t;

/* View point */
typedef struct {
    view_mode_t mode;                   /* View mode */
    point_t pos;                        /* position of camera */
    point_t plyr_pos;                   /* position of player */
    vector_t dir;                       /* viewing direction */
    vector_t up;                        /* up direction */
    matrixgl_t inv_view_mat;            /* inverse view matrix */
    bool_t initialized;                 /* has view been initialized? */
} view_t;

/* Control mode */
typedef enum {
    KEYBOARD = 0,
    MOUSE = 1,
    JOYSTICK = 2
} control_mode_t;

/* Control data */
typedef struct {
    control_mode_t mode;                /* control mode */
    scalar_t turn_fact;                 /* turning [-1,1] */
    scalar_t turn_animation;            /* animation step [-1,1] */
    bool_t is_braking;                  /* is player braking? */
    bool_t is_paddling;                 /* is player paddling? */
    scalar_t paddle_time;
    bool_t begin_jump;
    bool_t jumping;
    bool_t jump_charging;
    scalar_t jump_amt;
    scalar_t jump_start_time;
    bool_t barrel_roll_left;
    bool_t barrel_roll_right;
    scalar_t barrel_roll_factor;
    bool_t front_flip;
    bool_t back_flip;
    scalar_t flip_factor;
} control_t;

/* Player data */
typedef struct {
    char *name;                         /* name of player */
    int lives;                          /* number of lives left */
    point_t pos;                        /* current position */
    vector_t vel;                       /* current velocity */
    quaternion_t orientation;           /* current orientation */
    bool_t orientation_initialized;     /* is orientation initialized? */
    vector_t plane_nml;                 /* vector sticking out of
					   bellybutton (assuming on back) */
    vector_t direction;                 /* vector sticking out of feet */
    vector_t net_force;                 /* net force on player */
    vector_t normal_force;              /* terrain force on player */
    bool_t airborne;                    /* is plyr in the air? */
    bool_t collision;                   /* has plyr collided with obstacle? */
    control_t control;                  /* player control data */
    view_t view;                        /* player's view point */
    int herring;                        /* number of fish collected */
    int score;                          /* players' score */
} player_data_t;

/* All global data is stored in a variable of this type */
typedef struct {
    game_mode_t mode;                   /* game mode */
    game_mode_t prev_mode;              /* previous game mode */
    scalar_t time;                      /* game time */
    scalar_t time_step;                 /* size of current time step 
					   (i.e., time between frames) */
    int num_players;                    /* number of players */
    player_data_t player[MAX_PLAYERS];  /* player data */
    Tcl_Interp *tcl_interp;             /* instance of tcl interpreter */

    difficulty_level_t difficulty;      /* game difficulty */
    char *current_event;                /* name of current event */
    char *current_cup;                  /* name of current cup */
    int current_race;                   /* number of race in current cup */
    course_data_t course;               /* info about current course */
    race_data_t race;                   /* info about current race */
    int cup_races_won;                  /* how many races have been won in 
					   current cup? */
    bool_t practicing; 			/* are we in practice mode? */
    bool_t race_aborted;                /* was the race quit prematurely? */
    scalar_t secs_since_start;          /* seconds since game was started */
} game_data_t;

extern game_data_t g_game;

#define get_player_data( plyr ) ( & g_game.player[ (plyr) ] )

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

/* Emacs Customizations
;;; Local Variables: ***
;;; c-basic-offset:0 ***
;;; End: ***
*/

/* EOF */
