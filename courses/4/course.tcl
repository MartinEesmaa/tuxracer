tux_course_name "Path of Daggers"
tux_course_author "Jasmin Patry <jfpatry@cgl.uwaterloo.ca>"
tux_course_dim 54 800
tux_start_pt 45 3.0  ;# start
#tux_start_pt 4 60    ;# 1st entrance
#tux_start_pt 14 69   ;# 2st entrance
#tux_start_pt 30 69   ;# 3rd entrance
#tux_start_pt 48 60   ;# 4th entrance
#tux_start_pt 45 160  ;# mid ramp
#tux_start_pt 14 160  ;# mid floor
#tux_start_pt 10 480  ;# middle left bobsled
#tux_start_pt 30 360  ;# right bobsled entrance
#tux_start_pt 10 330  ;# left bobsled entrance
tux_angle 23 
tux_elev_scale 12
tux_base_height_value 80
tux_elev elev.rgb
tux_terrain terrain.rgb
tux_tree_size 2.2 6
tux_trees trees.rgb
tux_bgnd_img ../common/background3.rgb
tux_tree_tex ../common/tree.rgb
tux_ice_tex ../common/ice.rgb
tux_rock_tex ../common/rock.rgb
tux_snow_tex ../common/snow.rgb
tux_friction 0.25 0.9 0.35

source ../common/tux_walk.tcl 
source ../common/standard_light.tcl
source ../common/tree_polyhedron.tcl
