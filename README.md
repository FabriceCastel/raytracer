Fab's Raytracer
===============

Objectives:

1. ~~Texture mapping~~
2. ~~Volumetric lighting~~
3. Height mapping on a sphere [BACKBURNER]
4. ~~Post-processing filter~~
5. ~~Refraction~~
6. ~~Blinn-Phong lighting (to replace current Phong model)~~
7. ~~Generate animation frames based off MIDI input~~
8. ~~Particle system~~
9. ~~Optimize raytracer with multithreading~~
10. ~~4th degree implicit surface primitive with genus > 0~~

A ray tracer written for CS488, to be extended as a final project due Dec 1st 2014, I'm keeping progress logs below:

# November 29th - 9:30pm

The fourth degree implicit surface normal took a while to figure out since I wasn't aquainted with the concept of gradient fields, but now I've got it up and running and it looks awesome! That's nine out of my ten objectives done, I'm really happy with the way this is panning out. Now, to figure out how to scale this beast...

![Alt text](/samples/sampleimplicit3.png?raw=true "Sample implicit surface")

# November 29th - 6:20pm

A wild fourth degree implicit surface has appeared. Working on normal finding for it now, please don't let me down like the height maps did dear blob object.

![Alt text](/samples/sampleimplicit.png?raw=true "Sample implicit surface")

# November 29th - 4:15pm

Soft shadows are done but they're taking way too long to render, I don't think I'll be able to use them as is in the final video. If I cast less than around 25 rays, the shadows look noisy (and that's with SSAAx2 enabled). Here I'm casting 50 rays per point with SSAAx2. It took 33 seconds to render the frame.. welp, at least there goes volumetric lighting (sort of). The 4th degree implicit surface is very non-cooperative at the moment and results in a cloud of points.

![Alt text](/samples/samplesoftshadow.png?raw=true "Sample soft shadow")

# November 28th - 12:40am

Texture mapping works for cubes via a Lua interface now, that's one more item off my list. Getting pretty close to completing it :) I've still got to link it up with the MIDI in order to *really* finish off that objective but given how little work that involves I'll consider this an early victory.

![Alt text](/samples/sampletexture.png?raw=true "Sample texture mapping")

# November 26th - 2:40am

Particles galore, I've got them working, set up as Lua script input and MIDI-ready. They're more fun to watch with the animation but youtube keeps rejecting my .avi so a still will have to do for the time being.

![Alt text](samples/sampleparticles.png?raw=true "Sample particles")

# November 25th - 11:00am

Multithreading is in place, it's sped up render times considerably. As far as performance is concerned there are still some minor memory issues to deal with. Overall it's looking pretty good though.

Here are some stats for a 500 frame render on my 2014 macbook pro.

|                        | Single thread | 16 threads |
|------------------------|---------------|------------|
| Average time per frame | 1.39s         | 0.54s      |
| Total time             | 11:34         | 4:30       |

# November 24th - 11:30am

I've successfully generated an output video that syncs up with the MIDI input and uses a linear release envelope to smoothen out the applied filter on the snare hits. There were some issues with ffmpeg sticking to 25fps despite having -r 30 set, but it's not much of a concern now. I'll stick to 25fps and make note of finding a fix for that to allow for variable frame rates in the future.

# November 19th - 9:15pm

MIDI input works now. All that's left to do with regards to that is passing the info through the intersection method and have the concerned objects check the MIDI state for the given frame to decide whatever it is they need to decide. Oh and allowing the user to specify what note to attach to what parameter with the Lua script. Not bad, not bad. I meant to post an exapmle video with the Sin City filter getting triggered on and off but youtube wouldn't take my output .avi for reasons unknown to me.

# November 19th - 9:50am

I've solved the velocity problem! Turns out the STK library was passing me back the three components of MIDI events backwards, so instead of reading the velocity data I was reading the status byte and getting wonky results. D'oh. Now that this is sorted, I've got to convert event ticks to absolute time and find a nice way of passing this information along through my renderer. Easy, right?

# November 18th - 8:30pm

I'm taking a third stab at the MIDI file interpretation, this time with the STK library. So far the note ID and ticks between MIDI events seem to read properly, the only issue is the velocities I'm reading that are a bit out of whack. Thankfully velocity isn't a crucial feature, so I might roll with what I've got right now and come back to try and fix velocity past the project due date. Good thing I switched over to this library, writing my own MIDI SMF0 parser was starting to make me rip my hair out. The in-progress is on the midi_third_attempt branch, I'll take another quick jab at velocity then merge it.

# November 15th - 5:10pm

I've implemented a Sin City-like post-processing filter. The first image below is pre-filter and the second is post-filter. The results look pretty decent but it took a lot of messing around with the parameters, I'm not sure how well this will apply to my geometry further down the line. At any rate it shouldn't be too hard now that I've put these tests in place.

![Alt text](samples/prefilter.png?raw=true "Sample render")
![Alt text](samples/postfilter.png?raw=true "Sample render")

# November 14th 2014 - 1:15pm

Valgrind is love! A little while back I tried sticking my render method in a for loop to generate animation frames and the render rates absolutely tanked past the first frame. This was due to memory leaks which I've now tackled with the help of the trusty valgrind --leak-check=yes tool. This doesn't strike any objectives off my list, but it sure is reassuring to know I'll be able to generate my animation when it comes down to that.

# November 13th 2014 - 9:30pm

I'm putting the height map on hold for a moment to knock off some smaller objectives and take a bit of a breather. I've upgraded the lighting model from Phong to Blinn-Phong. I've set up the sample render (image below) to use the Blinn specular value for the red channel and the Phong specular for the green, so the orange areas are more or less the same with both specular models. The difference is very noticeable around the specular dots on the glass sphere.

![Alt text](samples/sampleblinn.png?raw=true "Sample render")

# November 8th 2014 - 2:20pm

The height mapping algorithm is painful to implement. I'm able to get the correct outline of the warped sphere now and I can also get the normals right for most of the surface, but my method of adding an epsilon to the theta and phi (in spherical coordinates) isn't working too well at the poles, so I'll have to try a tangent plane method next. I'm getting close though. I've also updated the objectives list to reflect the final project proposal.

![Alt text](samples/sampleheight1.png?raw=true "Sample render")


# November 5th 2014 - 1:30pm

I've been working on the heightmap feature for spheres for several hours now. The algorithm is ridiculously hard to implement given how confused I am about the math background involved here. In other news, I've improved the glass fakery, so there's that for progress.

![Alt text](samples/samplebetterglass.png?raw=true "Sample render")


# November 2nd 2014 - 6:00pm

I've added refraction to the sphere objects; the raytracer can now emulate glass-like surfaces.

![Alt text](samples/samplerefraction.png?raw=true "Sample render")