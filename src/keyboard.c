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
#include "keyboard.h"
#include "keyboard_util.h"

#define KEYMAP_SIZE 1000
#define KEYTABLE_SIZE UCHAR_MAX
#define SPECIAL_KEYTABLE_SIZE UCHAR_MAX

static key_cb_t keytable[KEYTABLE_SIZE];
static key_cb_t special_keytable[SPECIAL_KEYTABLE_SIZE];

static keymap_t keymap[KEYMAP_SIZE];
static int num_keymap_entries = 0;

/*
    { START, DEFAULT_CALLBACK, NULL, NULL, start_menu_cb },
    { RACING, CONFIGURABLE_KEY, "esc", get_quit_key, quit_racing_cb },
    { RACING, CONFIGURABLE_KEY, "j", get_turn_left_key, turn_left_cb },
    { RACING, CONFIGURABLE_KEY, "l", get_turn_right_key, turn_right_cb },
    { RACING, CONFIGURABLE_KEY, "space", get_brake_key, brake_cb },
    { GAME_OVER, DEFAULT_CALLBACK, NULL, NULL, game_over_cb }
*/

int add_keymap_entry( game_mode_t mode, keymap_class_t class,
		      char *keys, key_func_t key_func, key_cb_t key_cb )
{
    if ( num_keymap_entries == KEYMAP_SIZE ) {
	return 1; /* keymap full */
    }

    keymap[ num_keymap_entries ].mode = mode;
    keymap[ num_keymap_entries ].class = class;
    keymap[ num_keymap_entries ].keys = keys;
    keymap[ num_keymap_entries ].key_func = key_func;
    keymap[ num_keymap_entries ].key_cb = key_cb;

    num_keymap_entries += 1;

    return 0; /* success */
}

static void fill_keytable( key_cb_t value  )
{
    int i;

    for (i=0; i<KEYTABLE_SIZE; i++) {
	keytable[i] = value;
    }

    for (i=0; i<SPECIAL_KEYTABLE_SIZE; i++) {
	special_keytable[i] = value;
    }
}

static int insert_keytable_entries( char *keys, key_cb_t callback )
{
    key_desc_t *key_list;
    int num_keys;
    int i;

    num_keys = translate_key_string( keys, &key_list );

    if ( num_keys > 0 ) {
	for ( i=0; i<num_keys; i++ ) {
	    if ( key_list[i].special ) {
		special_keytable[ key_list[i].key ] = callback;
	    } else {
		keytable[ key_list[i].key ] = callback;
	    }
	} 

	free( key_list );
	return 1;

    } else {
	return 0;
    }
}

static void init_keytable( game_mode_t mode )
{
    int i;
    char *keys;
    fill_keytable( NULL );

    /* Handle default callbacks first */
    for (i=0; i<num_keymap_entries; i++) {
	if ( keymap[i].mode == mode && keymap[i].class == DEFAULT_CALLBACK ) {
	    fill_keytable( keymap[i].key_cb );
	}
    }

    /* Handle other classes */
    for (i=0; i<num_keymap_entries; i++) {
	if ( keymap[i].mode == mode ) {
	    switch ( keymap[i].class ) {
	    case FIXED_KEY:
		if ( ! insert_keytable_entries( keymap[i].keys, 
						keymap[i].key_cb ) )
		{
		    assert( 0 );
		}

		break;

	    case CONFIGURABLE_KEY:
		keys = keymap[i].key_func();
		if ( ! insert_keytable_entries( keys, keymap[i].key_cb ) )
		{
		    fprintf( stderr, "Tux Racer warning: key specification "
			     "'%s' is unrecognized; using '%s' instead.\n",
			     keys, keymap[i].keys );
		    if ( ! insert_keytable_entries( keymap[i].keys, 
						    keymap[i].key_cb ) )
		    {
			assert( 0 );
		    }
		}

		break;

	    case DEFAULT_CALLBACK:
		/* handled above */
		break;

            default:
		assert(0);
	    }

	}
    }
}

static void keyboard_handler( int key, bool_t special, bool_t release, int x, int y )
{
    static game_mode_t last_mode = NO_MODE;
    key_cb_t *table;

    if ( last_mode != g_game.mode ) {
	last_mode = g_game.mode;
	init_keytable( last_mode );
    }

    if ( special ) {
	table = special_keytable;
    } else {
	table = keytable;
    }

    if ( table[key] != NULL ) {
	(table[key])( key, special, release, x, y );
    }
}

static void glut_keyboard_cb( unsigned char ch, int x, int y ) 
{
    keyboard_handler( ch, False, False, x, y );
}

static void glut_special_cb( int key, int x, int y ) 
{
    keyboard_handler( key, True, False, x, y );
}

static void glut_keyboard_up_cb( unsigned char ch, int x, int y ) 
{
    keyboard_handler( ch, False, True, x, y );
}

static void glut_special_up_cb( int key, int x, int y ) 
{
    keyboard_handler( key, True, True, x, y );
}

void init_keyboard()
{
    glutKeyboardFunc( glut_keyboard_cb );
    glutKeyboardUpFunc( glut_keyboard_up_cb );
    glutSpecialFunc( glut_special_cb );
    glutSpecialUpFunc( glut_special_up_cb );
}
