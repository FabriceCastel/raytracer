Fab's Raytracer
===============

Objectives:

1. Texture mapping
2. Volumetric lighting
3. Height mapping [IN PROGRESS]
4. ~~Post-processing filter~~
5. ~~Refraction~~
6. ~~Blinn-Phong lighting (to replace current Phong model)~~
7. Generate animation frames based off MIDI input [IN PROGRESS]
8. Particle system
9. Optimize raytracer with multithreading
10. 4th degree implicit surface primitive with genus > 0

A ray tracer written for CS488, to be extended as a final project due Dec 1st 2014, I'm keeping progress logs below:

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

![Alt text](/prefilter.png?raw=true "Sample render")
![Alt text](/postfilter.png?raw=true "Sample render")

# November 14th 2014 - 1:15pm

Valgrind is love! A little while back I tried sticking my render method in a for loop to generate animation frames and the render rates absolutely tanked past the first frame. This was due to memory leaks which I've now tackled with the help of the trusty valgrind --leak-check=yes tool. This doesn't strike any objectives off my list, but it sure is reassuring to know I'll be able to generate my animation when it comes down to that.

# November 13th 2014 - 9:30pm

I'm putting the height map on hold for a moment to knock off some smaller objectives and take a bit of a breather. I've upgraded the lighting model from Phong to Blinn-Phong. I've set up the sample render (image below) to use the Blinn specular value for the red channel and the Phong specular for the green, so the orange areas are more or less the same with both specular models. The difference is very noticeable around the specular dots on the glass sphere.

![Alt text](/sampleblinn.png?raw=true "Sample render")

# November 8th 2014 - 2:20pm

The height mapping algorithm is painful to implement. I'm able to get the correct outline of the warped sphere now and I can also get the normals right for most of the surface, but my method of adding an epsilon to the theta and phi (in spherical coordinates) isn't working too well at the poles, so I'll have to try a tangent plane method next. I'm getting close though. I've also updated the objectives list to reflect the final project proposal.

![Alt text](/sampleheight1.png?raw=true "Sample render")


# November 5th 2014 - 1:30pm

I've been working on the heightmap feature for spheres for several hours now. The algorithm is ridiculously hard to implement given how confused I am about the math background involved here. In other news, I've improved the glass fakery, so there's that for progress.

![Alt text](/samplebetterglass.png?raw=true "Sample render")


# November 2nd 2014 - 6:00pm

I've added refraction to the sphere objects; the raytracer can now emulate glass-like surfaces.

![Alt text](/samplerefraction.png?raw=true "Sample render")