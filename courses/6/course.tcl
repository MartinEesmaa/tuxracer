#
# Course configuration
#
tux_course_author "Steve Baker <sjbaker1@airmail.net>"
tux_course_dim 75 155          ;# width, length of course in m
tux_start_pt 55 3.5            ;# start position, measured from left rear corner
tux_angle 30                   ;# angle of course
tux_elev_scale 15.0            ;# amount by which to scale elevation data
tux_base_height_value 0        ;# greyscale value corresponding to height
                               ;#     offset of 0 (integer from 0 - 255)
tux_elev elev.rgb              ;# bitmap specifying course elevations
tux_terrain terrain.rgb        ;# bitmap specifying terrains type
tux_tree_size 1.4 3.2          ;# average diameter, height of trees
tux_trees trees.rgb            ;# bitmap specifying tree locations

#
# Textures
#
tux_bgnd_img ../common/background2.rgb       ;# image to use for background
tux_tree_tex ../common/tree.rgb   ;# tree image
tux_ice_tex ../common/ice.rgb     ;# ice image
tux_rock_tex ../common/rock.rgb   ;# rock image
tux_snow_tex ../common/snow.rgb   ;# snow image

tux_friction 0.15 0.5 0.25     ;# ice, rock, snow friction coefficients

# Introductory animation keyframe data
source ../common/tux_walk.tcl

# Lighting
source ../common/standard_light.tcl

# Tree shape
source ../common/tree_polyhedron.tcl
