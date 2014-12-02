-- A simple scene with some miscellaneous geometry.

black = gr.material({0.1, 0.1, 0.1}, {1.0, 1.0, 1.0}, 70)
white = gr.material({0.7, 0.4, 0.4}, {1.0, 1.0, 1.0}, 0)
glass = gr.material({0.0, 0.24, 0.2}, {0.7, 1.0, 0.8}, 400)

red = gr.material({1.0, 0.2, 0.2}, {0.5, 0.7, 0.5}, 100)
orange = gr.material({1.0, 0.5, 0.0}, {0.5, 0.7, 0.5}, 40)
yellow = gr.material({1.0, 1.0, 0.0}, {0.5, 0.7, 0.5}, 45)
green = gr.material({0.0, 1.0, 0.0}, {0.5, 0.7, 0.5}, 50)
blue = gr.material({0.4, 0.4, 1.0}, {0.5, 0.7, 0.5}, 60)
indigo = gr.material({0.3, 0.0, 0.6}, {0.5, 0.7, 0.5}, 90)
violet = gr.material({0.1, 0.1, 0.1}, {0.5, 0.7, 0.5}, 70)

chicken = gr.texture('../data/cocainebear.png')

scene_root = gr.node('root')

--ground = gr.nh_box('ground', {-3000, -10300, -7000}, 10000)
--scene_root:add_child(ground)
--ground:set_material(white)

wall = gr.nh_box('wall', {-200, -200, -500}, 400)
scene_root:add_child(wall)
wall:set_material(black)
wall:set_texture(chicken, 2)

snow = gr.emitter('snow', {-6, -14, -15}, 12, {0, 0.07, 0}, 0.5, 0.3)
scene_root:add_child(snow)

-- box = gr.nh_box('box', {80, -50, -900}, 400)
-- scene_root:add_child(box)
-- box:set_material(orange)

-- chicken = gr.texture('../samples/prefilter.png')
-- box:set_texture(chicken, 9)

-- box4 = gr.nh_box('box4', {-400, -40, -40}, 80)
-- scene_root:add_child(box4)
-- box4:set_material(indigo)

-- box2 = gr.nh_box('box2', {-400, -0, -1200}, 300)
-- scene_root:add_child(box2)
-- box2:set_material(green)

s1 = gr.nh_sphere('s1', {0, 0, 19}, 2)
scene_root:add_child(s1)
s1:set_material(glass)

tc = gr.nh_tanglecube('tc', {0, 0, 0}, 1)
scene_root:add_child(tc)
tc:set_material(red)







c0 = gr.nh_box('c0', {-2.15, -2.15, -8}, 1)
scene_root:add_child(c0)
c0:set_material(white)
c1 = gr.nh_box('c1', {-1.05, -2.15, -8}, 1)
scene_root:add_child(c1)
c1:set_material(white)
c2 = gr.nh_box('c2', {0.05, -2.15, -8}, 1)
scene_root:add_child(c2)
c2:set_material(white)
c3 = gr.nh_box('c3', {1.15, -2.15, -8}, 1)
scene_root:add_child(c3)
c3:set_material(white)
c4 = gr.nh_box('c4', {-2.15, -1.05, -8}, 1)
scene_root:add_child(c4)
c4:set_material(white)
c5 = gr.nh_box('c5', {-1.05, -1.05, -8}, 1)
scene_root:add_child(c5)
c5:set_material(white)
c6 = gr.nh_box('c6', {0.05, -1.05, -8}, 1)
scene_root:add_child(c6)
c6:set_material(white)
c7 = gr.nh_box('c7', {1.15, -1.05, -8}, 1)
scene_root:add_child(c7)
c7:set_material(white)
c8 = gr.nh_box('c8', {-2.15, 0.05, -8}, 1)
scene_root:add_child(c8)
c8:set_material(white)
c9 = gr.nh_box('c9', {-1.05, 0.05, -8}, 1)
scene_root:add_child(c9)
c9:set_material(white)
c10 = gr.nh_box('c10', {0.05, 0.05, -8}, 1)
scene_root:add_child(c10)
c10:set_material(white)
c11 = gr.nh_box('c11', {1.15, 0.05, -8}, 1)
scene_root:add_child(c11)
c11:set_material(white)
c12 = gr.nh_box('c12', {-2.15, 1.15, -8}, 1)
scene_root:add_child(c12)
c12:set_material(white)
c13 = gr.nh_box('c13', {-1.05, 1.15, -8}, 1)
scene_root:add_child(c13)
c13:set_material(white)
c14 = gr.nh_box('c14', {0.05, 1.15, -8}, 1)
scene_root:add_child(c14)
c14:set_material(white)
c15 = gr.nh_box('c15', {1.15, 1.15, -8}, 1)
scene_root:add_child(c15)
c15:set_material(white)

b0 = gr.texture('../data/b0.png')
c5:set_texture(b0, 9)
b1 = gr.texture('../data/b1.png')
c7:set_texture(b1, 10)
b2 = gr.texture('../data/b2.png')
c6:set_texture(b2, 11)
b3 = gr.texture('../data/b3.png')
c9:set_texture(b3, 12)
b4 = gr.texture('../data/b4.png')
c10:set_texture(b4, 13)

c0:set_texture(b0, 9)
c1:set_texture(b2, 11)
c2:set_texture(b3, 12)
c3:set_texture(b4, 13)
c4:set_texture(b3, 12)
c7:set_texture(b0, 9)
c8:set_texture(b4, 13)
c11:set_texture(b2, 11)
c12:set_texture(b3, 12)
c13:set_texture(b1, 10)
c14:set_texture(b0, 9)
c15:set_texture(b4, 13)





white_light = gr.light({-200.0, 800.0, 600.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 0.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

-- gr.render(scene_root, 'sample', 700, 500,
-- 	  {0, 0, 1200}, {0, 0, -1}, {0, 1, 0}, 50,
-- 	  {0.3, 0.3, 0.3}, {white_light})

gr.render(scene_root, 'sample', 700,700,--2880, 1800, --640, 480,--175, 125,--700, 500,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 25,
	  {0.3, 0.3, 0.3}, {white_light})
