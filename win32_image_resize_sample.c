#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#define K15_FALSE 0
#define K15_TRUE 1

typedef unsigned char bool8;
typedef unsigned char byte;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

#ifndef RESIZE_DLL

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

HDC backbufferDC = 0;
HBITMAP backbufferBitmap = 0;
uint32 screenWidth = 1024;
uint32 screenHeight = 768;
uint32 timePerFrameInMS = 16;

uint32 sourceImageWidth = 0;
uint32 sourceImageHeight = 0;
uint32 destinationImageWidth = 0;
uint32 destinationImageHeight = 0;
unsigned char* sourceImageData = 0;
unsigned char* sourceImageDataBGR = 0;
unsigned char* destinationImageData = 0;
unsigned char* destinationImageDataBGR = 0;

typedef void(*resizeFunctionType)(unsigned char*, uint32, uint32, int, unsigned char*, uint32,
	uint32, int);

unsigned char* convertToBGR(unsigned char* p_PixelData, uint32 p_Width, uint32 p_Height)
{
	uint32 numPixels = p_Width * p_Height;
	uint32 pixelDataSizeInBytes = numPixels * 4;

	unsigned char* dataBuffer = (unsigned char*)malloc(pixelDataSizeInBytes);
	memset(dataBuffer, 0, pixelDataSizeInBytes);

	uint32 pixelIndex = 0;
	for (; pixelIndex < numPixels; ++pixelIndex)
	{
		unsigned char r = p_PixelData[pixelIndex * 3 + 0];
		unsigned char g = p_PixelData[pixelIndex * 3 + 1];
		unsigned char b = p_PixelData[pixelIndex * 3 + 2];

		dataBuffer[pixelIndex * 4 + 0] = b;
		dataBuffer[pixelIndex * 4 + 1] = g;
		dataBuffer[pixelIndex * 4 + 2] = r;
		dataBuffer[pixelIndex * 4 + 3] = 0;
	}

	return dataBuffer;
}

void resizeImage()
{
	HMODULE resizeLib = LoadLibraryA("k15_resize.dll");

	if (resizeLib)
	{
		resizeFunctionType resizeFunction = 
			(resizeFunctionType)GetProcAddress(resizeLib, "resizeFunction");

		if (resizeFunction)
		{
			destinationImageWidth = sourceImageWidth / 2;
			destinationImageHeight = sourceImageHeight / 2;

			if (destinationImageData)
			{
				free(destinationImageData);
				destinationImageData = 0;
			}

			destinationImageData = (unsigned char*)malloc(destinationImageHeight * destinationImageWidth * 3);

			resizeFunction(sourceImageData, sourceImageWidth, sourceImageHeight, 
				3, destinationImageData, destinationImageWidth, 
				destinationImageHeight, 3);

			free(destinationImageDataBGR);
			destinationImageDataBGR = convertToBGR(destinationImageData, 
				destinationImageWidth, destinationImageHeight);

			BOOL result = FreeLibrary(resizeLib);

			if (!result)
			{
				printf("Erro!");
			}
		}
	}
}

void printErrorToFile(const char* p_FileName)
{
	DWORD errorId = GetLastError();
	char* textBuffer = 0;
	DWORD writtenChars = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, errorId, 
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPSTR)&textBuffer, 512, 0);

	if (writtenChars > 0)
	{
		FILE* file = fopen(p_FileName, "w");

		if (file)
		{
			fwrite(textBuffer, writtenChars, 1, file);			
			fflush(file);
			fclose(file);
		}
	}
}

void allocateDebugConsole()
{
	AllocConsole();
	AttachConsole(ATTACH_PARENT_PROCESS);
	freopen("CONOUT$", "w", stdout);
}

void resizeBackbuffer(HWND p_HWND, uint32 p_Width, uint32 p_Height);

void K15_WindowCreated(HWND p_HWND, UINT p_Message, WPARAM p_wParam, LPARAM p_lParam)
{

}

void K15_WindowClosed(HWND p_HWND, UINT p_Message, WPARAM p_wParam, LPARAM p_lParam)
{

}

void K15_KeyInput(HWND p_HWND, UINT p_Message, WPARAM p_wParam, LPARAM p_lParam)
{

}

void K15_MouseButtonInput(HWND p_HWND, UINT p_Message, WPARAM p_wParam, LPARAM p_lParam)
{
	if (p_Message == WM_MBUTTONDOWN)
	{
		resizeImage();
	}
}

void K15_MouseMove(HWND p_HWND, UINT p_Message, WPARAM p_wParam, LPARAM p_lParam)
{

}

void K15_MouseWheel(HWND p_HWND, UINT p_Message, WPARAM p_wParam, LPARAM p_lParam)
{

}

void K15_WindowResized(HWND p_HWND, UINT p_Messaeg, WPARAM p_wParam, LPARAM p_lParam)
{
	WORD newWidth = (WORD)(p_lParam);
	WORD newHeight = (WORD)(p_lParam >> 16);

	resizeBackbuffer(p_HWND, newWidth, newHeight);
}

LRESULT CALLBACK K15_WNDPROC(HWND p_HWND, UINT p_Message, WPARAM p_wParam, LPARAM p_lParam)
{
	bool8 messageHandled = K15_FALSE;

	switch (p_Message)
	{
	case WM_CREATE:
		K15_WindowCreated(p_HWND, p_Message, p_wParam, p_lParam);
		break;

	case WM_CLOSE:
		K15_WindowClosed(p_HWND, p_Message, p_wParam, p_lParam);
		PostQuitMessage(0);
		messageHandled = K15_TRUE;
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		K15_KeyInput(p_HWND, p_Message, p_wParam, p_lParam);
		break;

	case WM_SIZE:
		K15_WindowResized(p_HWND, p_Message, p_wParam, p_lParam);
		break;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_XBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
		K15_MouseButtonInput(p_HWND, p_Message, p_wParam, p_lParam);
		break;

	case WM_MOUSEMOVE:
		K15_MouseMove(p_HWND, p_Message, p_wParam, p_lParam);
		break;

	case WM_MOUSEWHEEL:
		K15_MouseWheel(p_HWND, p_Message, p_wParam, p_lParam);
		break;
	}

	if (messageHandled == K15_FALSE)
	{
		return DefWindowProc(p_HWND, p_Message, p_wParam, p_lParam);
	}

	return 0;
}

HWND setupWindow(HINSTANCE p_Instance, int p_Width, int p_Height)
{
	WNDCLASS wndClass = {0};
	wndClass.style = CS_HREDRAW | CS_OWNDC | CS_VREDRAW;
	wndClass.hInstance = p_Instance;
	wndClass.lpszClassName = "K15_Win32Template";
	wndClass.lpfnWndProc = K15_WNDPROC;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&wndClass);

	HWND hwnd = CreateWindowA("K15_Win32Template", "Win32 Template",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		p_Width, p_Height, 0, 0, p_Instance, 0);

	if (hwnd == INVALID_HANDLE_VALUE)
		MessageBox(0, "Error creating Window.\n", "Error!", 0);
	else
	{
		allocateDebugConsole();
		ShowWindow(hwnd, SW_SHOW);
	}
	return hwnd;
}

uint32 getTimeInMilliseconds(LARGE_INTEGER p_PerformanceFrequency)
{
	LARGE_INTEGER appTime = {0};
	QueryPerformanceCounter(&appTime);

	appTime.QuadPart *= 1000; //to milliseconds

	return (uint32)(appTime.QuadPart / p_PerformanceFrequency.QuadPart);
}

void resizeBackbuffer(HWND p_HWND, uint32 p_Width, uint32 p_Height)
{
	DeleteObject(backbufferBitmap);

	HDC originalDC = GetDC(p_HWND);
	backbufferBitmap = CreateCompatibleBitmap(originalDC, p_Width, p_Height);
	screenWidth = p_Width;
	screenHeight = p_Height;
	
	SelectObject(backbufferDC, backbufferBitmap);
}

void setup(HWND p_HWND)
{	
	HDC originalDC = GetDC(p_HWND);
	backbufferDC = CreateCompatibleDC(originalDC);
	resizeBackbuffer(p_HWND, screenWidth, screenHeight);

	int sourceImageColorComponents = 0;
	sourceImageData = stbi_load("image2.png", &sourceImageWidth, &sourceImageHeight, 
		&sourceImageColorComponents, 0);

	free(sourceImageDataBGR);
	sourceImageDataBGR = convertToBGR(sourceImageData, 
		sourceImageWidth, sourceImageHeight);

	resizeImage();
}

void swapBuffers(HWND p_HWND)
{
	HDC originalDC = GetDC(p_HWND);
	
	//blit to front buffer
	BitBlt(originalDC, 0, 0, screenWidth, screenHeight, backbufferDC, 0, 0, SRCCOPY);

	//clear backbuffer
	BitBlt(backbufferDC, 0, 0, screenWidth, screenHeight, backbufferDC, 0, 0, BLACKNESS);
}

void drawDeltaTime(uint32 p_DeltaTimeInMS)
{
	RECT textRect;
	textRect.left = 70;
	textRect.top = 70;
	textRect.bottom = screenHeight;
	textRect.right = screenWidth;

	char messageBuffer[64];
	SetTextColor(backbufferDC, RGB(255, 255, 255));
	SetBkColor(backbufferDC, RGB(0, 0, 0));

	sprintf_s(messageBuffer, 64, "MS: %d", p_DeltaTimeInMS);
	DrawTextA(backbufferDC, messageBuffer, -1, &textRect, DT_LEFT | DT_TOP);
}

void drawImages(HWND p_HWND)
{
	int scaleFactor = 1;

	int posX1 = 40;
	int posY1 = 40;

	int posX2 = posX1 + (sourceImageWidth * scaleFactor) + 20;
	int posY2 = posY1;

	BITMAPINFO sourceImageInfo = {0};
	sourceImageInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	sourceImageInfo.bmiHeader.biWidth = sourceImageWidth;
	sourceImageInfo.bmiHeader.biHeight = sourceImageHeight * -1;
	sourceImageInfo.bmiHeader.biPlanes = 1;
	sourceImageInfo.bmiHeader.biBitCount = 32;

	StretchDIBits(backbufferDC, posX1, posY1, 
		sourceImageWidth * scaleFactor, sourceImageHeight * scaleFactor, 0, 0, 
		sourceImageWidth, sourceImageHeight, sourceImageDataBGR, 
		&sourceImageInfo, DIB_RGB_COLORS, SRCCOPY);

	if (destinationImageData)
	{
		BITMAPINFO destinationImageInfo = {0};
		destinationImageInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		destinationImageInfo.bmiHeader.biWidth = destinationImageWidth;
		destinationImageInfo.bmiHeader.biHeight = destinationImageHeight * -1;
		destinationImageInfo.bmiHeader.biPlanes = 1;
		destinationImageInfo.bmiHeader.biBitCount = 32;

		StretchDIBits(backbufferDC, posX2, posY2, 
			destinationImageWidth * scaleFactor, 
			destinationImageHeight * scaleFactor, 0, 0, 
			destinationImageWidth, destinationImageHeight, 
			destinationImageDataBGR, 
			&destinationImageInfo, DIB_RGB_COLORS, SRCCOPY);
	}
}

FILETIME libraryWriteTime = {0};

void checkLibrary()
{
	HANDLE libraryFileHandle = CreateFileA("k15_resize.dll", GENERIC_READ, 0, 0, 
		OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, 0);

	if (libraryFileHandle != INVALID_HANDLE_VALUE)
	{
		FILETIME newLibraryWriteTime = {0};
		GetFileTime(libraryFileHandle, 0, 0, &newLibraryWriteTime);
		CloseHandle(libraryFileHandle);

		if (CompareFileTime(&libraryWriteTime, &newLibraryWriteTime) == -1)
		{
			memcpy(&libraryWriteTime, &newLibraryWriteTime, sizeof(FILETIME));
			resizeImage();
		}
	}
}

void doFrame(uint32 p_DeltaTimeInMS, HWND p_HWND)
{
	checkLibrary();
	drawImages(p_HWND);
	swapBuffers(p_HWND);
}

int CALLBACK WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nShowCmd)
{
	LARGE_INTEGER performanceFrequency;
	QueryPerformanceFrequency(&performanceFrequency);

	HWND hwnd = setupWindow(hInstance, screenWidth, screenHeight);

	if (hwnd == INVALID_HANDLE_VALUE)
		return -1;

	setup(hwnd);

	uint32 timeFrameStarted = 0;
	uint32 timeFrameEnded = 0;
	uint32 deltaMs = 0;

	bool8 loopRunning = K15_TRUE;
	MSG msg = {0};

	while (loopRunning)
	{
		timeFrameStarted = getTimeInMilliseconds(performanceFrequency);

		while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				loopRunning = K15_FALSE;
		}

		doFrame(deltaMs, hwnd);

		timeFrameEnded = getTimeInMilliseconds(performanceFrequency);
		deltaMs = timeFrameEnded - timeFrameStarted;

		if (deltaMs < timePerFrameInMS)
			Sleep(timePerFrameInMS - deltaMs);
	}

	return 0;
}

#else
# define K15_IR_IMPLEMENTATION
# include "k15_image_resize.h"

extern void resizeFunction(kir_u8* p_SourceImageData, kir_u32 p_SourceImagePixelWidth, 
	kir_u32 p_SourceImagePixelHeight, kir_pixel_format p_SourceImageDataPixelFormat,
	kir_u8* p_DestinationImageData, kir_u32 p_DestinationImagePixelWidth,
	kir_u32 p_DestinationImagePixelHeight, kir_pixel_format p_DestinationImageDataPixelFormat)
{
	K15_IRScaleImageData(p_SourceImageData, p_SourceImagePixelWidth, 
		p_SourceImagePixelHeight, p_SourceImageDataPixelFormat,
		p_DestinationImageData, p_DestinationImagePixelWidth,
		p_DestinationImagePixelHeight, p_DestinationImageDataPixelFormat);
}


#endif //RESIZE_DLL