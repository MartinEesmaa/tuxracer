#
# Course configuration
#
tux_course_dim 17.5 175        ;# width, length of course in m
tux_start_pt 9.8 3.5           ;# start position, measured from left rear corner
tux_angle 23                   ;# angle of course
tux_elev_scale 7.0             ;# amount by which to scale elevation data
tux_elev elev.rgb              ;# bitmap specifying course elevations
tux_terrain terrain.rgb        ;# bitmap specifying terrains type
tux_tree_size 1.4 3.2          ;# average diameter, height of trees
tux_trees trees.rgb            ;# bitmap specifying tree locations

#
# Textures
#
tux_bgnd_img background.rgb       ;# image to use for background
tux_tree_tex ../common/tree.rgb   ;# tree image
tux_ice_tex ../common/ice.rgb     ;# ice image
tux_rock_tex ../common/rock.rgb   ;# rock image
tux_snow_tex ../common/snow.rgb   ;# snow image

tux_friction 0.25 0.9 0.35     ;# ice, rock, snow friction coefficients

#
# Polyhedron representing tree 
#
# Syntax: tux_tree_poly <vertex list> <polygon list>
#     where each polygon in the polygon list is a list of indices into
#     the vertex list.
#
tux_tree_poly {              
    { 0 0 0 }
    { 0.0 0.15  0.5 }
    { 0.5 0.15  0.0 }
    { 0.0 0.15 -0.5 }
    {-0.5 0.15  0.0 }
    { 0 1 0 }
} {
    { 0 1 4 }
    { 0 2 1 }
    { 0 3 2 }
    { 0 4 3 }
    { 1 5 4 }
    { 2 5 1 }
    { 3 5 2 }
    { 4 5 3 }
} 

#
# Introductory animation keyframe data
#
tux_key_frame 0.5 8.05 3.5 0.455 90 0 -20.6 39.8 -23.4 18.2
tux_key_frame 1.0 8.4  3.5 0.455 90 0 39.8 -20.6 18.2 -23.4
tux_key_frame 1.5 8.75 3.5 0.455 90 0 -20.6 39.8 -23.4 18.2
tux_key_frame 2.0 9.1  3.5 0.455 90 0 39.8 -20.6 18.2 -23.4
tux_key_frame 2.5 9.45 3.5 0.455 90 0 -20.6 39.8 -23.4 18.2
tux_key_frame 3.0 9.8  3.5 0.455 90 0 39.8 -20.6 18.2 -23.4
tux_key_frame 3.5 9.8  3.5 0.21 180 0 0 0 +80 +80
tux_key_frame 4.0 9.8  3.5 0.0 180 -66 0 0 0 0
