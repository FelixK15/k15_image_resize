/*
K15 Image Resize v 1.0
	Single header public domain library

# Author(s):
	Felix Klinge (f dot klinge15 dot gmail dot com)

# Version history


# What problem is this library trying to solve? (ELI5)


# How do I add this library to my project?
	This library is a single header library which means that you just have to
	#include it into your project. However, you have to add the implementation
	of the functions used into *one* of your C/CPP files. To do this you have
	to add #define K15_IA_IMPLEMENTATION *before* the #include.

# How does this library work?
	

# Example Usage
	

# Hints


# TODO


# License:
	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy
	and modify this file however you want.
*/

#ifndef _K15_ImageResize_h_
#define _K15_ImageResize_h_

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#ifndef K15_IR_STATIC
# define kir_def static
#else
# define kir_def extern
#endif //K15_IA_STATIC

typedef signed 		int 	kir_s32;
typedef unsigned 	int 	kir_u32;
typedef signed 		short 	kir_s16;
typedef unsigned 	short 	kir_u16;

typedef unsigned char kir_u8;
typedef unsigned char kir_b8;
typedef unsigned char kir_byte;

typedef enum 
{
	K15_IR_RESULT_SUCCESS 		= 0,
	K15_IR_RESULT_OUT_OF_MEMORY = 1
} kir_result;

typedef enum 
{
	K15_IR_PIXEL_FORMAT_R8 			= 0,
	K15_IR_PIXEL_FORMAT_R8A8 		= 1,
	K15_IR_PIXEL_FORMAT_R8A8B8 		= 2,
	K15_IR_PIXEL_FORMAT_R8A8B8A8 	= 3
} kir_pixel_format;

typedef enum 
{
	K15_IR_WRAP_CLAMP 	= 0,
	K15_IR_WRAP_REPEAT	= 1,
	K15_IR_WRAP_MIRROR 	= 2
} kir_wrap_mode;

typedef enum 
{
	K15_IR_NEAREST_NEIGHBOUR_FILTER_MODE	= 0,
	K15_IR_BILINEAR_FILTER_MODE				= 1,
	K15_IR_BICUBIC_FILTER_MODE				= 2
} kir_filter_mode;

kir_result K15_IRScaleImageDataWithCustomMemory(unsigned char* pCustomMemory, unsigned int customMemorySizeInBytes, 
	const unsigned char* pSourceImageData, unsigned int sourceImageWidth, unsigned int sourceImageHeight, kir_pixel_format sourcePixelFormat,
	unsigned char* pDestinationImageData, unsigned int destinationWidth, unsigned int destinationHeight, kir_pixel_format destinationPixelFormat,
	kir_wrap_mode wrapMode, kir_filter_mode filterMode);

kir_result K15_IRScaleImageData(const unsigned char* pSourceImageData, unsigned int sourceImageWidth, unsigned int sourceImageHeight, kir_pixel_format sourcePixelFormat,
	unsigned char* pDestinationImageData, unsigned int destinationWidth, unsigned int destinationHeight, kir_pixel_format destinationPixelFormat,
	kir_wrap_mode wrapMode, kir_filter_mode filterMode);

#ifdef K15_IR_IMPLEMENTATION

#define K15_IR_TRUE 1
#define K15_IR_FALSE 0

#ifndef K15_IR_MALLOC
# include <stdlib.h>
# define K15_IR_MALLOC malloc
# define K15_IR_FREE free
#endif //K15_IR_MALLOC

#ifndef K15_IR_MEMCPY
# include <string.h>
# define K15_IR_MEMCPY memcpy
#endif //K15_IR_MEMCPY

#ifndef K15_IR_MEMSET
# include <string.h>
# define K15_IR_MEMSET memset
#endif //K15_IR_MEMSET

#ifndef K15_IR_MEMMOVE
# include <string.h>
# define K15_IR_MEMMOVE memmove
#endif //K15_IR_MEMMOVE

#ifndef K15_IR_CEIL
# include <math.h>
# define K15_IR_CEILF ceilf
#endif //K15_IR_CEILF

#ifndef K15_IR_FLOORF
# include <math.h>
# define K15_IR_FLOORF floorf
#endif //K15_IR_FLOORF

#ifndef K15_IR_SINF
# include <math.h>
# define K15_IR_SINF sinf
#endif //K15_IR_SINF

#ifndef K15_IR_ABS
# include <math.h>
# define K15_IR_ABS abs
#endif //K15_IR_ABS

#ifndef K15_IR_ALLOCA
# include <malloc.h>
# define K15_IR_ALLOCA alloca
#endif //K15_IR_ALLOCA

#ifndef K15_IR_MIN
# define K15_IR_MIN(a,b) ((a) < (b) ? (a) : (b))
#endif //K15_IR_MIN

#ifndef K15_IR_MAX
# define K15_IR_MAX(a,b) ((a) > (b) ? (a) : (b))
#endif //K15_IR_MAX

#ifndef K15_IR_CLAMP
# define K15_IR_CLAMP(a, min, max) ((a) < (min) ? (min) : (a) > (max) ? (max) : (a))
#endif //K15_IR_CLAMP

#ifndef kir_internal
# define kir_internal static
#endif //kir_internal

kir_internal kir_u32 kir_format_to_byte_lut[] = {
	1, //K15_KIR_PIXEL_FORMAT_R8
	2, //K15_KIR_PIXEL_FORMAT_R8G8
	3, //K15_KIR_PIXEL_FORMAT_R8G8B8
	4  //K15_KIR_PIXEL_FORMAT_R8G8B8A8
};

typedef struct
{
	kir_u32 i1;
	kir_u32 i2;
	float t;
} kir_bilinear_index;

typedef struct 
{
	kir_u32 i1;
	kir_u32 i2;
	kir_u32 i3;
	kir_u32 i4;
	float t;
} kir_bicubic_index;

typedef struct
{
	const kir_u8*		pSourcePixels;
	kir_u8*				pDestinationPixels;
	kir_u8*				pTranscodedPixels;
	kir_filter_mode		filter;
	kir_pixel_format	sourceFormat;
	kir_pixel_format	destinationFormat;
	kir_wrap_mode		wrapMode;
	kir_u32				sourceWidth;
	kir_u32				sourceHeight;
	kir_u32				destinationWidth;
	kir_u32				destinationHeight;
} kir_resize_context;

kir_internal kir_u8 _K15_IRCubicHermiteU8(float factor, kir_u8 value1, kir_u8 value2, kir_u8 value3, kir_u8 value4)
{
	const float A = value1;
	const float B = value2;
	const float C = value3;
	const float D = value4;
	const float t = factor;

	//http://blog.demofox.org/2015/08/15/resizing-images-with-bicubic-interpolation/
	const float a = -A / 2.0f + (3.0f*B) / 2.0f - (3.0f*C) / 2.0f + D / 2.0f;
    const float b = A - (5.0f*B) / 2.0f + 2.0f*C - D / 2.0f;
    const float c = -A / 2.0f + C / 2.0f;
    const float d = B;
 
	float r = a*t*t*t + b*t*t + c*t + d;

	r = K15_IR_CLAMP(r, 0.f, 255.f);

	return (kir_u8)r;
}

kir_internal kir_u8 _K15_IRLerpU8(float factor, kir_u8 startValue, kir_u8 endValue)
{
	return (kir_u8)((float)startValue * (1.f - factor) + ((float)endValue * factor));
}

kir_internal void _K15_IRReadPixelFromIndex(kir_u32 pixelIndex, kir_pixel_format pixelFormat, const kir_u8* pPixels, kir_u8* pOutputPixels)
{
	const kir_u32 bpp = kir_format_to_byte_lut[pixelFormat];
	K15_IR_MEMCPY(pOutputPixels, pPixels + pixelIndex * bpp, bpp);
}

kir_internal void _K15_IRWritePixelToIndex(kir_u32 pixelIndex, kir_pixel_format pixelFormat, kir_u8* pPixels, const kir_u8* pInputPixels)
{
	const kir_u32 bpp = kir_format_to_byte_lut[pixelFormat];
	K15_IR_MEMCPY(pPixels + pixelIndex * bpp, pInputPixels, bpp);
}

kir_internal void _K15_IRTranscodeFromR8ToR8G8(const kir_u8* pInputPixels, kir_u8* pOutputPixels, kir_u32 width, kir_u32 height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 255;

	for (posY = 0; posY < height; ++posY)
	{
		for (posX = 0; posX < width; ++posX)
		{
			pixelIndex = posX + (posY * width);
			r = pInputPixels[pixelIndex];

			pOutputPixels[pixelIndex * 2 + 0] = r;
			pOutputPixels[pixelIndex * 2 + 1] = g;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8ToR8G8B8(const kir_u8* pInputPixels, kir_u8* pOutputPixels, kir_u32 width, kir_u32 height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;

	for (posY = 0; posY < height; ++posY)
	{
		for (posX = 0; posX < width; ++posX)
		{
			pixelIndex = posX + (posY * width);
			r = pInputPixels[pixelIndex];

			pOutputPixels[pixelIndex * 3 + 0] = r;
			pOutputPixels[pixelIndex * 3 + 1] = r;
			pOutputPixels[pixelIndex * 3 + 2] = r;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8ToR8G8B8A8(const kir_u8* pInputPixels, kir_u8* pOutputPixels, kir_u32 width, kir_u32 height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 a = 255;

	for (posY = 0; posY < height; ++posY)
	{
		for (posX = 0; posX < width; ++posX)
		{
			pixelIndex = posX + (posY * width);
			r = pInputPixels[pixelIndex];

			pOutputPixels[pixelIndex * 4 + 0] = r;
			pOutputPixels[pixelIndex * 4 + 1] = r;
			pOutputPixels[pixelIndex * 4 + 2] = r;
			pOutputPixels[pixelIndex * 4 + 3] = a;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8G8ToR8(const kir_u8* pInputPixels, kir_u8* pOutputPixels, kir_u32 width, kir_u32 height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 0;

	float coverage = 0.f;

	for (posY = 0; posY < height; ++posY)
	{
		for (posX = 0; posX < width; ++posX)
		{
			pixelIndex = posX + (posY * width);
			r = pInputPixels[pixelIndex * 2 + 0];
			g = pInputPixels[pixelIndex * 2 + 1];

			coverage = (float)g / 255.f;

			r = (kir_u8)((float)r * coverage + 0.5f);

			pOutputPixels[pixelIndex] = r;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8G8ToR8G8B8(const kir_u8* pInputPixels, kir_u8* pOutputPixels, kir_u32 width, kir_u32 height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 0;

	float coverage = 0.f;

	for (posY = 0; posY < height; ++posY)
	{
		for (posX = 0; posX < width; ++posX)
		{
			pixelIndex = posX + (posY * width);
			r = pInputPixels[pixelIndex * 2 + 0];
			g = pInputPixels[pixelIndex * 2 + 1];

			coverage = (float)g / 255.f;

			r = (kir_u8)((float)r * coverage + 0.5f);

			pOutputPixels[pixelIndex * 3 + 0] = r;
			pOutputPixels[pixelIndex * 3 + 1] = r;
			pOutputPixels[pixelIndex * 3 + 2] = r;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8G8ToR8G8B8A8(const kir_u8* pInputPixels, kir_u8* pOutputPixels, kir_u32 width, kir_u32 height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 0;
	kir_u8 b = 0;
	kir_u8 a = 0;

	for (posY = 0; posY < height; ++posY)
	{
		for (posX = 0; posX < width; ++posX)
		{
			pixelIndex = posX + (posY * width);
			r = pInputPixels[pixelIndex * 2 + 0];
			a = pInputPixels[pixelIndex * 2 + 1];

			pOutputPixels[pixelIndex * 4 + 0] = r;
			pOutputPixels[pixelIndex * 4 + 1] = r;
			pOutputPixels[pixelIndex * 4 + 2] = r;
			pOutputPixels[pixelIndex * 4 + 3] = a;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8G8B8ToR8(const kir_u8* pInputPixels, kir_u8* pOutputPixels, kir_u32 width, kir_u32 height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 0;
	kir_u8 b = 0;

	kir_u8 luminance = 0;

	for (posY = 0; posY < height; ++posY)
	{
		for (posX = 0; posX < width; ++posX)
		{
			pixelIndex = posX + (posY * width);
			r = pInputPixels[pixelIndex * 3 + 0];
			g = pInputPixels[pixelIndex * 3 + 1];
			b = pInputPixels[pixelIndex * 3 + 2];

			luminance = (kir_u8)((float)r * 0.21f + (float)g * 0.72f + (float)b * 0.07f);

			pOutputPixels[pixelIndex] = luminance;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8G8B8ToR8G8(const kir_u8* pInputPixels, kir_u8* pOutputPixels, kir_u32 width, kir_u32 height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 0;
	kir_u8 b = 0;
	kir_u8 a = 255;

	kir_u8 luminance = 0;

	for (posY = 0; posY < height; ++posY)
	{
		for (posX = 0; posX < width; ++posX)
		{
			pixelIndex = posX + (posY * width);
			r = pInputPixels[pixelIndex * 3 + 0];
			g = pInputPixels[pixelIndex * 3 + 1];
			b = pInputPixels[pixelIndex * 3 + 2];

			luminance = (kir_u8)((float)r * 0.21f + (float)g * 0.72f + (float)b * 0.07f);

			pOutputPixels[pixelIndex * 2 + 0] = luminance;
			pOutputPixels[pixelIndex * 2 + 1] = a;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8G8B8ToR8G8B8A8(const kir_u8* pInputPixels, kir_u8* pOutputPixels, kir_u32 width, kir_u32 height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 0;
	kir_u8 b = 0;
	kir_u8 a = 255;

	for (posY = 0; posY < height; ++posY)
	{
		for (posX = 0; posX < width; ++posX)
		{
			pixelIndex = posX + (posY * width);
			r = pInputPixels[pixelIndex * 3 + 0];
			g = pInputPixels[pixelIndex * 3 + 1];
			b = pInputPixels[pixelIndex * 3 + 2];

			pOutputPixels[pixelIndex * 4 + 0] = r;
			pOutputPixels[pixelIndex * 4 + 1] = g;
			pOutputPixels[pixelIndex * 4 + 2] = b;
			pOutputPixels[pixelIndex * 4 + 3] = a;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8G8B8A8ToR8(const kir_u8* pInputPixels, kir_u8* pOutputPixels, kir_u32 width, kir_u32 height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 0;
	kir_u8 b = 0;
	kir_u8 a = 0;
	kir_u8 luminance = 0;

	float coverage = 0.f;

	for (posY = 0; posY < height; ++posY)
	{
		for (posX = 0; posX < width; ++posX)
		{
			pixelIndex = posX + (posY * width);
			r = pInputPixels[pixelIndex * 4 + 0];
			g = pInputPixels[pixelIndex * 4 + 1];
			b = pInputPixels[pixelIndex * 4 + 2];
			a = pInputPixels[pixelIndex * 4 + 3];

			coverage = (float)a / 255.f;

			luminance = (kir_u8)(coverage * ((float)r * 0.21f + (float)g * 0.72f + (float)b * 0.07f));

			pOutputPixels[pixelIndex] = luminance;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8G8B8A8ToR8G8(const kir_u8* pInputPixels, kir_u8* pOutputPixels, kir_u32 width, kir_u32 height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 0;
	kir_u8 b = 0;
	kir_u8 a = 0;
	kir_u8 luminance = 0;

	for (posY = 0; posY < height; ++posY)
	{
		for (posX = 0; posX < width; ++posX)
		{
			pixelIndex = posX + (posY * width);
			r = pInputPixels[pixelIndex * 4 + 0];
			g = pInputPixels[pixelIndex * 4 + 1];
			b = pInputPixels[pixelIndex * 4 + 2];
			a = pInputPixels[pixelIndex * 4 + 3];

			luminance = (kir_u8)((float)r * 0.21f + (float)g * 0.72f + (float)b * 0.07f);

			pOutputPixels[pixelIndex * 2 + 0] = luminance;
			pOutputPixels[pixelIndex * 2 + 1] = a;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8G8B8A8ToR8G8B8(const kir_u8* pInputPixels, kir_u8* pOutputPixels, kir_u32 width, kir_u32 height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 0;
	kir_u8 b = 0;
	kir_u8 a = 0;

	float coverage = 0.f;

	for (posY = 0; posY < height; ++posY)
	{
		for (posX = 0; posX < width; ++posX)
		{
			pixelIndex = posX + (posY * width);
			r = pInputPixels[pixelIndex * 4 + 0];
			g = pInputPixels[pixelIndex * 4 + 1];
			b = pInputPixels[pixelIndex * 4 + 2];
			a = pInputPixels[pixelIndex * 4 + 3];

			coverage = (float)a / 255.f;

			r = (kir_u8)((float)r * coverage); 
			g = (kir_u8)((float)g * coverage); 
			b = (kir_u8)((float)b * coverage); 

			pOutputPixels[pixelIndex * 3 + 0] = r;
			pOutputPixels[pixelIndex * 3 + 1] = g;
			pOutputPixels[pixelIndex * 3 + 2] = b;
		}
	}
}


kir_internal void _K15_IRTranscodeSourcePixelsToDestinationPixelFormat(kir_resize_context* pContext)
{
	kir_pixel_format sourceFormat = pContext->sourceFormat;
	kir_pixel_format destinationFormat = pContext->destinationFormat;

	//FK: check if we actually have to transcode pixels
	if (sourceFormat != destinationFormat)
	{
		kir_u8* pTranscodedPixels = ( kir_u8* )K15_IR_MALLOC( pContext->sourceWidth * pContext->sourceHeight * kir_format_to_byte_lut[destinationFormat] );

		if (sourceFormat == K15_IR_PIXEL_FORMAT_R8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8A8)
			_K15_IRTranscodeFromR8ToR8G8(pContext->pSourcePixels, pTranscodedPixels, pContext->sourceWidth, pContext->sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8A8B8)
			_K15_IRTranscodeFromR8ToR8G8B8(pContext->pSourcePixels, pTranscodedPixels, pContext->sourceWidth, pContext->sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8A8B8A8)
			_K15_IRTranscodeFromR8ToR8G8B8A8(pContext->pSourcePixels, pTranscodedPixels, pContext->sourceWidth, pContext->sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8A8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8)
			_K15_IRTranscodeFromR8G8ToR8(pContext->pSourcePixels, pTranscodedPixels, pContext->sourceWidth, pContext->sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8A8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8A8B8)
			_K15_IRTranscodeFromR8G8ToR8G8B8(pContext->pSourcePixels, pTranscodedPixels, pContext->sourceWidth, pContext->sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8A8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8A8B8A8)
			_K15_IRTranscodeFromR8G8ToR8G8B8A8(pContext->pSourcePixels, pTranscodedPixels, pContext->sourceWidth, pContext->sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8A8B8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8)
			_K15_IRTranscodeFromR8G8B8ToR8(pContext->pSourcePixels, pTranscodedPixels, pContext->sourceWidth, pContext->sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8A8B8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8A8)
			_K15_IRTranscodeFromR8G8B8ToR8G8(pContext->pSourcePixels, pTranscodedPixels, pContext->sourceWidth, pContext->sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8A8B8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8A8B8A8)
			_K15_IRTranscodeFromR8G8B8ToR8G8B8A8(pContext->pSourcePixels, pTranscodedPixels, pContext->sourceWidth, pContext->sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8A8B8A8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8)
			_K15_IRTranscodeFromR8G8B8A8ToR8(pContext->pSourcePixels, pTranscodedPixels, pContext->sourceWidth, pContext->sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8A8B8A8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8A8)
			_K15_IRTranscodeFromR8G8B8A8ToR8G8(pContext->pSourcePixels, pTranscodedPixels, pContext->sourceWidth, pContext->sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8A8B8A8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8A8B8)
			_K15_IRTranscodeFromR8G8B8A8ToR8G8B8(pContext->pSourcePixels, pTranscodedPixels, pContext->sourceWidth, pContext->sourceHeight);

		pContext->pTranscodedPixels = pTranscodedPixels;
	}
}

kir_internal kir_u32 _K15_IRGetWrappedCoordinate(kir_s32 pos, 
	kir_s32 p_Threshold, kir_wrap_mode wrapMode)
{
	if (pos < 0)
	{
		if (wrapMode == K15_IR_WRAP_CLAMP)
			return 0;
		else if (wrapMode == K15_IR_WRAP_REPEAT)
		{
			kir_u16 absPos = K15_IR_ABS(pos);
			absPos %= p_Threshold;
			return (p_Threshold - absPos);
		}
	}
	else if (pos >= p_Threshold)
	{
		if (wrapMode == K15_IR_WRAP_CLAMP)
			return p_Threshold - 1;
		else if (wrapMode == K15_IR_WRAP_REPEAT)
		{
			pos %= p_Threshold;
			return (pos);
		}
	}

	return pos;
}

kir_internal kir_u32 _K15_IRGetWrappedIndex(kir_s32 posX, kir_s32 posY, 
	kir_u32 width, kir_u32 height, kir_wrap_mode wrapMode)
{
	posX = _K15_IRGetWrappedCoordinate(posX, width, wrapMode);
	posY = _K15_IRGetWrappedCoordinate(posY, height, wrapMode);

	return (posX + (posY * width));
}

kir_internal void _K15_IRSampleNearestNeighbour(const kir_u8* pSourcePixels, kir_u8* pDestinationPixels, 
	kir_u32 sourceWidth, kir_u32 sourceHeight, kir_u32 destinationWidth, kir_u32 destinationHeight,
	kir_pixel_format pixelFormat)
{
	kir_u32 sourcePosX = 0;
	kir_u32 sourcePosY = 0;

	kir_u32 destinationPosX = 0;
	kir_u32 destinationPosY = 0;

	float u = 0.f;
	float v = 0.f;

	kir_u32 sourcePixelIndex = 0;
	kir_u32 destinationPixelIndex = 0;

	kir_u32 bpp = kir_format_to_byte_lut[pixelFormat];
	kir_u8* pixelBuffer = (kir_u8*)K15_IR_ALLOCA(bpp);

	for (destinationPosY = 0; destinationPosY < destinationHeight; ++destinationPosY)
	{
		v = (float)destinationPosY / (float)destinationHeight;
		sourcePosY = (kir_u32)(v * (float)sourceHeight);
		
		for (destinationPosX = 0; destinationPosX < destinationWidth; ++destinationPosX)
		{
			u = (float)destinationPosX / (float)destinationWidth;

			sourcePosX = (kir_u32)(u * (float)sourceWidth);

			sourcePixelIndex = sourcePosX + (sourcePosY * sourceWidth);
			destinationPixelIndex = destinationPosX + (destinationPosY * destinationWidth);
			
			_K15_IRReadPixelFromIndex(sourcePixelIndex, pixelFormat, pSourcePixels, pixelBuffer);
			_K15_IRWritePixelToIndex(destinationPixelIndex, pixelFormat, pDestinationPixels, pixelBuffer);
		}	
	}
}

kir_internal void _K15_IRCalculateBilinearIndexTableForScanline(kir_u32 sourceWidth, kir_u32 destinationWidth, 
	kir_wrap_mode wrapMode, kir_bilinear_index* indexTable)
{
	kir_u32 sourcePosX = 0;
	kir_u32 destinationPosX = 0;
	kir_u32 indexTablePos = 0;

	float sourcePosXFract = 0.f;
	float u = 0.f;
	float t = 0.f;

	for (destinationPosX = 0; destinationPosX < destinationWidth; ++destinationPosX, ++indexTablePos)
	{
		u = (float)destinationPosX / (float)destinationWidth;
		sourcePosXFract = (u * (float)sourceWidth);
		sourcePosX = (kir_u32)K15_IR_FLOORF(sourcePosXFract);
		t = sourcePosXFract - (float)sourcePosX;

		indexTable[indexTablePos].i1 = _K15_IRGetWrappedCoordinate(sourcePosX + 0, sourceWidth, wrapMode); 
		indexTable[indexTablePos].i2 = _K15_IRGetWrappedCoordinate(sourcePosX + 1, sourceWidth, wrapMode); 
		indexTable[indexTablePos].t  = t;
	} 
}

kir_internal void _K15_IRHorizontallySampleBilinear(const kir_u8* pSourcePixels, kir_u8* pDestinationPixels,
	kir_u16 sourceWidth, kir_u16 sourceHeight, kir_u16 destinationWidth, kir_u16 destinationHeight,
	kir_pixel_format pixelFormat, kir_wrap_mode wrapMode)
{
	kir_u32 sourcePosY = 0;

	kir_u32 destinationPosX = 0;
	kir_u32 destinationPosY = 0;

	kir_u32 sourcePixelIndex1 = 0;
	kir_u32 sourcePixelIndex2 = 0;

	float t = 0.f;

	kir_u32 destinationPixelIndex = 0;
	kir_u32 componentIndex = 0;

	kir_u32 bpp = kir_format_to_byte_lut[pixelFormat];
	kir_u8* inputPixelBuffer1 = (kir_u8*)K15_IR_ALLOCA(bpp);
	kir_u8* inputPixelBuffer2 = (kir_u8*)K15_IR_ALLOCA(bpp);
	kir_u8* outputPixelBuffer = (kir_u8*)K15_IR_ALLOCA(bpp);

	kir_u32 scanlineSizeInBytes = bpp * sourceWidth;
	kir_u32 scanlineOffsetInBytes = 0;
	kir_u8* scanlineBuffer = (kir_u8*)K15_IR_MALLOC(scanlineSizeInBytes);

	//FK: There'll be one entry for each horizontal destination pixel.
	kir_u32 numIndexTableEntries			= destinationWidth;
	kir_u32 indexTableSizeInBytes			= numIndexTableEntries * sizeof(kir_bilinear_index);
	kir_u32 indexTablePos					= 0;
	kir_bilinear_index* scanlineIndexTable	= (kir_bilinear_index*)K15_IR_MALLOC(indexTableSizeInBytes);

	//FK: Fill the index table. Basically for each horizontal destination pixel, the
	//	  2 contribution source pixels together with the blend factor is calculated.
	_K15_IRCalculateBilinearIndexTableForScanline(sourceWidth, destinationWidth, wrapMode, scanlineIndexTable);

	for (sourcePosY = 0; sourcePosY < sourceHeight; ++sourcePosY, ++destinationPosY)
	{
		//FK: Get scan line for current Y position.
		scanlineOffsetInBytes = sourcePosY * sourceWidth * bpp;		
		K15_IR_MEMCPY(scanlineBuffer, pSourcePixels + scanlineOffsetInBytes, scanlineSizeInBytes);

		for (destinationPosX = 0, indexTablePos = 0; destinationPosX < destinationWidth; ++destinationPosX, ++indexTablePos)
		{
			//FK: Calculate index where we write the interpolated pixel to
			destinationPixelIndex = (destinationPosY + (destinationPosX * destinationHeight));

			sourcePixelIndex1 = scanlineIndexTable[indexTablePos].i1;
			sourcePixelIndex2 = scanlineIndexTable[indexTablePos].i2;
			t = scanlineIndexTable[indexTablePos].t;

			//FK: Read pixels to interpolate
			_K15_IRReadPixelFromIndex(sourcePixelIndex1, pixelFormat, scanlineBuffer, inputPixelBuffer1);
			_K15_IRReadPixelFromIndex(sourcePixelIndex2, pixelFormat, scanlineBuffer, inputPixelBuffer2);

			//FK: Read pixel to accumulate - add interpolated value to existing pixel values 
			_K15_IRReadPixelFromIndex(destinationPixelIndex, pixelFormat, pDestinationPixels, outputPixelBuffer);

			for (componentIndex = 0; componentIndex < bpp; ++componentIndex)
			{
				kir_u8 lerpedComponent = _K15_IRLerpU8(t, inputPixelBuffer1[componentIndex], inputPixelBuffer2[componentIndex]);
				outputPixelBuffer[componentIndex] += lerpedComponent;
			}
			_K15_IRWritePixelToIndex(destinationPixelIndex, pixelFormat, pDestinationPixels, outputPixelBuffer);
		}
	}
}

kir_internal void _K15_IRCalculateBicubicIndexTableForScanline(kir_u32 sourceWidth, 
	kir_u32 destinationWidth, kir_wrap_mode wrapMode, kir_bicubic_index* indexTable)
{
	kir_u32 sourcePosX = 0;
	kir_u32 destinationPosX = 0;
	kir_u32 indexTablePos = 0;

	float sourcePosXFract = 0.f;
	float u = 0.f;
	float t = 0.f;

	for (destinationPosX = 0; destinationPosX < destinationWidth; ++destinationPosX, ++indexTablePos)
	{
		u = (float)destinationPosX / (float)destinationWidth;
		sourcePosXFract = (u * (float)sourceWidth);
		sourcePosX = (kir_u32)K15_IR_FLOORF(sourcePosXFract);
		t = sourcePosXFract - (float)sourcePosX;

		indexTable[indexTablePos].i1 = _K15_IRGetWrappedCoordinate(sourcePosX - 1, sourceWidth, wrapMode); 
		indexTable[indexTablePos].i2 = _K15_IRGetWrappedCoordinate(sourcePosX + 0, sourceWidth, wrapMode); 
		indexTable[indexTablePos].i3 = _K15_IRGetWrappedCoordinate(sourcePosX + 1, sourceWidth, wrapMode); 
		indexTable[indexTablePos].i4 = _K15_IRGetWrappedCoordinate(sourcePosX + 2, sourceWidth, wrapMode); 
		indexTable[indexTablePos].t  = t;
	} 
}

kir_internal void _K15_IRHorizontallySampleBicubic(const kir_u8* pSourcePixels, kir_u8* pDestinationPixels,
	kir_u16 sourceWidth, kir_u16 sourceHeight, kir_u16 destinationWidth, kir_u16 destinationHeight,
	kir_pixel_format pixelFormat, kir_wrap_mode wrapMode)
{
	kir_u32 sourcePosY = 0;

	kir_u32 destinationPosX = 0;
	kir_u32 destinationPosY = 0;

	kir_u32 sourcePixelIndex1 = 0;
	kir_u32 sourcePixelIndex2 = 0;
	kir_u32 sourcePixelIndex3 = 0;
	kir_u32 sourcePixelIndex4 = 0;

	float t = 0.f;

	kir_u32 destinationPixelIndex = 0;
	kir_u32 componentIndex = 0;

	kir_u32 bpp = kir_format_to_byte_lut[pixelFormat];
	kir_u8* inputPixelBuffer1 = (kir_u8*)K15_IR_ALLOCA(bpp);
	kir_u8* inputPixelBuffer2 = (kir_u8*)K15_IR_ALLOCA(bpp);
	kir_u8* inputPixelBuffer3 = (kir_u8*)K15_IR_ALLOCA(bpp);
	kir_u8* inputPixelBuffer4 = (kir_u8*)K15_IR_ALLOCA(bpp);
	kir_u8* outputPixelBuffer = (kir_u8*)K15_IR_ALLOCA(bpp);

	kir_u32 scanlineSizeInBytes = bpp * sourceWidth;
	kir_u32 scanlineOffsetInBytes = 0;
	kir_u8* scanlineBuffer = (kir_u8*)K15_IR_MALLOC(scanlineSizeInBytes);

	//FK: There'll be one entry for each horizontal destination pixel.
	kir_u32 numIndexTableEntries = destinationWidth;
	kir_u32 indexTableSizeInBytes = numIndexTableEntries * sizeof(kir_bicubic_index);
	kir_u32 indexTablePos = 0;
	kir_bicubic_index* scanlineIndexTable = (kir_bicubic_index*)K15_IR_MALLOC(indexTableSizeInBytes);

	//FK: Fill the index table. Basically for each horizontal destination pixel, the
	//	  4 contribution source pixels together with the blend factor is calculated.
	_K15_IRCalculateBicubicIndexTableForScanline(sourceWidth, destinationWidth, 
		wrapMode, scanlineIndexTable);

	for (sourcePosY = 0; sourcePosY < sourceHeight; ++sourcePosY, ++destinationPosY)
	{
		//FK: Get scan line for current Y position.
		scanlineOffsetInBytes = sourcePosY * sourceWidth * bpp;		
		K15_IR_MEMCPY(scanlineBuffer, pSourcePixels + scanlineOffsetInBytes, scanlineSizeInBytes);

		for (destinationPosX = 0, indexTablePos = 0; 
			destinationPosX < destinationWidth; 
			++destinationPosX, ++indexTablePos)
		{
			//FK: Calculate index where we write the interpolated pixel to
			destinationPixelIndex = (destinationPosY + (destinationPosX * destinationHeight));

			sourcePixelIndex1 = scanlineIndexTable[indexTablePos].i1;
			sourcePixelIndex2 = scanlineIndexTable[indexTablePos].i2;
			sourcePixelIndex3 = scanlineIndexTable[indexTablePos].i3;
			sourcePixelIndex4 = scanlineIndexTable[indexTablePos].i4;
			t = scanlineIndexTable[indexTablePos].t;

			//FK: Read pixels to interpolate
			_K15_IRReadPixelFromIndex(sourcePixelIndex1, pixelFormat, scanlineBuffer, inputPixelBuffer1);
			_K15_IRReadPixelFromIndex(sourcePixelIndex2, pixelFormat, scanlineBuffer, inputPixelBuffer2);
			_K15_IRReadPixelFromIndex(sourcePixelIndex3, pixelFormat, scanlineBuffer, inputPixelBuffer3);
			_K15_IRReadPixelFromIndex(sourcePixelIndex4, pixelFormat, scanlineBuffer, inputPixelBuffer4);

			//FK: Read pixel to accumulate - add interpolated value to existing pixel values 
			_K15_IRReadPixelFromIndex(destinationPixelIndex, pixelFormat, pDestinationPixels, outputPixelBuffer);

			for (componentIndex = 0; componentIndex < bpp; ++componentIndex)
			{
				kir_u8 lerpedComponent = _K15_IRCubicHermiteU8(t,
					inputPixelBuffer1[componentIndex],
					inputPixelBuffer2[componentIndex],
					inputPixelBuffer3[componentIndex],
					inputPixelBuffer4[componentIndex]);

				outputPixelBuffer[componentIndex] += lerpedComponent;
			}
			_K15_IRWritePixelToIndex(destinationPixelIndex, pixelFormat, pDestinationPixels, outputPixelBuffer);
		}
	}
}

kir_internal kir_result _K15_IRResample(kir_resize_context* pContext)
{
	kir_filter_mode filter			= pContext->filter;
	const kir_u8* sourcePixels		= (pContext->pTranscodedPixels ? pContext->pTranscodedPixels : pContext->pSourcePixels);
	kir_u8* destinationPixels		= pContext->pDestinationPixels;
	kir_u32 destinationWidth		= pContext->destinationWidth;
	kir_u32 destinationHeight		= pContext->destinationHeight;
	kir_u32 sourceHeight			= pContext->sourceHeight;
	kir_u32 sourceWidth				= pContext->sourceWidth;
	kir_wrap_mode wrapMode			= pContext->wrapMode;
	kir_pixel_format pixelFormat	= pContext->destinationFormat;

	int bpp = kir_format_to_byte_lut[pContext->destinationFormat];

	kir_u32 destinationPixelDataSizeInBytes = destinationWidth * destinationHeight * bpp;
	K15_IR_MEMSET(destinationPixels, 0, destinationPixelDataSizeInBytes);

	if (filter == K15_IR_NEAREST_NEIGHBOUR_FILTER_MODE)
	{
		_K15_IRSampleNearestNeighbour(sourcePixels, pContext->pDestinationPixels,
			pContext->sourceWidth, pContext->sourceHeight, pContext->destinationWidth, 
			pContext->destinationHeight, pContext->destinationFormat);
	}
	else if (filter == K15_IR_BILINEAR_FILTER_MODE ||
		filter == K15_IR_BICUBIC_FILTER_MODE)
	{
		kir_u32 tempWidth = destinationWidth;
		kir_u32 tempHeight = sourceHeight;

		kir_pixel_format format			= pContext->destinationFormat;
		kir_u32 bpp						= kir_format_to_byte_lut[format];
		kir_u32 tempBufferSizeInBytes	= tempWidth * tempHeight * bpp;
		kir_u8* horizontalTempBuffer	= (kir_u8*)K15_IR_MALLOC(tempBufferSizeInBytes);
		K15_IR_MEMSET(horizontalTempBuffer, 0, tempBufferSizeInBytes);

		if (filter == K15_IR_BILINEAR_FILTER_MODE)
		{
			_K15_IRHorizontallySampleBilinear(sourcePixels, horizontalTempBuffer, 
				sourceWidth, sourceHeight, 
				tempWidth, tempHeight, 
				pixelFormat, wrapMode);
		
			//FK: Image now vertically aligned as opposed to horizintally.
			//	  Lets switch width and height, so everything is back to "normal".
			tempWidth	= sourceHeight;
			tempHeight	= destinationWidth;

			destinationWidth	= pContext->destinationHeight;
			destinationHeight	= pContext->destinationWidth;

			_K15_IRHorizontallySampleBilinear(horizontalTempBuffer, destinationPixels, 
				tempWidth, tempHeight, 
				destinationWidth, destinationHeight,
				pixelFormat, wrapMode);
		}
		else if (filter == K15_IR_BICUBIC_FILTER_MODE)
		{
			_K15_IRHorizontallySampleBicubic(sourcePixels, horizontalTempBuffer,
				sourceWidth, sourceHeight,
				tempWidth, tempHeight,
				pixelFormat, wrapMode);

			//FK: Image now vertically aligned as opposed to horizintally.
			//	  Lets switch width and height, so everything is back to "normal".
			tempWidth = sourceHeight;
			tempHeight = destinationWidth;

			destinationWidth = pContext->destinationHeight;
			destinationHeight = pContext->destinationWidth;

			_K15_IRHorizontallySampleBicubic(horizontalTempBuffer, destinationPixels, 
				tempWidth, tempHeight, 
				destinationWidth, destinationHeight,
				pixelFormat, wrapMode);
		}

	}

	return K15_IR_RESULT_SUCCESS;
}

kir_result K15_IRScaleImageData(const kir_u8* pSourcePixels, kir_u32 sourceWidth, kir_u32 sourceHeight, kir_pixel_format sourcePixelFormat,
	kir_u8* pDestinationPixels, kir_u32 destinationWIdth, kir_u32 destinationHeight, kir_pixel_format destinationPixelFormat,
	kir_wrap_mode wrapMode, kir_filter_mode filterMode)
{
	kir_resize_context ctx = {0};

	ctx.pSourcePixels 		= pSourcePixels;
	ctx.pDestinationPixels 	= pDestinationPixels;
	ctx.sourceWidth 		= sourceWidth;
	ctx.sourceHeight 		= sourceHeight;
	ctx.destinationWidth 	= destinationWIdth;
	ctx.destinationHeight 	= destinationHeight;
	ctx.sourceFormat 		= sourcePixelFormat;
	ctx.destinationFormat 	= destinationPixelFormat;
	ctx.wrapMode 			= wrapMode;
	ctx.filter 				= filterMode;

	_K15_IRTranscodeSourcePixelsToDestinationPixelFormat(&ctx);

	return _K15_IRResample(&ctx);
}

#endif //K15_IR_IMPLEMENTATION
#endif //_K15_ImageResize_h_