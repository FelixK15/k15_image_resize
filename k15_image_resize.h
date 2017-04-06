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

#ifndef K15_IR_STATIC
# define kir_def static
#else
# define kir_def extern
#endif //K15_IA_STATIC

typedef signed int kir_s32;
typedef unsigned int kir_u32;
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

kir_result K15_IRScaleImageData(kir_u8* p_SourceImageData, kir_u32 p_SourceImagePixelWidth, 
	kir_u32 p_SourceImagePixelHeight, kir_pixel_format p_SourceImageDataPixelFormat,
	kir_u8* p_DestinationImageData, kir_u32 p_DestinationImagePixelWidth,
	kir_u32 p_DestinationImagePixelHeight, kir_pixel_format p_DestinationImageDataPixelFormat);

#ifdef K15_IR_IMPLEMENTATION

#define K15_IR_TRUE 1
#define K15_IR_FALSE 0

#ifndef K15_IR_MALLOC
# include <stdlib.h>
# define K15_IR_MALLOC malloc
# define K15_IR_FREE free
#endif //K15_IA_MALLOC

#ifndef K15_IR_MEMCPY
# include <string.h>
# define K15_IR_MEMCPY memcpy
#endif //K15_IA_MEMCPY

#ifndef K15_IR_MEMSET
# include <string.h>
# define K15_IR_MEMSET memset
#endif //K15_IA_MEMSET

#ifndef K15_IR_MEMMOVE
# include <string.h>
# define K15_IR_MEMMOVE memmove
#endif //K15_IA_MEMMOVE

#ifndef K15_IR_CEIL
# include <math.h>
# define K15_IR_CEIL ceil
# define K15_IR_SIN sinf
#endif //K15_IR_CEIL

#ifndef K15_IR_MIN
# define K15_IR_MIN(a,b) ((a) < (b) ? (a) : (b))
#endif //K15_IA_MIN

#ifndef K15_IR_MAX
# define K15_IR_MAX(a,b) ((a) > (b) ? (a) : (b))
#endif //K15_IA_MAX

#ifndef K15_IR_CLAMP
# define K15_IR_CLAMP(a, min, max) ((a) < (min) ? (min) : (a) > (max) ? (max) : (a))
#endif //K15_IR_CLAMP

#ifndef kir_internal
# define kir_internal static
#endif //kia_internal

kir_internal kir_u32 kir_format_to_byte_lut[] = {
	1, //K15_KIR_PIXEL_FORMAT_R8
	2, //K15_KIR_PIXEL_FORMAT_R8G8
	3, //K15_KIR_PIXEL_FORMAT_R8G8B8
	4  //K15_KIR_PIXEL_FORMAT_R8G8B8A8
};

typedef struct 
{
	kir_u8 r;
	kir_u8 g;
	kir_u8 b;
} kir_rgb_pixel;

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
	void* horizontalBuffer;
	kir_u8* sourcePixels;
	kir_u8* destinationPixels;
	kir_u8* transcodedSourcePixels;
	kir_u32 sourceWidth;
	kir_u32 sourceHeight;
	kir_u32 destinationWidth;
	kir_u32 destinationHeight;
	kir_u32 flags;
	kir_filter filter;
	kir_pixel_format sourceFormat;
	kir_pixel_format destinationFormat;
} kir_resize_context;

kir_internal kir_u8 _K15_IRLerpU8(float p_Factor, kir_u8 p_StartValue, kir_u8 p_EndValue)
{
	return (kir_u8)((float)p_StartValue * (1.f - p_Factor) + ((float)p_EndValue * p_Factor));
}

kir_internal void _K15_IRReadRGBPixelFromIndex(kir_u32 p_PixelIndex, kir_u8* p_ImageData, kir_rgb_pixel* p_PixelOut)
{
	p_PixelOut->r = p_ImageData[p_PixelIndex * 3 + 0];
	p_PixelOut->g = p_ImageData[p_PixelIndex * 3 + 1];
	p_PixelOut->b = p_ImageData[p_PixelIndex * 3 + 2];
}

kir_internal void _K15_IRWriteRGBPixelToIndex(kir_u32 p_PixelIndex, kir_u8* p_ImageData, kir_rgb_pixel* p_Pixel)
{
	p_ImageData[p_PixelIndex * 3 + 0] = p_Pixel->r;
	p_ImageData[p_PixelIndex * 3 + 1] = p_Pixel->g;
	p_ImageData[p_PixelIndex * 3 + 2] = p_Pixel->b;
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

	//FK: Do we actually have to transcode pixels?
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

kir_internal void _K15_IR_DownsampleNearestNeighbour(kir_u8* p_SourcePixels, kir_u8* p_DestinationPixels, 
	kir_u32 p_SourceWidth, kir_u32 p_SourceHeight, kir_u32 p_DestinationWidth, kir_u32 p_DestinationHeight,
	kir_pixel_format p_PixelFormat)
{
	kir_u32 sourcePosX = 0;
	kir_u32 sourcePosY = 0;

	kir_u32 destinationPosX = 0;
	kir_u32 destinationPosY = 0;

	float u = 0.f;
	float v = 0.f;

	kir_rgb_pixel sourcePixel = {0};

	kir_u32 sourcePixelIndex = 0;
	kir_u32 destinationPixelIndex = 0;

	for (destinationPosY = 0; destinationPosY < p_DestinationHeight; ++destinationPosY)
	{
		v = ((float)p_DestinationHeight / (float)destinationPosY);

		for (destinationPosX = 0; destinationPosX < p_DestinationWidth; ++destinationPosX)
		{
			u = ((float)p_DestinationWidth / (float)destinationPosX);

			sourcePosX = (kir_u32)((u * (float)p_SourceWidth) + 0.5f);
			sourcePosY = (kir_u32)((v * (float)p_SourceHeight) + 0.5f);

			sourcePixelIndex = sourcePosX + (sourcePosY * p_SourceWidth);
			destinationPixelIndex = destinationPosX + (destinationPosY * p_DestinationWidth);

			_K15_IRReadRGBPixelFromIndex(sourcePixelIndex, p_SourcePixels, &sourcePixel);
			_K15_IRWriteRGBPixelToIndex(destinationPixelIndex, p_DestinationPixels, &sourcePixel);
		}	
	}
}

kir_internal void _k15_IR_DownsampleBilinear(kir_resize_context* p_Context)
{
	float sourcePosXFract = 0.f;
	float sourcePosYFract = 0.f;

	kir_u32 sourcePosX = 0;
	kir_u32 sourcePosY = 0;

	kir_u32 destinationPosX = 0;
	kir_u32 destinationPosY = 0;

	float u = 0.f;
	float v = 0.f;

	kir_rgb_pixel sourcePixel = {0};

	kir_u32 sourcePixelIndex = 0;
	kir_u32 destinationPixelIndex = 0;

	for (destinationPosY = 0; destinationPosY < p_DestinationHeight; ++destinationPosY)
	{
		v = ((float)p_DestinationHeight / (float)destinationPosY);
		
		for (destinationPosX = 0; destinationPosX < p_DestinationWidth; ++destinationPosX)
		{
			u = ((float)p_DestinationWidth / (float)destinationPosX);

			sourcePosXFract = (u * (float)p_SourceWidth) - 0.5f;
			sourcePosYFract = (v * (float)p_SourceHeight) - 0.5f;

			sourcePosX = (kir_u32)(sourcePosXFract);
			sourcePosY = (kir_u32)(sourcePosXFract);

			sourcePosXFract = (float)sourcePosX - K15_IR_FLOOR(sourcePosXFract);
			sourcePosXFract = (float)sourcePosY - K15_IR_FLOOR(sourcePosYFract);

			sourcePixelIndex = sourcePosX + (sourcePosY * p_SourceWidth);
			destinationPixelIndex = destinationPosX + (destinationPosY * p_DestinationWidth);

			_K15_IRReadRGBPixelFromIndex(sourcePixelIndex, p_SourcePixels, &sourcePixel);
			_K15_IRWriteRGBPixelToIndex(destinationPixelIndex, p_DestinationPixels, &sourcePixel);
		}	
	}
}

kir_internal void _K15_IRResample(kir_resize_context* p_Context)
{
	kir_filter filter = p_Context->filter;

	if (filter == K15_IR_NEAREST_NEIGHBOUR_FILTER)
		_K15_IR_DownsampleNearestNeighbour(p_Context);
	else if (filter == K15_IR_BILINEAR_FILTER)
		_k15_IR_DownsampleBilinear(p_Context);
}

kir_result K15_IRScaleImageData(kir_u8* p_SourceImageData, kir_u32 p_SourceImagePixelWidth, 
	kir_u32 p_SourceImagePixelHeight, kir_pixel_format p_SourceImageDataPixelFormat,
	kir_u8* p_DestinationImageData, kir_u32 p_DestinationImagePixelWidth,
	kir_u32 p_DestinationImagePixelHeight, kir_pixel_format p_DestinationImageDataPixelFormat)
{
	void* tempBuffer = K15_IR_MALLOC(p_DestinationImagePixelWidth * 
		p_SourceImagePixelHeight * p_DestinationImageDataPixelFormat);

	if (!tempBuffer)
		return K15_IR_RESULT_OUT_OF_MEMORY;

	kir_resize_context ctx = {0};
	ctx.sourcePixels = p_SourceImageData;
	ctx.destinationPixels = p_DestinationImageData;
	ctx.sourceWidth = p_SourceImagePixelWidth;
	ctx.sourceHeight = p_SourceImagePixelHeight;
	ctx.destinationWidth = p_DestinationImagePixelWidth;
	ctx.destinationHeight = p_DestinationImagePixelHeight;
	ctx.sourceFormat = p_SourceImageDataPixelFormat;
	ctx.destinationFormat = p_DestinationImageDataPixelFormat;
	ctx.horizontalBuffer = tempBuffer;
	ctx.flags |= (ctx.destinationWidth > ctx.sourceWidth ? K15_IR_CONTEXT_UPSAMPLE_HORIZONTAL_FLAG : K15_IR_CONTEXT_DOWNSAMPLE_HORIZONTAL_FLAG);
	ctx.flags |= (ctx.destinationHeight > ctx.sourceHeight ? K15_IR_CONTEXT_UPSAMPLE_VERTICAL_FLAG : K15_IR_CONTEXT_DOWNSAMPLE_VERTICAL_FLAG);

	_K15_IRTranscodeSourcePixelsToDestinationPixelFormat(&ctx);

	kir_result result = _K15_IRResample(&ctx);

	K15_IR_FREE(tempBuffer);

	return result;
}

#endif //K15_IR_IMPLEMENTATION
#endif //_K15_ImageResize_h_