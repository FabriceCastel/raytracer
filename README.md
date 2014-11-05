Fab's Raytracer
===============

Objectives:

1. Texture mapping
2. Volumetric lighting
3. Height mapping
4. Depth of field
5. ~~Refraction~~
6. Blinn-Phong lighting (to replace current Phong model)
7. Generate animation frames based off MIDI input
8. Particle system
9. Taurus primitive
10. Non-taurus 4th degree implicit surface primitive with genus > 1

A ray tracer written for CS488, to be extended as a final project due Dec 1st 2014, I'm keeping progress logs below:


# November 5th 2014 - 1:30pm

I've been working on the heightmap feature for spheres for several hours now. The algorithm is ridiculously hard to implement given how confused I am about the math background involved here. In other news, I've improved the glass fakery, so there's that for progress.

![Alt text](/samplebetterglass.png?raw=true "Sample render")


# November 2nd 2014 - 6:00pm

I've added refraction to the sphere objects; the raytracer can now emulate glass-like surfaces.

![Alt text](/samplerefraction.png?raw=true "Sample render")