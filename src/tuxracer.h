
#ifndef _TUXRACER_H_
#define _TUXRACER_H_

#include "config.h"

#include <GL/gl.h>
#include <GL/glut.h>
#include <tcl.h>

#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <sys/time.h>
#include <string.h>
#include <limits.h>

#include "string_util.h"
#include "game_config.h"

#ifndef M_PI
#define M_PI 3.1415926535
#endif

#define EPS 1e-13

#include "tux_types.h"
#include "alglib.h"

#define DEBUG 1

#define BUFF_LEN 512

/* Multiplayer is not yet supported */
#define MAX_PLAYERS 1


/* Game state */
typedef enum {
    NO_MODE = -1,
    START = 0,
    INTRO = 1,
    RACING = 2,
    GAME_OVER = 3,
    NUM_GAME_MODES = 4
} game_mode_t;

/* Course data */
typedef struct {
    int num;                            /* current course number */
    char elev_file[BUFF_LEN];           /* elevation data file */
    char terrain_file[BUFF_LEN];        /* terrain data file */
    char tree_file[BUFF_LEN];           /* tree data file */
} course_data_t;

/* View mode */
typedef enum {
    PENGUIN_EYE,
    BEHIND,
    ABOVE
} view_mode_t;

/* View point */
typedef struct {
    view_mode_t mode;                   /* View mode */
    point_t pos;                        /* position of camera */
    vector_t dir;                       /* viewing direction */
    vector_t up;                        /* up direction */
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
    scalar_t turn_fact;                 /* amount turning [-1,1] */
    bool_t is_braking;                  /* is player braking? */
} control_t;

/* Player data */
typedef struct {
    point_t pos;                        /* current position */
    vector_t vel;                       /* current velocity */
    quaternion_t orientation;           /* current orientation */
    bool_t orientation_initialized;     /* is orientation initialized? */
    vector_t plane_nml;                 /* vector sticking out of
					   bellybutton */
    vector_t direction;                 /* vector sticking out of feet */
    vector_t net_force;                 /* net force on player */
    control_t control;                  /* player control data */
    view_t view;                        /* player's view point */
} player_data_t;

/* All global data is stored in a variable of this type */
typedef struct {
    game_mode_t mode;                   /* game mode */
    scalar_t time;                      /* game time */
    scalar_t time_step;                 /* size of current time step 
					   (i.e., time between frames) */
    course_data_t course;               /* course data */
    int num_players;                    /* number of players */
    player_data_t player[MAX_PLAYERS];  /* player data */
    Tcl_Interp *tcl_interp;             /* instance of tcl interpreter */
} game_data_t;

extern game_data_t g_game;

#define get_player_data( plyr ) ( & g_game.player[ (plyr) ] )

#endif
