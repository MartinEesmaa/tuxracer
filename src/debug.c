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

#include <string.h>
#include <stdarg.h>
#include "debug.h"
#include "tuxracer.h"
#include "string_util.h"

static bool_t debug_setting[ NUM_DEBUG_MODES ];
static char* debug_desc[ NUM_DEBUG_MODES ] = {
    "ode"
};

/* Parse the debug parameter, fill in the debug_setting array */
void init_debug()
{

#ifndef TUXRACER_NO_DEBUG

    char *debug_str, *tmp_str;
    char *p;
    int i;
    bool_t new_setting;

    for ( i=0; i<NUM_DEBUG_MODES; i++ ) {
	debug_setting[i] = False;
    }

    debug_str = getparam_debug();
    tmp_str = debug_str;

    while ( (p = strtok( tmp_str, " " )) != NULL ) {
	tmp_str = NULL;

	new_setting = True;

	if ( *p == '-' ) {
	    p++;
	    new_setting = False;

	    if ( *p == '\0' ) {
		print_warning( CONFIGURATION_WARNING, 
			       "solitary `-' in debug parameter -- ignored." );
		continue;
	    }
	}

	if ( *p == '\0' ) {
	    continue;
	}

	for ( i=0; i<NUM_DEBUG_MODES; i++ ) {
	    if ( string_cmp_no_case( p, debug_desc[i] ) == 0 ||
		 string_cmp_no_case( p, "all" ) == 0 ) {
		debug_setting[i] = new_setting;
		break;
	    }
	}

	if ( i == NUM_DEBUG_MODES ) {
	    print_warning( CONFIGURATION_WARNING,
			   "unrecognized debug mode `%s'", p );
	}
    }

#endif /* TUXRACER_NO_DEBUG */

}

void print_debug( debug_mode_t mode, char *fmt, ... )
{
#ifndef TUXRACER_NO_DEBUG

    va_list args;

    check_assertion( 0 <= mode && mode < NUM_DEBUG_MODES,
		     "invalid debugging mode" );

    if ( ! debug_setting[ mode ] ) {
	return;
    }

    va_start( args, fmt );

    fprintf( stderr, PROG_NAME " debug (%s): ", debug_desc[ mode ] );
    vfprintf( stderr, fmt, args );
    fprintf( stderr, "\n" );

    va_end( args );

#endif /* TUXRACER_NO_DEBUG */
}

