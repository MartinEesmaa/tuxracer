tux_course_name "Ingo's Speedway"
tux_course_author "Ingo Ruhnke <grumbel@gmx.de>"
tux_course_dim 54 800
tux_start_pt 20 2.0  ;# start
#tux_start_pt 4 60    ;# 1st entrance
#tux_start_pt 14 69   ;# 2st entrance
#tux_start_pt 30 69   ;# 3rd entrance
#tux_start_pt 48 60   ;# 4th entrance
#tux_start_pt 45 160  ;# mid ramp
#tux_start_pt 14 160  ;# mid floor
tux_angle 23 
tux_elev_scale 12
tux_elev elev.rgb
tux_terrain terrain.rgb
tux_tree_size 1.4 3.15
tux_trees trees.rgb
tux_bgnd_img ../common/background2.rgb
tux_tree_tex ../common/tree.rgb
tux_ice_tex ../common/ice.rgb
tux_rock_tex ../common/rock.rgb
tux_snow_tex ../common/snow.rgb
tux_friction 0.25 0.9 0.35

source ../common/tux_walk.tcl
source ../common/standard_light.tcl
source ../common/tree_polyhedron.tcl
