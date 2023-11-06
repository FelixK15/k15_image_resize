# What problem is this library trying to solve? (ELI5) #
This library can be used to resize a single image (either upscale or downscale). Additionally, the library will convert between various pixel formats
if the output image pixel format differs from the input image pixel format.

The library supports 3 types of sampling when up-/downscaling:
- nearest neighbor
- bilinear
- bicubic

# How do I add this library to my project? #
This library is a single header library which means that you just have to #include it into your project. However, you have to add the implementation
of the functions used into *one* of your C/CPP files. To do this you have to add **#define K15_IR_IMPLEMENTATION** *before* the #include.  


```c
#define K15_IR_IMPLEMENTATION
#include "k15_image_resize.h"
```

# Customization
You can customize some aspects of the library by adding some #define statements before you include the library.  
**Note:*** You have to add these #define statements in the same header where you place the implementation
using **#define K15_IR_IMPLEMENTATION**.

The library also uses some functions from the C standard library. You can replace these functions calls
with your own functions if you like.

* **K15_IR_MALLOC** - resolves to malloc (must be defined together with K15_IA_FREE)
* **K15_IR_FREE** - resolves to free (must be defined together with K15_IA_MALLOC)
* **K15_IR_MEMCPY** - resolves to memcpy
* **K15_IR_MEMSET** - resolves to memset
* **K15_IR_CEIL** - resolves to ceilf
* **K15_IR_FLOORF** - resolves to floorf
* **K15_IR_SINF** - resolves to sinf
* **K15_IR_ABS** - resolves to abs
* **K15_IR_ALLOCA** - resolves to alloca
* **K15_IR_MIN** - resolves to `((a) < (b) ? (a) : (b))`
* **K15_IR_MAX** - resolves to `((a) > (b) ? (a) : (b))`
* **K15_IR_CLAMP** - resolves to `((a) < (min) ? (min) : (a) > (max) ? (max) : (a))`

Just #define your own functions if you don't want to use the C standard library functions.

```c
#define K15_IR_MALLOC CustomMalloc
#define K15_IR_FREE CustomFree
#define K15_IR_IMPLEMENTATION
#include "k15_image_resize.h"
```

# How does this library work? #
This library works by scaling 2x "along the scanline" to reduce cache misses.
The first scale-step is done along the X axis - the image is then stored 90° rotated.
The second scale-step then also scales along the X axis - but since the image is rotated, it actually scales along the Y axis.
The image is then stored -90° rotated again so that the final image is the correctly oriented scaled result.

# Example Project #
The repository contains an example project - it's currently Win32 only and currently used for testing.  
However, if you want to try it out, just execute the build.bat script (visual studio required) and run the resulting .exe file.

# Basic C example #
```c
int imageX, imageY;
char* image = load_image(&imageX, &imageY);
char* destinationImage = create_empty_image(imageX/2, imageY/2);

K15_IRScaleImageData(image, imageX, imageY, K15_IR_PIXEL_FORMAT_R8A8B8, destinationImage, imageX/2, imageY/2, K15_IR_PIXEL_FORMAT_R8A8B8, K15_IR_WRAP_CLAMP);
```

# License #
This software is in the public domain. You are granted a perpetual, irrevocable license to copy
and modify this file however you want.
