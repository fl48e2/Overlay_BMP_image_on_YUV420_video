#pragma once
#include "stdafx.h"

class Frame
{
public:
	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int size = 0;
	unsigned int start_of_block_U = 0;
	unsigned int start_of_block_V = 0;
	char* data;

	Frame(const unsigned int in_width, const unsigned int in_height)
	{
		width = in_width;
		height = in_height;
		size = width * height * 3 / 2;
		start_of_block_U = width * height;
		start_of_block_V = width * height * 5 / 4;
		data = new char[size];
	}

	~Frame()
	{
		delete[] data;
	}
};

struct Point
{
	unsigned int x;
	unsigned int y;
};

bool CheckIntersections(const Frame& image, const Frame& video_frame, const Point top_left_pos, Point& down_right_pos);		// Calculating the visible area of an image on a frame.
void OverlayImageOnFrameRows(const Frame& image, Frame& video_frame, const Point top_left_pos, const Point down_right_pos, const int step, const int initial_shift);
void OverlayImageOnFrame(const Frame& image, Frame& video_frame, const Point top_left_pos, const Point down_right_pos);
bool OverlayImageOnVideo(const Frame& image, Frame& video_frame, const Point top_left_pos);
