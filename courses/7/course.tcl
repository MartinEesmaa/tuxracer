tux_course_dim 200 400
tux_course_name "Ice Canyon at Night"
tux_course_author "Ingo Ruhnke <grumbel@gmx.de>"
tux_start_pt 100 2.0  ;# start
#tux_start_pt 4 60    ;# 1st entrance
#tux_start_pt 14 69   ;# 2st entrance
#tux_start_pt 30 69   ;# 3rd entrance
#tux_start_pt 48 60   ;# 4th entrance
#tux_start_pt 45 160  ;# mid ramp
#tux_start_pt 14 160  ;# mid floor
tux_angle 23 
tux_elev_scale 25
tux_base_height_value 67
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

tux_course_light 0 -on -position { 1 1 1 0 } -diffuse { 0.2 0.2 0.2 1 } \
     -specular { 0.5 0.5 0.5 1 } -ambient { 0.05 0.05 0.05 1.0 }

tux_fog -on -mode linear -start 10 -end 60 -colour { 0.0 0.0 0.0 1.0 }

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

# Keyframe data goes here
