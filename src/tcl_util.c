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

#include "tcl_util.h"

/* Parse an n-tuple of doubles specified as a tcl-list.
 * Used for grabbing point or vector coordinates, colours, and other things.
 * Puts results into an array of doubles.
 */
int get_tcl_tuple ( Tcl_Interp *ip, char *inList, scalar_t *p, int n ) 
{
    char **indices;
    double tmp;
    int num_doubles;
    int rtn;
    char s[100];
    int i;

    rtn = Tcl_SplitList(ip, inList, &num_doubles, &indices);

    if ((TCL_OK != rtn) || (n != num_doubles)) {
	sprintf(s,"%d",n);
	Tcl_AppendResult(ip, 
			 "Expected a tuple of ", s, " doubles.\n",
			 (char *) 0
	    );
	Tcl_Free((char *)indices);
	return TCL_ERROR;
    }

    for (i = 0; i < n; i++) {
	if (TCL_OK != Tcl_GetDouble(ip, indices[i], &tmp)) {
	    Tcl_Free((char *)indices);
	    sprintf(s,"%d",n);
	    Tcl_AppendResult(ip, 
			     "Expected a tuple of ", s, " doubles.\n",
			     (char *) 0
		);
	    return TCL_ERROR;
	}
	p[i] = tmp;
    }
    Tcl_Free((char *)indices);
    return TCL_OK;

}

point2d_t make_point2d_from_array ( scalar_t *p ) 
{
    return make_point2d(p[0],p[1]);
}

point_t make_point_from_array ( scalar_t *p ) 
{
    return make_point(p[0],p[1],p[2]);
}

vector_t make_vector_from_array ( scalar_t *v ) 
{
    return make_vector(v[0],v[1],v[2]);
}

colour_t make_colour_from_array ( scalar_t *c ) 
{
    return make_colour(c[0],c[1],c[2]);
}

colour_t make_colour( scalar_t r, scalar_t g, scalar_t b)
{
    colour_t result;
    result.r = r;
    result.g = g;
    result.b = b;
    return result;
}
