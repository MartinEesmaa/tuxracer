#
# Course configuration
#
tux_course_name "The Downhill Fear"
tux_course_author "Julien Canet <jools@free.fr>"
tux_course_dim 60 1800        ;# width, length of course in m
tux_start_pt 30 3.5           ;# start position, measured from left rear corner
tux_angle 23                   ;# angle of course
tux_elev_scale 7.0             ;# amount by which to scale elevation data
tux_base_height_value 202      ;# greyscale value corresponding to height
                               ;#     offset of 0 (integer from 0 - 255)
tux_elev elev.rgb              ;# bitmap specifying course elevations
tux_terrain terrain.rgb        ;# bitmap specifying terrains type
tux_tree_size 1.9 5.0          ;# average diameter, height of trees
tux_trees trees.rgb            ;# bitmap specifying tree locations

#
# Textures
#
tux_bgnd_img ../common/background1.rgb ;# image to use for background
tux_tree_tex ../common/tree.rgb   ;# tree image
tux_ice_tex ../common/ice.rgb     ;# ice image
tux_rock_tex ../common/rock.rgb   ;# rock image
tux_snow_tex ../common/snow.rgb   ;# snow image

tux_friction 0.20 0.8 0.33     ;# ice, rock, snow friction coefficients

#
# Introductory animation keyframe data
#
source ../common/tux_walk.tcl

#
# Lighting
#
source ../common/standard_light.tcl

#
# Fog
# These settings correspond to the default fog settings
# Note that in exp or exp2 mode, -start and -end have no effect 
# (they are used in linear mode; see course #2)
#
tux_fog -on -mode exp -density 0.005 -colour { 1 1 1 1 } -start 0 -end 1

#
# Particle colour
#
tux_particle_colour -diffuse { 0.5 0.5 0.8 0.9 } -specular { 0.3 0.3 0.3 1 } \
    -shininess 0

#
# Tree shape
#
source ../common/tree_polyhedron.tcl

