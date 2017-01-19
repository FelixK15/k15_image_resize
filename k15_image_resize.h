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
	K15_IR_RESULT_SUCCESS = 0
} kir_result;

typedef enum 
{
	KIR_PIXEL_FORMAT_R8 = 0,
	KIR_PIXEL_FORMAT_R8A8,
	KIR_PIXEL_FORMAT_R8G8B8,
	KIR_PIXEL_FORMAT_R8G8B8A8
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

#ifndef K15_IR_MIN
# define K15_IR_MIN(a,b) ((a) < (b) ? (a) : (b))
#endif //K15_IA_MIN

#ifndef K15_IR_MAX
# define K15_IR_MAX(a,b) ((a) > (b) ? (a) : (b))
#endif //K15_IA_MAX

#ifndef kir_internal
# define kir_internal static
#endif //kia_internal

kir_internal kir_result _K15_IRDownscaleImageData(kir_u8* p_SourceImageData, kir_u32 p_SourceImagePixelWidth,
	kir_u32 p_SourceImagePixelHeight, kir_pixel_format p_SourceImageDataPixelFormat,
	kir_u8* p_DestinationImageData, kir_u32 p_DestinationImagePixelWidth,
	kir_u32 p_DestinationImagePixelHeight, kir_pixel_format p_DestinationImageDataPixelFormat)
{
	float horizontalWeight = (float)p_DestinationImagePixelWidth / (float)p_SourceImagePixelWidth;
	float verticalWeight = (float)p_DestinationImagePixelHeight / (float)p_SourceImagePixelHeight;

	int verticalStepSize = p_SourceImagePixelHeight / p_DestinationImagePixelHeight;
	int horizontalStepSize = p_SourceImagePixelWidth / p_DestinationImagePixelWidth;

	int stepIndex = 0;

	for (stepIndex = 0; stepIndex < p_SourceImagePixelWidth; stepIndex += horizontalStepSize)
	{

	}

	return K15_IR_RESULT_SUCCESS;
}

kir_internal kir_result _K15_IRUpscaleImageData(kir_u8* p_SourceImageData, kir_u32 p_SourceImagePixelWidth, 
	kir_u32 p_SourceImagePixelHeight, kir_pixel_format p_SourceImageDataPixelFormat,
	kir_u8* p_DestinationImageData, kir_u32 p_DestinationImagePixelWidth,
	kir_u32 p_DestinationImagePixelHeight, kir_pixel_format p_DestinationImageDataPixelFormat)
{
	return K15_IR_RESULT_SUCCESS;
}

kir_result K15_IRScaleImageData(kir_u8* p_SourceImageData, kir_u32 p_SourceImagePixelWidth, 
	kir_u32 p_SourceImagePixelHeight, kir_pixel_format p_SourceImageDataPixelFormat,
	kir_u8* p_DestinationImageData, kir_u32 p_DestinationImagePixelWidth,
	kir_u32 p_DestinationImagePixelHeight, kir_pixel_format p_DestinationImageDataPixelFormat)
{
	kir_u32 sourceImageArea = p_SourceImagePixelHeight * p_SourceImagePixelWidth;
	kir_u32 destinationImageArea = p_DestinationImagePixelHeight * p_DestinationImagePixelWidth;

	kir_result result = K15_IR_RESULT_SUCCESS;

	if (sourceImageArea > destinationImageArea)
	{
		result = _K15_IRDownscaleImageData(p_SourceImageData, p_SourceImagePixelWidth,
			p_SourceImagePixelHeight, p_SourceImageDataPixelFormat, p_DestinationImageData,
			p_DestinationImagePixelWidth, p_DestinationImagePixelHeight, 
			p_DestinationImageDataPixelFormat);
	}
	else if (sourceImageArea < destinationImageArea)
	{
		result = _K15_IRUpscaleImageData(p_SourceImageData, p_SourceImagePixelWidth,
			p_SourceImagePixelHeight, p_SourceImageDataPixelFormat, p_DestinationImageData,
			p_DestinationImagePixelWidth, p_DestinationImagePixelHeight, 
			p_DestinationImageDataPixelFormat);
	}

	return result;
}

#endif //K15_IR_IMPLEMENTATION
#endif //_K15_ImageResize_h_