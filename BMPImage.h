#pragma once
#include "stdafx.h"
#include "Frame.h"

// https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef long LONG;

// https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader
typedef struct tagBITMAPFILEHEADER {
	DWORD  bfSize;
	WORD   bfReserved1;
	WORD   bfReserved2;
	DWORD  bfOffBits;
} BITMAPFILEHEADER, * LPBITMAPFILEHEADER, * PBITMAPFILEHEADER;

// https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapinfoheader
typedef struct tagBITMAPINFOHEADER {
	DWORD   biSize;
	LONG    biWidth;
	LONG    biHeight;
	WORD    biPlanes;
	WORD    biBitCount;
	DWORD   biCompression;
	DWORD   biSizeImage;
	LONG    biXPelsPerMeter;
	LONG    biYPelsPerMeter;
	DWORD   biClrUsed;
	DWORD   biClrImportant;
} BITMAPINFOHEADER, * LPBITMAPINFOHEADER, * PBITMAPINFOHEADER;

void ConvertRGBRowToYUV(const unsigned char* bitmap_colour_data, Frame& image, const int stride, const int height_index);
void ConvertRGBRowsToYUV(const unsigned char* bitmap_colour_data, Frame& image, const int stride, const int step, const int start_index);
void ConvertRGBToYUV(unsigned char* bitmap_colour_data, Frame& image, const int stride);
bool ReadBMPImageHeaders(std::ifstream& image_file, int& image_width, int& image_height, int& stride, unsigned int& bitmap_size);
bool ReadBMPImage(const char* image_name, std::unique_ptr<unsigned char>& p_bitmap_colour_data, int& image_width, int& image_height, int& stride);
