-- A simple scene with some miscellaneous geometry.

black = gr.material({0.1, 0.1, 0.1}, {1.0, 1.0, 1.0}, 70)
white = gr.material({1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, 100)

red = gr.material({1.0, 0.0, 0.0}, {0.5, 0.7, 0.5}, 30)
orange = gr.material({1.0, 0.5, 0.0}, {0.5, 0.7, 0.5}, 40)
yellow = gr.material({1.0, 1.0, 0.0}, {0.5, 0.7, 0.5}, 45)
green = gr.material({0.0, 1.0, 0.0}, {0.5, 0.7, 0.5}, 50)
blue = gr.material({0.0, 0.0, 1.0}, {0.5, 0.7, 0.5}, 60)
indigo = gr.material({0.3, 0.0, 0.6}, {0.5, 0.7, 0.5}, 90)
violet = gr.material({0.6, 0.0, 1.0}, {0.5, 0.7, 0.5}, 110)

scene_root = gr.node('root')

ground = gr.nh_box('ground', {-300, -800, -600}, 600)
scene_root:add_child(ground)
ground:set_material(violet)

s1 = gr.nh_sphere('s1', {0, -145, -100}, 50)
scene_root:add_child(s1)
s1:set_material(green)

s2 = gr.nh_sphere('s2', {-100, -135, -120}, 50)
scene_root:add_child(s2)
s2:set_material(yellow)

s3 = gr.nh_sphere('s3', {100, -135, -120}, 50)
scene_root:add_child(s3)
s3:set_material(blue)

s4 = gr.nh_sphere('s4', {-180, -120, -160}, 50)
scene_root:add_child(s4)
s4:set_material(orange)

s5 = gr.nh_sphere('s5', {180, -120, -160}, 50)
scene_root:add_child(s5)
s5:set_material(indigo)

s6 = gr.nh_sphere('s6', {-220, -50, -200}, 50)
scene_root:add_child(s6)
s6:set_material(red)

s7 = gr.nh_sphere('s7', {220, -50, -200}, 50)
scene_root:add_child(s7)
s7:set_material(violet)

re = gr.nh_box('re', {120, 190, -330}, 100)
scene_root:add_child(re)
re:set_material(white)

-- A small stellated dodecahedron.

require('smstdodeca')

steldodec:set_material(black)
scene_root:add_child(steldodec)

white_light = gr.light({0.0, 400.0, 200.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 0.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'sample.png', 600, 600,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
