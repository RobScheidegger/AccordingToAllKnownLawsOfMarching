# Project 4: Illuminate

$$\sum_{i=0}^n x_i$$

## Overview

Overall I think my implementation of Illuminate is pretty straight-forward, but here are some points of interest that may be relevant:

1. The additional lighting types have been implented and handled in `raytracer/lighting.cpp`, by handling the different cases when getting the attenuation and light direction.
2. I performed the recursive aspect of my raytracing for reflections/shadows in `lighting.cpp`. If shadows/reflections are enabled in the config, then the necessary raytracing is done to check/compute the reflected pixel as needed. To prevent infinite recursion, I have a `RECURSIVE_DEPTH_LIMIT`, which I have set to 5 (it could probably be lowered for a performance boost without loosing too much detail).
3. Since intersect, I refactored some of by shape code (separating them into `shapes/{shape_name}.{h/cpp}`), and fixed some inefficiencies that I got docked for in intersect (such as not storing the camera inverse view matrix and the matrix for converting normals to world space). These are now computed once and re-used, which should offer a substantial performance improvement.
4. For texture mapping, I load the textures ad-hoc as needed during raytracing, and then cache them in the `RayTracer` class so that they are only loaded into memory once. I chose to load them on-demand since this alleviates the problem of loading all of the textures into the scene, when some won't even end up being visible, while still having the efficiency of loading them exactly once. This is done via the `RayTracer::loadTexture` and `RayTracer::getTexture` functions.
5. Computing the `u,v` values for each of the shapes is done in the `Shape.getTextureMap` function, which is implement for each of the primitive types in the scene. It can be found in `shapes/{shape_name}.cpp`. The color of the texture is found using basic nearest-neighbor texturing in `getTextureColor` in `raytracer/lighting.cpp`.

Overall I actually thought this project was very interesting, and makes me want to do more with raytracing and lighting :).

## Known Bugs

As of now, there are no known bugs in this implementation. I have attempted to check my implementation with most of the benchmark images provided in the `scenefiles` repository, so I am hopeful that I have caught most of the relevant edge cases.

## Extra Credit

I implemented the following extra credit for this this assignment:

### Soft Shadows

As suggested by the handout, to implement this I made it so that my scene could handle `LIGHT_AREA` lights, that take a position (center), direction (determines how the rectangle is angled), height, and width (which determine the size of the rectangular light source).

When it comes to computing the effect of light on a particular point, I sample (deterministically) points in this rectangle every `AREA_LIGHT_INTERVAL` units along the rectangle, and average the light intensity (including illumination) between the given point and every sampled point on the light source. This has the effect of soft shadowing because points where only some of the light source is visible will have less illumination from the light (but still some), so there are no more hard shadow edges.

Admittedly, this does have a pretty large impact on performance (since it causes many more rays to be traced through the scene), but to some extent this is unavoidable if we want this effect, and can be tweaked by changing the `AREA_LIGHT_INTERVAL` constant, as well as changing the size of the actual source.

I have included some test scenes for this in the `scenes/` directory. Note that due to path issues, these can only be rendered _without textures_ (which is fine since we still see the effect of soft shadowing). Tweaking the texture paths makes this work, but I wasn't sure how this would work on the TA side on gradescope, but regardless it should still work if you set the flag `textures` to `false`.

# Project 3: Intersect

## Overview

For my implementation of this project, I can't say that I have done anything that out of the ordinary, but here is a summary of some of the organizational bits of my codebase:

1. I used most of the provided scaffolding, and brought in code from labs 4,5, and 6 for their respective parts of the project.
2. `intersect.{h/cpp}` contains all of my code finding intersections between objects. To achieve proper runtime polymorphism, here I defined a `Shape` abstract class with `intersect` and `getNormal` functions, to find the closest intersection with the shape and the normal at a given point, respectively. From this I created the corresponding `Sphere`, `Cylinder`, `Cube`, and `Cone` classes to implement the specific intersections.
3. `lighting.{h/cpp}` contains all of the lighting computation. Since we only had to implement the phong lighting model (which was from lab 6), I pretty much copied over my lab code and just made a few cosmetic changes (as well as negating the direction of the incoming light).

Other than that, I would say that my implementation is somewhat standard and (hopefully!) easy to follow. Had a great time with this project!

## Known Bugs

Currently, there are no known bugs in the application. I believe I have tested each of the provided scenefiles in the `scenefiles` repo against my implementation and they have produced the correct result based on the reference image.

## Extra Credit

I implemented the following extra credit:

### Parallelization

I opted for the simple method, and just implemented the basic OpenMP directive to paralellize the main render `for` loop in `raytracer.cpp`. This was a pretty simple change that resulted in about an 6x speedup on my laptop while testing (8 core), and so I was very impressed by both the outcome and the simplicity with which you can do it in Open MP!

### Super-sampling

I implemented naive super-sampling in my base render loop, effectively by just shooting more rays through each pixel and averaging over the results. Overall I don't think what I've done here really differs from what is expected, and the relevant code can be found in `raytracer.cpp`.
