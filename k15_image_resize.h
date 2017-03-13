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

#ifndef K15_IR_CEIL
# include <math.h>
# define K15_IR_CEIL ceil
#endif //K15_IR_CEIL

#ifndef K15_IR_MIN
# define K15_IR_MIN(a,b) ((a) < (b) ? (a) : (b))
#endif //K15_IA_MIN

#ifndef K15_IR_MAX
# define K15_IR_MAX(a,b) ((a) > (b) ? (a) : (b))
#endif //K15_IA_MAX

#ifndef kir_internal
# define kir_internal static
#endif //kia_internal

typedef struct 
{
	kir_u8 r;
	kir_u8 g;
	kir_u8 b;
} kir_rgb_pixel;

kir_internal kir_u8 _K15_IRLerpU8(float p_Factor, kir_u8 p_StartValue, kir_u8 p_EndValue)
{
	return (kir_u8)((float)p_StartValue * (1.f - p_Factor) + ((float)p_EndValue * p_Factor));
}

kir_internal void _K15_IRGetSampleRange(float p_PosX, float p_PosY, kir_u32 p_Stride, float p_NumSamples, kir_u32* p_StartSample, kir_u32* p_EndSample)
{
	p_PosX += 0.5f;
	p_PosY += 0.5f;

	kir_u32 fixedPosX = (kir_u32)p_PosX;
	kir_u32 fixedPosY = (kir_u32)p_PosY;

	float fractPosX =  p_PosX - (float)fixedPosX;
	float fractPosY =  p_PosY - (float)fixedPosY;

	kir_u32 pixelIndex = (fixedPosX + (fixedPosY * p_Stride));

	*p_StartSample = fractPosX < 0.5f ? pixelIndex - 1 : pixelIndex;
	*p_EndSample = (kir_u32)(*p_StartSample + (p_NumSamples + 0.5f));
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

kir_internal void _K15_IRDownscaleImageDataLine(kir_u8* p_SourceImageData, kir_u32 p_SourceImageDataStride,
	kir_pixel_format p_SourceImageDataPixelFormat, kir_u8* p_DestinationImageData, 
	kir_u32 p_DestinationImageDataStride, kir_pixel_format p_DestinationImageDataPixelFormat,
	kir_u32 p_LinePosY)
{

}

kir_internal kir_result _K15_IRDownscaleImageData(kir_u8* p_SourceImageData, kir_u32 p_SourceImagePixelWidth,
	kir_u32 p_SourceImagePixelHeight, kir_pixel_format p_SourceImageDataPixelFormat,
	kir_u8* p_DestinationImageData, kir_u32 p_DestinationImagePixelWidth,
	kir_u32 p_DestinationImagePixelHeight, kir_pixel_format p_DestinationImageDataPixelFormat)
{
	float numVSamples = (float)p_SourceImagePixelHeight / (float)p_DestinationImagePixelHeight;
	float numHSamples = (float)p_SourceImagePixelWidth / (float)p_DestinationImagePixelWidth;

	kir_u32 numVIndexSamples = (kir_u32)K15_IR_CEIL(numVSamples);
	kir_u32 numHIndexSamples = (kir_u32)K15_IR_CEIL(numHSamples);

	kir_u32	numVBitSamples = (kir_u32)(numVSamples * 255.f);
	kir_u32 numHBitSamples = (kir_u32)(numHSamples * 255.f);
	
	kir_u32 bitCounter = 255;

	kir_rgb_pixel samplePixel = {0};

	kir_u32 dPosX = 0;
	kir_u32 sPosX = 0;
	kir_u32 dPosY = 0;
	kir_u32 sPosY = 0;

	for (dPosY = 0; dPosY < p_DestinationImagePixelHeight; ++dPosY)
	{
		for (dPosX = 0; dPosX < p_DestinationImagePixelWidth; ++dPosX)
		{
			_K15_IRDownscaleImageDataLine(p_SourceImageData, p_SourceImagePixelWidth,
				p_SourceImageDataPixelFormat, p_DestinationImageData, p_DestinationImageDataStride,
				p_DestinationImageDataPixelFormat, dPosY);
			
			kir_rgb_pixel destinationPixel = {0};
			kir_u32 dPixelIndex = dPosX + (dPosY * p_DestinationImagePixelWidth);
			kir_u32 sampleIndex = 0;
			kir_u32 bitSampleCounter = numHBitSamples;
			kir_u32 numPixelsFinished = 0;
			for (sampleIndex = 0; sampleIndex < numHIndexSamples; ++sampleIndex)
			{
				kir_u32 samplePosX = (kir_u32)sPosX + sampleIndex;

				//Clamp
				if (samplePosX >= p_SourceImagePixelWidth)
				{
					samplePosX = p_SourceImagePixelWidth - 1;
				}

				kir_u32 samplePixelIndex = samplePosX + (sPosY * p_SourceImagePixelWidth);

				//Get rgb pixel values and check how much we need to sample from this pixel
				_K15_IRReadRGBPixelFromIndex(samplePixelIndex, p_SourceImageData, &samplePixel);

				kir_u32 numHBitsToSample = K15_IR_MIN(bitCounter, bitSampleCounter);
				bitSampleCounter -= numHBitsToSample;
				bitCounter -= numHBitsToSample;

				if (bitCounter == 0)
				{
					bitCounter = 255;
					numPixelsFinished += 1;
				}

				float sampleWeight = ((float)bitCounter / 255.f) / numHIndexSamples;
				destinationPixel.r += (kir_u8)((float)samplePixel.r * sampleWeight);
				destinationPixel.g += (kir_u8)((float)samplePixel.g * sampleWeight);
				destinationPixel.b += (kir_u8)((float)samplePixel.b * sampleWeight);
			}

			sPosX += numPixelsFinished;

			if ((sPosX + 1) >= p_SourceImagePixelWidth)
			{
				sPosX = 0;
				sPosY += 1;
			}

			_K15_IRWriteRGBPixelToIndex(dPixelIndex, p_DestinationImageData, &destinationPixel);
		}

		dPosX = 0;
	}


	return K15_IR_RESULT_SUCCESS;
}

kir_internal kir_result _K15_IRUpscaleImageData(kir_u8* p_SourceImageData, kir_u32 p_SourceImagePixelWidth, 
	kir_u32 p_SourceImagePixelHeight, kir_pixel_format p_SourceImageDataPixelFormat,
	kir_u8* p_DestinationImageData, kir_u32 p_DestinationImagePixelWidth,
	kir_u32 p_DestinationImagePixelHeight, kir_pixel_format p_DestinationImageDataPixelFormat)
{
	int pixelIndex = 0;
	int numDestinationPixels = p_DestinationImagePixelHeight * p_DestinationImagePixelWidth;

	kir_u32 posX = 0;
	kir_u32 posY = 0;

	for (; posY < p_DestinationImagePixelHeight; ++posY)
	{
		for (; posX < p_DestinationImagePixelWidth; ++posX)
		{
			kir_u32 pixelIndex = posX + (posY * p_DestinationImagePixelWidth);

			float u = (float)posX / (float)p_DestinationImagePixelWidth;
			float v = (float)posY / (float)p_DestinationImagePixelHeight;

			float sourcePosX = u * (float)p_SourceImagePixelWidth;
			float sourcePosY = v * (float)p_SourceImagePixelHeight;

			int fixedSourcePosX = (int)(sourcePosX);
			int fixedSourcePosY = (int)(sourcePosY);

			float fractSourcePosX = sourcePosX - (float)fixedSourcePosX;
			float fractSourcePosY = sourcePosY - (float)fixedSourcePosY;

			int destPixelIndices[] = {
				(fixedSourcePosX + (fixedSourcePosY * p_SourceImagePixelWidth)) * 3,
				((fixedSourcePosX + 1) + (fixedSourcePosY * p_SourceImagePixelWidth)) * 3,
				(fixedSourcePosX + ((fixedSourcePosY + 1)* p_SourceImagePixelWidth)) * 3,
				((fixedSourcePosX + 1) + ((fixedSourcePosY + 1) * p_SourceImagePixelWidth)) * 3
			};

			kir_u8 texelData[4][3] = {
				{
					p_SourceImageData[destPixelIndices[0] + 0], 
					p_SourceImageData[destPixelIndices[0] + 1], 
					p_SourceImageData[destPixelIndices[0] + 2]
				},

				{
					p_SourceImageData[destPixelIndices[1] + 0], 
					p_SourceImageData[destPixelIndices[1] + 1], 
					p_SourceImageData[destPixelIndices[1] + 2]
				},

				{
					p_SourceImageData[destPixelIndices[2] + 0], 
					p_SourceImageData[destPixelIndices[2] + 1], 
					p_SourceImageData[destPixelIndices[2] + 2]
				},

				{
					p_SourceImageData[destPixelIndices[3] + 0], 
					p_SourceImageData[destPixelIndices[3] + 1], 
					p_SourceImageData[destPixelIndices[3] + 2]
				},
			};

			kir_u8 verticalSamples[2][3] = {
				{
					_K15_IRLerpU8(fractSourcePosX, texelData[0][0], texelData[1][0]),
					_K15_IRLerpU8(fractSourcePosX, texelData[0][1], texelData[1][1]),
					_K15_IRLerpU8(fractSourcePosX, texelData[0][2], texelData[1][2])
				},

				{
					_K15_IRLerpU8(fractSourcePosX, texelData[2][0], texelData[3][0]),
					_K15_IRLerpU8(fractSourcePosX, texelData[2][1], texelData[3][1]),
					_K15_IRLerpU8(fractSourcePosX, texelData[2][2], texelData[3][2])
				}
			};

			kir_u8 sample[3] = {
				_K15_IRLerpU8(fractSourcePosY, verticalSamples[0][0], verticalSamples[1][0]),
				_K15_IRLerpU8(fractSourcePosY, verticalSamples[0][1], verticalSamples[1][1]),
				_K15_IRLerpU8(fractSourcePosY, verticalSamples[0][2], verticalSamples[1][2])
			};

			// p_DestinationImageData[pixelIndex * 3 + 0] = sample[0];
			// p_DestinationImageData[pixelIndex * 3 + 1] = sample[1];
			// p_DestinationImageData[pixelIndex * 3 + 2] = sample[2];

			p_DestinationImageData[pixelIndex * 3 + 0] = sample[0];
			p_DestinationImageData[pixelIndex * 3 + 1] = sample[1];
			p_DestinationImageData[pixelIndex * 3 + 2] = sample[2];
		}

		posX = 0;
	}

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

	//if (sourceImageArea > destinationImageArea)
	{
		result = _K15_IRDownscaleImageData(p_SourceImageData, p_SourceImagePixelWidth,
			p_SourceImagePixelHeight, p_SourceImageDataPixelFormat, p_DestinationImageData,
			p_DestinationImagePixelWidth, p_DestinationImagePixelHeight, 
			p_DestinationImageDataPixelFormat);
	}
// 	else if (sourceImageArea < destinationImageArea)
// 	{
// 		result = _K15_IRUpscaleImageData(p_SourceImageData, p_SourceImagePixelWidth,
// 			p_SourceImagePixelHeight, p_SourceImageDataPixelFormat, p_DestinationImageData,
// 			p_DestinationImagePixelWidth, p_DestinationImagePixelHeight, 
// 			p_DestinationImageDataPixelFormat);
// 	}

	return result;
}

#endif //K15_IR_IMPLEMENTATION
#endif //_K15_ImageResize_h_