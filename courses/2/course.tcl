tux_course_name "Test 2"
tux_course_dim 24.5 210
tux_start_pt 14 3.5 
tux_angle 23 
tux_elev_scale 10.5
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

# Keyframe data
source ../common/tux_moonwalk.tcl

# Lights
tux_course_light 0 -on -position { 1 1 1 0 } -diffuse { 0.2 0.2 0.2 1 } \
     -specular { 0.3 0.3 0.3 1 } -ambient { 0.5 0.5 0.5 1.0 }

# Fog
# This creates fog that starts 10m in front of the camera and becomes totally
# opaque 60 m in front of camera
tux_fog -on -mode linear -start 10 -end 60 -colour { 0.8 0.8 0.84 1.0 }

# Tree shape
source ../common/tree_polyhedron.tcl

# Particles
tux_particle_colour -diffuse { 0.5 0.5 0.6 0.7 } -specular { 0 0 0 1 } \
    -shininess 0
