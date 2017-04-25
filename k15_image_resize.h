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

typedef signed int kir_s32;
typedef unsigned int kir_u32;
typedef signed short kir_s16;
typedef unsigned short kir_u16;

typedef unsigned char kir_u8;
typedef unsigned char kir_b8;
typedef unsigned char kir_byte;

typedef enum 
{
	K15_IR_RESULT_SUCCESS = 0,
	K15_IR_RESULT_OUT_OF_MEMORY = 1
} kir_result;

typedef enum 
{
	K15_IR_PIXEL_FORMAT_R8 = 0,
	K15_IR_PIXEL_FORMAT_R8A8,
	K15_IR_PIXEL_FORMAT_R8A8B8,
	K15_IR_PIXEL_FORMAT_R8A8B8A8
} kir_pixel_format;

typedef enum 
{
	K15_IR_WRAP_CLAMP = 0,
	K15_IR_WRAP_REPEAT,
	K15_IR_WRAP_MIRROR
} kir_wrap_mode;

kir_result K15_IRScaleImageData(kir_u8* p_SourceImageData, kir_u32 p_SourceImagePixelWidth, 
	kir_u32 p_SourceImagePixelHeight, kir_pixel_format p_SourceImageDataPixelFormat,
	kir_u8* p_DestinationImageData, kir_u32 p_DestinationImagePixelWidth,
	kir_u32 p_DestinationImagePixelHeight, kir_pixel_format p_DestinationImageDataPixelFormat,
	kir_wrap_mode p_WrapMode);

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

typedef enum
{
	K15_IR_CONTEXT_UPSAMPLE_HORIZONTAL_FLAG = 0x0,
	K15_IR_CONTEXT_UPSAMPLE_VERTICAL_FLAG = 0x1,
	K15_IR_CONTEXT_DOWNSAMPLE_HORIZONTAL_FLAG = 0x2,
	K15_IR_CONTEXT_DOWNSAMPLE_VERTICAL_FLAG = 0x4
} kir_ctx_flags;

typedef enum 
{
	K15_IR_NEAREST_NEIGHBOUR_FILTER = 0,
	K15_IR_BILINEAR_FILTER = 1,
	K15_IR_BICUBIC_FILTER = 2
} kir_filter;

typedef struct
{
	kir_u32 i1;
	kir_u32 i2;
	float t;
} kir_bilinear_index;

typedef struct
{
	void* horizontalBuffer;
	kir_u8* sourcePixels;
	kir_u8* destinationPixels;
	kir_u8* transcodedSourcePixels;
	kir_u32 flags;
	kir_filter filter;
	kir_pixel_format sourceFormat;
	kir_pixel_format destinationFormat;
	kir_wrap_mode wrapMode;
	kir_u32 sourceWidth;
	kir_u32 sourceHeight;
	kir_u32 destinationWidth;
	kir_u32 destinationHeight;

} kir_resize_context;

kir_internal kir_u8 _K15_IRLerpU8(float p_Factor, kir_u8 p_StartValue, kir_u8 p_EndValue)
{
	return (kir_u8)((float)p_StartValue * (1.f - p_Factor) + ((float)p_EndValue * p_Factor));
}

kir_internal void _K15_IRReadPixelFromIndex(kir_u32 p_PixelIndex, kir_pixel_format p_PixelFormat, 
	kir_u8* p_Pixels, kir_u8* p_OutputPixels)
{
	kir_u32 bpp = kir_format_to_byte_lut[p_PixelFormat];
	K15_IR_MEMCPY(p_OutputPixels, p_Pixels + p_PixelIndex * bpp, bpp);
}

kir_internal void _K15_IRWritePixelToIndex(kir_u32 p_PixelIndex, kir_pixel_format p_PixelFormat, 
	kir_u8* p_Pixels, kir_u8* p_InputPixels)
{
	kir_u32 bpp = kir_format_to_byte_lut[p_PixelFormat];
	K15_IR_MEMCPY(p_Pixels + p_PixelIndex * bpp, p_InputPixels, bpp);
}

kir_internal void _K15_IRTranscodeFromR8ToR8G8(kir_u8* p_InputPixels, kir_u8* p_OutputPixels, kir_u32 p_Width, kir_u32 p_Height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 255;

	for (posY = 0; posY < p_Height; ++posY)
	{
		for (posX = 0; posX < p_Width; ++posX)
		{
			pixelIndex = posX + (posY * p_Width);
			r = p_InputPixels[pixelIndex];

			p_OutputPixels[pixelIndex * 2 + 0] = r;
			p_OutputPixels[pixelIndex * 2 + 1] = g;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8ToR8G8B8(kir_u8* p_InputPixels, kir_u8* p_OutputPixels, kir_u32 p_Width, kir_u32 p_Height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;

	for (posY = 0; posY < p_Height; ++posY)
	{
		for (posX = 0; posX < p_Width; ++posX)
		{
			pixelIndex = posX + (posY * p_Width);
			r = p_InputPixels[pixelIndex];

			p_OutputPixels[pixelIndex * 3 + 0] = r;
			p_OutputPixels[pixelIndex * 3 + 1] = r;
			p_OutputPixels[pixelIndex * 3 + 2] = r;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8ToR8G8B8A8(kir_u8* p_InputPixels, kir_u8* p_OutputPixels, kir_u32 p_Width, kir_u32 p_Height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 a = 255;

	for (posY = 0; posY < p_Height; ++posY)
	{
		for (posX = 0; posX < p_Width; ++posX)
		{
			pixelIndex = posX + (posY * p_Width);
			r = p_InputPixels[pixelIndex];

			p_OutputPixels[pixelIndex * 4 + 0] = r;
			p_OutputPixels[pixelIndex * 4 + 1] = r;
			p_OutputPixels[pixelIndex * 4 + 2] = r;
			p_OutputPixels[pixelIndex * 4 + 3] = a;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8G8ToR8(kir_u8* p_InputPixels, kir_u8* p_OutputPixels, kir_u32 p_Width, kir_u32 p_Height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 0;

	float coverage = 0.f;

	for (posY = 0; posY < p_Height; ++posY)
	{
		for (posX = 0; posX < p_Width; ++posX)
		{
			pixelIndex = posX + (posY * p_Width);
			r = p_InputPixels[pixelIndex * 2 + 0];
			g = p_InputPixels[pixelIndex * 2 + 1];

			coverage = (float)g / 255.f;

			r = (kir_u8)((float)r * coverage + 0.5f);

			p_OutputPixels[pixelIndex] = r;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8G8ToR8G8B8(kir_u8* p_InputPixels, kir_u8* p_OutputPixels, kir_u32 p_Width, kir_u32 p_Height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 0;
	kir_u8 b = 0;

	float coverage = 0.f;

	for (posY = 0; posY < p_Height; ++posY)
	{
		for (posX = 0; posX < p_Width; ++posX)
		{
			pixelIndex = posX + (posY * p_Width);
			r = p_InputPixels[pixelIndex * 2 + 0];
			g = p_InputPixels[pixelIndex * 2 + 1];

			coverage = (float)g / 255.f;

			r = (kir_u8)((float)r * coverage + 0.5f);

			p_OutputPixels[pixelIndex * 3 + 0] = r;
			p_OutputPixels[pixelIndex * 3 + 1] = r;
			p_OutputPixels[pixelIndex * 3 + 2] = r;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8G8ToR8G8B8A8(kir_u8* p_InputPixels, kir_u8* p_OutputPixels, kir_u32 p_Width, kir_u32 p_Height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 0;
	kir_u8 b = 0;
	kir_u8 a = 0;

	for (posY = 0; posY < p_Height; ++posY)
	{
		for (posX = 0; posX < p_Width; ++posX)
		{
			pixelIndex = posX + (posY * p_Width);
			r = p_InputPixels[pixelIndex * 2 + 0];
			a = p_InputPixels[pixelIndex * 2 + 1];

			p_OutputPixels[pixelIndex * 4 + 0] = r;
			p_OutputPixels[pixelIndex * 4 + 1] = r;
			p_OutputPixels[pixelIndex * 4 + 2] = r;
			p_OutputPixels[pixelIndex * 4 + 3] = a;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8G8B8ToR8(kir_u8* p_InputPixels, kir_u8* p_OutputPixels, kir_u32 p_Width, kir_u32 p_Height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 0;
	kir_u8 b = 0;

	kir_u8 greyshade = 0;

	for (posY = 0; posY < p_Height; ++posY)
	{
		for (posX = 0; posX < p_Width; ++posX)
		{
			pixelIndex = posX + (posY * p_Width);
			r = p_InputPixels[pixelIndex * 3 + 0];
			g = p_InputPixels[pixelIndex * 3 + 1];
			b = p_InputPixels[pixelIndex * 3 + 2];

			greyshade = (kir_u8)((float)r * 0.21f + (float)g * 0.72f + (float)b * 0.07f);

			p_OutputPixels[pixelIndex] = greyshade;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8G8B8ToR8G8(kir_u8* p_InputPixels, kir_u8* p_OutputPixels, kir_u32 p_Width, kir_u32 p_Height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 0;
	kir_u8 b = 0;
	kir_u8 a = 255;

	kir_u8 greyshade = 0;

	for (posY = 0; posY < p_Height; ++posY)
	{
		for (posX = 0; posX < p_Width; ++posX)
		{
			pixelIndex = posX + (posY * p_Width);
			r = p_InputPixels[pixelIndex * 3 + 0];
			g = p_InputPixels[pixelIndex * 3 + 1];
			b = p_InputPixels[pixelIndex * 3 + 2];

			greyshade = (kir_u8)((float)r * 0.21f + (float)g * 0.72f + (float)b * 0.07f);

			p_OutputPixels[pixelIndex * 2 + 0] = greyshade;
			p_OutputPixels[pixelIndex * 2 + 1] = a;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8G8B8ToR8G8B8A8(kir_u8* p_InputPixels, kir_u8* p_OutputPixels, kir_u32 p_Width, kir_u32 p_Height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 0;
	kir_u8 b = 0;
	kir_u8 a = 255;

	for (posY = 0; posY < p_Height; ++posY)
	{
		for (posX = 0; posX < p_Width; ++posX)
		{
			pixelIndex = posX + (posY * p_Width);
			r = p_InputPixels[pixelIndex * 3 + 0];
			g = p_InputPixels[pixelIndex * 3 + 1];
			b = p_InputPixels[pixelIndex * 3 + 2];

			p_OutputPixels[pixelIndex * 4 + 0] = r;
			p_OutputPixels[pixelIndex * 4 + 1] = g;
			p_OutputPixels[pixelIndex * 4 + 2] = b;
			p_OutputPixels[pixelIndex * 4 + 3] = a;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8G8B8A8ToR8(kir_u8* p_InputPixels, kir_u8* p_OutputPixels, kir_u32 p_Width, kir_u32 p_Height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 0;
	kir_u8 b = 0;
	kir_u8 a = 0;
	kir_u8 greyshade = 0;

	float coverage = 0.f;

	for (posY = 0; posY < p_Height; ++posY)
	{
		for (posX = 0; posX < p_Width; ++posX)
		{
			pixelIndex = posX + (posY * p_Width);
			r = p_InputPixels[pixelIndex * 4 + 0];
			g = p_InputPixels[pixelIndex * 4 + 1];
			b = p_InputPixels[pixelIndex * 4 + 2];
			a = p_InputPixels[pixelIndex * 4 + 3];

			coverage = (float)a / 255.f;

			greyshade = (kir_u8)(coverage * ((float)r * 0.21f + (float)g * 0.72f + (float)b * 0.07f));

			p_OutputPixels[pixelIndex] = greyshade;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8G8B8A8ToR8G8(kir_u8* p_InputPixels, kir_u8* p_OutputPixels, kir_u32 p_Width, kir_u32 p_Height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 0;
	kir_u8 b = 0;
	kir_u8 a = 0;
	kir_u8 greyshade = 0;

	for (posY = 0; posY < p_Height; ++posY)
	{
		for (posX = 0; posX < p_Width; ++posX)
		{
			pixelIndex = posX + (posY * p_Width);
			r = p_InputPixels[pixelIndex * 4 + 0];
			g = p_InputPixels[pixelIndex * 4 + 1];
			b = p_InputPixels[pixelIndex * 4 + 2];
			a = p_InputPixels[pixelIndex * 4 + 3];

			greyshade = (kir_u8)((float)r * 0.21f + (float)g * 0.72f + (float)b * 0.07f);

			p_OutputPixels[pixelIndex * 2 + 0] = greyshade;
			p_OutputPixels[pixelIndex * 2 + 1] = a;
		}
	}
}

kir_internal void _K15_IRTranscodeFromR8G8B8A8ToR8G8B8(kir_u8* p_InputPixels, kir_u8* p_OutputPixels, kir_u32 p_Width, kir_u32 p_Height)
{
	kir_u32 posX = 0;
	kir_u32 posY = 0;

	kir_u32 pixelIndex = 0;

	kir_u8 r = 0;
	kir_u8 g = 0;
	kir_u8 b = 0;
	kir_u8 a = 0;

	float coverage = 0.f;

	for (posY = 0; posY < p_Height; ++posY)
	{
		for (posX = 0; posX < p_Width; ++posX)
		{
			pixelIndex = posX + (posY * p_Width);
			r = p_InputPixels[pixelIndex * 4 + 0];
			g = p_InputPixels[pixelIndex * 4 + 1];
			b = p_InputPixels[pixelIndex * 4 + 2];
			a = p_InputPixels[pixelIndex * 4 + 3];

			coverage = (float)a / 255.f;

			r = (kir_u8)((float)r * coverage); 
			g = (kir_u8)((float)g * coverage); 
			b = (kir_u8)((float)b * coverage); 

			p_OutputPixels[pixelIndex * 3 + 0] = r;
			p_OutputPixels[pixelIndex * 3 + 1] = g;
			p_OutputPixels[pixelIndex * 3 + 2] = b;
		}
	}
}


kir_internal void _K15_IRTranscodeSourcePixelsToDestinationPixelFormat(kir_resize_context* p_Context)
{
	kir_u32 sourceWidth = p_Context->sourceWidth;
	kir_u32 sourceHeight = p_Context->sourceHeight;
	kir_u8* sourcePixels = p_Context->sourcePixels;

	kir_pixel_format sourceFormat = p_Context->sourceFormat;
	kir_pixel_format destinationFormat = p_Context->destinationFormat;

	//FK: check if we actually have to transcode pixels
	if (sourceFormat != destinationFormat)
	{
		void* transcodedSourcePixels = K15_IR_MALLOC(sourceWidth * sourceHeight * kir_format_to_byte_lut[destinationFormat]);

		if (sourceFormat == K15_IR_PIXEL_FORMAT_R8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8A8)
			_K15_IRTranscodeFromR8ToR8G8(sourcePixels, transcodedSourcePixels, sourceWidth, sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8A8B8)
			_K15_IRTranscodeFromR8ToR8G8B8(sourcePixels, transcodedSourcePixels, sourceWidth, sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8A8B8A8)
			_K15_IRTranscodeFromR8ToR8G8B8A8(sourcePixels, transcodedSourcePixels, sourceWidth, sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8A8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8)
			_K15_IRTranscodeFromR8G8ToR8(sourcePixels, transcodedSourcePixels, sourceWidth, sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8A8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8A8B8)
			_K15_IRTranscodeFromR8G8ToR8G8B8(sourcePixels, transcodedSourcePixels, sourceWidth, sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8A8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8A8B8A8)
			_K15_IRTranscodeFromR8G8ToR8G8B8A8(sourcePixels, transcodedSourcePixels, sourceWidth, sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8A8B8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8)
			_K15_IRTranscodeFromR8G8B8ToR8(sourcePixels, transcodedSourcePixels, sourceWidth, sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8A8B8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8A8)
			_K15_IRTranscodeFromR8G8B8ToR8G8(sourcePixels, transcodedSourcePixels, sourceWidth, sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8A8B8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8A8B8A8)
			_K15_IRTranscodeFromR8G8B8ToR8G8B8A8(sourcePixels, transcodedSourcePixels, sourceWidth, sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8A8B8A8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8)
			_K15_IRTranscodeFromR8G8B8A8ToR8(sourcePixels, transcodedSourcePixels, sourceWidth, sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8A8B8A8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8A8)
			_K15_IRTranscodeFromR8G8B8A8ToR8G8(sourcePixels, transcodedSourcePixels, sourceWidth, sourceHeight);
		else if (sourceFormat == K15_IR_PIXEL_FORMAT_R8A8B8A8 && destinationFormat == K15_IR_PIXEL_FORMAT_R8A8B8)
			_K15_IRTranscodeFromR8G8B8A8ToR8G8B8(sourcePixels, transcodedSourcePixels, sourceWidth, sourceHeight);

		p_Context->transcodedSourcePixels = transcodedSourcePixels;
	}
}

kir_internal kir_u32 _K15_IRGetWrappedCoordinate(kir_s32 p_Pos, 
	kir_s32 p_Threshold, kir_wrap_mode p_WrapMode)
{
	if (p_Pos < 0)
	{
		if (p_WrapMode == K15_IR_WRAP_CLAMP)
			return 0;
		else if (p_WrapMode == K15_IR_WRAP_REPEAT)
		{
			kir_u16 absPos = K15_IR_ABS(p_Pos);
			absPos %= p_Threshold;
			return (p_Threshold - absPos);
		}
	}
	else if (p_Pos > p_Threshold)
	{
		if (p_WrapMode == K15_IR_WRAP_CLAMP)
			return p_Threshold - 1;
		else if (p_WrapMode == K15_IR_WRAP_REPEAT)
		{
			p_Pos %= p_Threshold;
			return (p_Pos);
		}
	}

	return p_Pos;
}

kir_internal kir_u32 _K15_IRGetWrappedIndex(kir_s32 p_PosX, kir_s32 p_PosY, 
	kir_u32 p_Width, kir_u32 p_Height, kir_wrap_mode p_WrapMode)
{
	p_PosX = _K15_IRGetWrappedCoordinate(p_PosX, p_Width, p_WrapMode);
	p_PosY = _K15_IRGetWrappedCoordinate(p_PosY, p_Height, p_WrapMode);

	return (p_PosX + (p_PosY * p_Width));
}

kir_internal void _K15_IRDownsampleNearestNeighbour(kir_u8* p_SourcePixels, kir_u8* p_DestinationPixels, 
	kir_u32 p_SourceWidth, kir_u32 p_SourceHeight, kir_u32 p_DestinationWidth, kir_u32 p_DestinationHeight,
	kir_pixel_format p_PixelFormat)
{
	kir_u32 sourcePosX = 0;
	kir_u32 sourcePosY = 0;

	kir_u32 destinationPosX = 0;
	kir_u32 destinationPosY = 0;

	float u = 0.f;
	float v = 0.f;

	kir_u32 sourcePixelIndex = 0;
	kir_u32 destinationPixelIndex = 0;

	kir_u32 bpp = kir_format_to_byte_lut[p_PixelFormat];
	kir_u8* pixelBuffer = (kir_u8*)K15_IR_ALLOCA(bpp);

	for (destinationPosY = 0; destinationPosY < p_DestinationHeight; ++destinationPosY)
	{
		v = (float)destinationPosY / (float)p_DestinationHeight;
		sourcePosY = (kir_u32)(v * (float)p_SourceHeight);
		
		for (destinationPosX = 0; destinationPosX < p_DestinationWidth; ++destinationPosX)
		{
			u = (float)destinationPosX / (float)p_DestinationWidth;

			sourcePosX = (kir_u32)(u * (float)p_SourceWidth);

			sourcePixelIndex = sourcePosX + (sourcePosY * p_SourceWidth);
			destinationPixelIndex = destinationPosX + (destinationPosY * p_DestinationWidth);
			
			_K15_IRReadPixelFromIndex(sourcePixelIndex, p_PixelFormat, p_SourcePixels, pixelBuffer);
			_K15_IRWritePixelToIndex(destinationPixelIndex, p_PixelFormat, p_DestinationPixels, pixelBuffer);
		}	
	}
}

kir_internal void _K15_IRCreateBilinearIndexTable(kir_u32 p_DestinationWidth, kir_u32 p_DestinationHeight,
                                                  kir_u32 p_SourceWidth, kir_u32 p_SourceHeight, 
                                                  kir_wrap_mode p_WrapMode, kir_pixel_format p_PixelFormat,
                                                  kir_bilinear_index* p_IndexTable)
{
	float u = 0.f;
	float v = 0.f;

	float sourcePosXFract = 0.f;
	float sourcePosYFract = 0.f;

	kir_s32 sourcePosX = 0;
	kir_s32 sourcePosY = 0;

	kir_u32 destinationPosX = 0;
	kir_u32 destinationPosY = 0;

	kir_u32 indexTableCounter = 0;

	kir_u32 i1 = 0;
	kir_u32 i2 = 0;

	for (destinationPosY = 0; destinationPosY < p_DestinationHeight; ++destinationPosY)
	{
		v = (float)destinationPosY / (float)p_DestinationHeight;
		sourcePosYFract = (v * (float)p_SourceHeight) + 0.5f;
		sourcePosY = (kir_s16)K15_IR_FLOORF(sourcePosYFract);

		for (destinationPosX = 0; destinationPosX < p_DestinationWidth; ++destinationPosX)
		{
			u = (float)destinationPosX / (float)p_DestinationWidth;

			sourcePosXFract = (u * (float)p_SourceWidth) + 0.5f;
			sourcePosX = (kir_s16)K15_IR_FLOORF(sourcePosXFract);
			sourcePosXFract = sourcePosXFract - (float)sourcePosX;

			i1 = _K15_IRGetWrappedIndex(sourcePosX + 0, sourcePosY, p_SourceWidth, p_SourceHeight, p_WrapMode);
			i2 = _K15_IRGetWrappedIndex(sourcePosX + 1, sourcePosY, p_SourceWidth, p_SourceHeight, p_WrapMode);

			p_IndexTable[indexTableCounter].i1 = i1;
			p_IndexTable[indexTableCounter].i2 = i2;
			p_IndexTable[indexTableCounter++].t = sourcePosXFract;
		}	
	}
}

kir_internal void _K15_IRHorizontallyDownsampleBilinear(kir_u8* p_SourcePixels, kir_u8* p_DestinationPixels,
	kir_u16 p_SourceWidth, kir_u16 p_SourceHeight, kir_u16 p_DestinationWidth, kir_u16 p_DestinationHeight,
	kir_pixel_format p_PixelFormat, kir_wrap_mode p_WrapMode)
{
	float sourcePosXFract = 0.f;
	float sourcePosYFract = 0.f;

	kir_u32 sourcePosX = 0;
	kir_u32 sourcePosY = 0;

	kir_u32 destinationPosX = 0;
	kir_u32 destinationPosY = 0;

	kir_u32 sourcePixelIndex1 = 0;
	kir_u32 sourcePixelIndex2 = 0;

	float u = 0.f;
	float v = 0.f;
	float t = 0.f;

	kir_u32 destinationPixelIndex = 0;

	kir_u32 componentIndex = 0;
	kir_u32 indexTableIndex = 0;
	kir_u32 sizeIndexTable = p_DestinationWidth * p_DestinationHeight;
	kir_u32 sizeIndexTableInBytes = sizeIndexTable * sizeof(kir_bilinear_index);
	kir_bilinear_index* indexTable = (kir_bilinear_index*)K15_IR_MALLOC(sizeIndexTableInBytes);

	kir_u32 bpp = kir_format_to_byte_lut[p_PixelFormat];
	kir_u8* inputPixelBuffer1 = (kir_u8*)K15_IR_ALLOCA(bpp);
	kir_u8* inputPixelBuffer2 = (kir_u8*)K15_IR_ALLOCA(bpp);
	kir_u8* outputPixelBuffer = (kir_u8*)K15_IR_ALLOCA(bpp);

	_K15_IRCreateBilinearIndexTable(p_DestinationWidth, p_DestinationHeight, 
		p_SourceWidth, p_SourceHeight, p_WrapMode, p_PixelFormat, indexTable);

	for (indexTableIndex = 0; indexTableIndex < sizeIndexTable; ++indexTableIndex, ++destinationPosX)
	{
		if (destinationPosX >= p_DestinationWidth)
		{
			destinationPosX = 0;
			++destinationPosY;
		}

		destinationPixelIndex = (destinationPosY + (destinationPosX * p_DestinationHeight));

		sourcePixelIndex1 = indexTable[indexTableIndex].i1;
		sourcePixelIndex2 = indexTable[indexTableIndex].i2;
		t = indexTable[indexTableIndex].t;

		//FK: Read pixels to interpolate
		_K15_IRReadPixelFromIndex(sourcePixelIndex1, p_PixelFormat, p_SourcePixels, inputPixelBuffer1);
		_K15_IRReadPixelFromIndex(sourcePixelIndex2, p_PixelFormat, p_SourcePixels, inputPixelBuffer2);
		
		//FK: Read pixel to accumulate
		_K15_IRReadPixelFromIndex(destinationPixelIndex, p_PixelFormat, p_DestinationPixels, outputPixelBuffer);

		for (componentIndex = 0; componentIndex < bpp; ++componentIndex)
		{
			kir_u8 lerpedComponent = _K15_IRLerpU8(t, inputPixelBuffer1[componentIndex], inputPixelBuffer2[componentIndex]);
			outputPixelBuffer[componentIndex] += lerpedComponent;
		}
		_K15_IRWritePixelToIndex(destinationPixelIndex, p_PixelFormat, p_DestinationPixels, outputPixelBuffer);
	}
}

kir_internal kir_result _K15_IRResample(kir_resize_context* p_Context)
{
	kir_filter filter = p_Context->filter;
	kir_u32 flags = p_Context->flags;
	kir_u8* sourcePixels = (p_Context->transcodedSourcePixels ? p_Context->transcodedSourcePixels : p_Context->sourcePixels);
	kir_u8* destinationPixels = p_Context->destinationPixels;
	kir_u32 destinationWidth = p_Context->destinationWidth;
	kir_u32 destinationHeight = p_Context->destinationHeight;
	kir_u32 sourceHeight = p_Context->sourceHeight;
	kir_u32 sourceWidth = p_Context->sourceWidth;
	kir_wrap_mode wrapMode = p_Context->wrapMode;
	kir_pixel_format pixelFormat = p_Context->destinationFormat;

	int bpp = kir_format_to_byte_lut[p_Context->destinationFormat];

	kir_u32 destinationPixelDataSizeInBytes = destinationWidth * destinationHeight * bpp;
	K15_IR_MEMSET(destinationPixels, 0, destinationPixelDataSizeInBytes);


	if (filter == K15_IR_NEAREST_NEIGHBOUR_FILTER)
	{
		_K15_IRDownsampleNearestNeighbour(sourcePixels, p_Context->destinationPixels,
			p_Context->sourceWidth, p_Context->sourceHeight, p_Context->destinationWidth, 
			p_Context->destinationHeight, p_Context->destinationFormat);
	}
	else if (filter == K15_IR_BILINEAR_FILTER)
	{
		kir_u32 tempWidth = destinationWidth;
		kir_u32 tempHeight = sourceHeight;

		kir_pixel_format format = p_Context->destinationFormat;
		kir_u32 bpp = kir_format_to_byte_lut[format];
		kir_u32 tempBufferSizeInBytes = tempWidth * tempHeight * bpp;
		kir_u8* horizontalTempBuffer = (kir_u8*)K15_IR_MALLOC(tempBufferSizeInBytes);
		K15_IR_MEMSET(horizontalTempBuffer, 0, tempBufferSizeInBytes);

		if (flags & K15_IR_CONTEXT_DOWNSAMPLE_HORIZONTAL_FLAG)
		{
			_K15_IRHorizontallyDownsampleBilinear(sourcePixels, horizontalTempBuffer, 
				sourceWidth, sourceHeight, 
				tempWidth, tempHeight, 
				pixelFormat, wrapMode);
		}
		
		//FK: Image now vertically aligned as opposed to horizintally.
		//	  Lets switch width and height, so everything is back to "normal".
		tempWidth = sourceHeight;
		tempHeight = destinationWidth;

		destinationWidth = p_Context->destinationHeight;
		destinationHeight = p_Context->destinationWidth;

		if (flags & K15_IR_CONTEXT_DOWNSAMPLE_VERTICAL_FLAG)
		{
			_K15_IRHorizontallyDownsampleBilinear(horizontalTempBuffer, destinationPixels, 
				tempWidth, tempHeight, 
				destinationWidth, destinationHeight,
				pixelFormat, wrapMode);
		}

		// else if (flags & K15_IR_CONTEXT_DOWNSAMPLE_HORIZONTAL_FLAG)
		// 	_K15_IR_HorizontallyDownsampleBilinear(p_Context);
	}

	return K15_IR_RESULT_SUCCESS;
}

kir_result K15_IRScaleImageData(kir_u8* p_SourceImageData, kir_u32 p_SourceImagePixelWidth, 
	kir_u32 p_SourceImagePixelHeight, kir_pixel_format p_SourceImageDataPixelFormat,
	kir_u8* p_DestinationImageData, kir_u32 p_DestinationImagePixelWidth,
	kir_u32 p_DestinationImagePixelHeight, kir_pixel_format p_DestinationImageDataPixelFormat,
	kir_wrap_mode p_WrapMode)
{
	kir_resize_context ctx = {0};

	ctx.sourcePixels = p_SourceImageData;
	ctx.destinationPixels = p_DestinationImageData;
	ctx.sourceWidth = p_SourceImagePixelWidth;
	ctx.sourceHeight = p_SourceImagePixelHeight;
	ctx.destinationWidth = p_DestinationImagePixelWidth;
	ctx.destinationHeight = p_DestinationImagePixelHeight;
	ctx.sourceFormat = p_SourceImageDataPixelFormat;
	ctx.destinationFormat = p_DestinationImageDataPixelFormat;
	ctx.wrapMode = p_WrapMode;
	//ctx.filter = K15_IR_BILINEAR_FILTER;
	ctx.filter = K15_IR_NEAREST_NEIGHBOUR_FILTER;
	ctx.flags |= (ctx.destinationWidth > ctx.sourceWidth ? K15_IR_CONTEXT_UPSAMPLE_HORIZONTAL_FLAG : K15_IR_CONTEXT_DOWNSAMPLE_HORIZONTAL_FLAG);
	ctx.flags |= (ctx.destinationHeight > ctx.sourceHeight ? K15_IR_CONTEXT_UPSAMPLE_VERTICAL_FLAG : K15_IR_CONTEXT_DOWNSAMPLE_VERTICAL_FLAG);

	_K15_IRTranscodeSourcePixelsToDestinationPixelFormat(&ctx);

	return _K15_IRResample(&ctx);
}

#endif //K15_IR_IMPLEMENTATION
#endif //_K15_ImageResize_h_