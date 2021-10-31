# ShivaVG

See AUTHORS for the list of contributors

ShivaVG is an open-source LGPL ANSI C implementation of the Khronos
Group OpenVG specification.

I.    BUILD

II.   TESTING

III.  IMPLEMENTATION STATUS

IV.   EXTENSIONS


## I. BUILD

### Prerequisites

   OpenGL and GLEW development libraries headers should be installed.
   Now ShivaVG require OpenGL >= 2.0 .
   Other than that, since it's ANSI C99 should compile with any modern
   C compiler (gcc or clang are the best choice).
   jpeglib needs to be installed for example programs that use images.

### Compiling
ShivaVG has now been converted to [CMake](http://www.cmake.org/).

To compile it, obtain CMake from its website, generate project files for your
favorite build system or IDE. <br> It is advisable to do so in a separate `build` 
directory to keep things clean.

`$ mkdir build; cd build`

For example using Make:
`cmake .. -G "Unix Makefiles" && make`

Xcode/OS X:
`$ cmake .. -G Xcode && xcodebuild`

For Windows, generate Visual Studio project files with the CMake GUI.
You can customize the name of the shared and static library by editing
the cmake variables `SHARED_LIBRARY_NAME` and `STATIC_LIBRARY_NAME`
(by default are equal to 'OpenVG' and 'OpenVGStatic')

#### Static Analysis
To run the Clang Static Analyzer with CMake, obtain the latest analyzer from its [website](http://clang-analyzer.llvm.org/) and unpack it somewhere.


To tell CMake to use the Analyzer and check the code, do

```
    $ export CCC_CC=clang
    $ export CCC_CXX=clang++
    $ scan-build cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -G "Unix Makefiles" ..
    $ scan-build make

```


## II. TESTING

There is no real testing suite yet. The example programs are there
just to play with what the implementation can currently do, but
can hardly provide any proper validation, since no reference images
are provided. Here is a description of each example program and
what features it highlights:

* *test_vgu*
  Constructs some path primitives using the VGU API.

* *test_tiger*
  The most simple performance test. It draws the well known svg
  tiger using just simple stroke and fill of solid colors. It
  consists of 240 paths.

* *test_dash*
   Shows different stroke dashing modes.

* *test_linear*
  A rectangle drawn using 3-color linear gradient fill paint

* *test_radial*
  A rectangle drawn using 3-color radial gradient fill paint

* *test_interpolate*
  Interpolates between two paths - an apple and a pear.

* *test_image*
  Images are drawn using `VG_DRAW_IMAGE_MULTIPLY` image mode to be
  multiplied with radial gradient fill paint.

* *test_pattern*
  An image is drawn in multiply mode with an image pattern fill
  paint.

* *test_warp*
  An image is drawn using Warp effect.

* *test_clock*
  A simple clock example

* *test_flowers*
  Flowers fall drawn using `VG_BLEND_ADDITIVE`

* *test_subwaymap*
  A complex stroking example

* *test_colormatrix*
  A simple example of color matrix usage

* *test_gaussianblur*
  A simple example of gaussian blur usage

* *test_gaussianblur2*
  Another example of gaussian blur usage

* *test_convolve*
  A simple example of vgConvolve usage

* *test_separableconvolve*
  A simple example of vgSeparableConvolve usage

* *test_lookup*
  A simple example of vgLookup usage

* *test_lookup_single*
  A simple example of vgLookupSingle usage

## III. IMPLEMENTATION STATUS

Khronos states in the OpenVG specification, that the contexts for all
their client APIs are expected to be created via the EGL API. Since
EGL to use with ShivaVG has not been implemented yet, there is a set
of extension functions provided for the task of creating, maintaining
and destroying the OpenVG context. (See next section EXTENSIONS for
details.)

What follows is a description of which functions or to what extent
a certain function has been implemented. When a function is marked
as PARTIALLY implemented, the TODO file or the comments in the code
itself would provide further clues.


* General:

```
vgGetError ............................ FULLY implemented
vgFlush ............................... FULLY implemented
vgFinish .............................. FULLY implemented
```

* Getters and setters:

```
vgSet ................................. FULLY implemented
vgSeti ................................ FULLY implemented
vgSetfv ............................... FULLY implemented
vgSetiv ............................... FULLY implemented
vgGetf ................................ FULLY implemented
vgGeti ................................ FULLY implemented
vgGetVectorSize ....................... FULLY implemented
vgGetfv ............................... FULLY implemented
vgGetiv ............................... FULLY implemented
vgSetParameterf ....................... FULLY implemented
vgSetParameteri ....................... FULLY implemented
vgSetParameterfv ...................... FULLY implemented
vgSetParameteriv ...................... FULLY implemented
vgGetParameterf ....................... FULLY implemented
vgGetParameteri ....................... FULLY implemented
vgGetParameterVectorSize............... FULLY implemented
vgGetParameterfv ...................... FULLY implemented
vgGetParameteriv ...................... FULLY implemented
```

* Matrix Manipulation:

```
vgLoadIdentity ........................ FULLY implemented
vgLoadMatrix .......................... FULLY implemented
vgGetMatrix ........................... FULLY implemented
vgMultMatrix .......................... FULLY implemented
vgTranslate ........................... FULLY implemented
vgScale ............................... FULLY implemented
vgShear ............................... FULLY implemented
vgRotate .............................. FULLY implemented
```

* Masking and Clearing:

```
vgMask ................................ NOT implemented
vgClear ............................... FULLY implemented
```

* Paths:

```
vgCreatePath .......................... FULLY implemented
vgClearPath ........................... FULLY implemented
vgDestroyPath ......................... FULLY implemented
vgRemovePathCapabilities .............. FULLY implemented
vgGetPathCapabilities ................. FULLY implemented
vgAppendPath .......................... FULLY implemented
vgAppendPathData ...................... FULLY implemented
vgModifyPathCoords .................... FULLY implemented
vgTransformPath ....................... FULLY implemented
vgInterpolatePath ..................... FULLY implemented
vgPathLength .......................... FULLY implemented
vgPointAlongPath ...................... NOT implemented
vgPathBounds .......................... FULLY implemented
vgPathTransformedBounds ............... FULLY implemented
vgDrawPath ............................ PARTIALLY implemented
```

* Paint:

```
vgCreatePaint ......................... FULLY implemented
vgDestroyPaint ........................ FULLY implemented
vgSetPaint ............................ FULLY implemented
vgGetPaint ............................ FULLY implemented
vgSetColor ............................ FULLY implemented
vgGetColor ............................ FULLY implemented
vgPaintPattern ........................ FULLY implemented
```

* Images:

```
vgCreateImage ......................... PARTIALLY implemented
vgDestroyImage ........................ FULLY implemented
vgClearImage .......................... FULLY implemented
vgImageSubData ........................ PARTIALLY implemented
vgGetImageSubData ..................... PARTIALLY implemented
vgChildImage .......................... NOT implemented
vgGetParent ........................... NOT implemented
vgCopyImage ........................... FULLY implemented
vgDrawImage ........................... PARTIALLY implemented
vgSetPixels ........................... FULLY implemented
vgWritePixels ......................... FULLY implemented
vgGetPixels ........................... FULLY implemented
vgReadPixels .......................... FULLY implemented
vgCopyPixels .......................... FULLY implemented
```

* Image Filters:

```
vgColorMatrix ......................... PARTIALLY implemented
vgConvolve ............................ PARTIALLY implemented
vgSeparableConvolve ................... PARTIALLY implemented
vgGaussianBlur ........................ PARTIALLY implemented
vgLookup .............................. PARTIALLY implemented
vgLookupSingle ........................ PARTIALLY implemented
```

* Hardware Queries:

```
vgHardwareQuery ....................... NOT implemented
```

* Renderer and Extension Information:

```
vgGetString ........................... FULLY implemented
```

* VGU

```
vguLine ............................... FULLY implemented
vguPolygon ............................ FULLY implemented
vguRect ............................... FULLY implemented
vguRoundRect .......................... FULLY implemented
vguEllipse ............................ FULLY implemented
vguArc ................................ FULLY implemented
vguComputeWarpQuadToSquare ............ FULLY implemented
vguComputeWarpSquareToQuad ............ FULLY implemented
vguComputeWarpQuadToQuad .............. FULLY implemented
```

## IV. EXTENSIONS

There are three extensions to the API that manipulate the OpenVG
context as a temporary replacement for EGL:

* `VGboolean vgCreateContextSH(VGint width, VGint height)`
  Creates an OpenVG context on top of an existing OpenGL context
  that should have been manually initialized by the user of the
  library. Width and height specify the size of the rendering
  surface. No multi-threading support has been implemented yet.
  The context is created once per process.

* `void vgResizeSurfaceSH(VGint width, VGint height)`
  Should be called whenever the size of the surface changes (e.g.
  the owner window of the OpenGL context is resized).

* `void vgDestroyContextSH()`
  Destroys the OpenVG context associated with the calling process.
