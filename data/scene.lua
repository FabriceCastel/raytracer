-- A simple scene with some miscellaneous geometry.

black = gr.material({0.1, 0.1, 0.1}, {1.0, 1.0, 1.0}, 70)
white = gr.material({1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, 0)
glass = gr.material({0.0, 0.24, 0.2}, {0.7, 1.0, 0.8}, 400)

red = gr.material({1.0, 0.0, 0.0}, {0.5, 0.7, 0.5}, 30)
orange = gr.material({1.0, 0.5, 0.0}, {0.5, 0.7, 0.5}, 40)
yellow = gr.material({1.0, 1.0, 0.0}, {0.5, 0.7, 0.5}, 45)
green = gr.material({0.0, 1.0, 0.0}, {0.5, 0.7, 0.5}, 50)
blue = gr.material({0.0, 0.0, 1.0}, {0.5, 0.7, 0.5}, 60)
indigo = gr.material({0.3, 0.0, 0.6}, {0.5, 0.7, 0.5}, 90)
violet = gr.material({0.6, 0.0, 1.0}, {0.5, 0.7, 0.5}, 110)

scene_root = gr.node('root')

--ground = gr.nh_box('ground', {-3000, -10300, -7000}, 10000)
--scene_root:add_child(ground)
--ground:set_material(white)

wall = gr.nh_box('wall', {-3500, -3500, -10000}, 7000)
scene_root:add_child(wall)
wall:set_material(violet)

box = gr.nh_box('box', {80, -50, -900}, 400)
scene_root:add_child(box)
box:set_material(orange)

box4 = gr.nh_box('box4', {-400, -40, -40}, 80)
scene_root:add_child(box4)
box4:set_material(indigo)

box2 = gr.nh_box('box2', {-400, -0, -1200}, 300)
scene_root:add_child(box2)
box2:set_material(green)

s1 = gr.nh_sphere('s1', {0, 0, -100}, 200)
scene_root:add_child(s1)
s1:set_material(glass)


white_light = gr.light({-200.0, 800.0, 600.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 0.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'sample', 700, 500,
	  {0, 0, 1200}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})
