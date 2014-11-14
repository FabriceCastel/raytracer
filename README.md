Fab's Raytracer
===============

Objectives:

1. Texture mapping
2. Volumetric lighting
3. Height mapping
4. Post-processing filter
5. ~~Refraction~~
6. ~~Blinn-Phong lighting (to replace current Phong model)~~
7. Generate animation frames based off MIDI input
8. Particle system
9. Optimize raytracer with multithreading
10. 4th degree implicit surface primitive with genus > 0

A ray tracer written for CS488, to be extended as a final project due Dec 1st 2014, I'm keeping progress logs below:

# November 13th 2014 - 9:30pm

I'm putting the height map on hold for a moment to knock off some smaller objectives for now. I've upgraded the lighting model from Phong to Blinn-Phong. I've set up the render to use the blinn specular value for the green channel and the phong specular for the red, so the orange areas are more or less the same with both specular models but the difference is noticeable around the specular dot on the glass sphere.

![Alt text](/sampleblinn.png?raw=true "Sample render");

# November 8th 2014 - 2:20pm

The height mapping algorithm is painful to implement. I'm able to get the correct outline of the warped sphere now and I can also get the normals right for most of the surface, but my method of adding an epsilon to the theta and phi (in spherical coordinates) isn't working too well at the poles, so I'll have to try a tangent plane method next. I'm getting close though. I've also updated the objectives list to reflect the final project proposal.

![Alt text](/sampleheight1.png?raw=true "Sample render")


# November 5th 2014 - 1:30pm

I've been working on the heightmap feature for spheres for several hours now. The algorithm is ridiculously hard to implement given how confused I am about the math background involved here. In other news, I've improved the glass fakery, so there's that for progress.

![Alt text](/samplebetterglass.png?raw=true "Sample render")


# November 2nd 2014 - 6:00pm

I've added refraction to the sphere objects; the raytracer can now emulate glass-like surfaces.

![Alt text](/samplerefraction.png?raw=true "Sample render")