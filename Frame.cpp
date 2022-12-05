#include "Frame.h"

bool CheckIntersections(const Frame& image, const Frame& video_frame, const Point top_left_pos, Point& down_right_pos)
{
	if (top_left_pos.x >= video_frame.width || top_left_pos.y >= video_frame.height)	// Check if point (top_left_pos.x, top_left_pos.y) is in the video resolution range.
	{
		std::cout << "Point (" << top_left_pos.x << ", " << top_left_pos.y << ") is out of the video resolution range." << std::endl;
		return false;
	}
	if (top_left_pos.x + image.width < video_frame.width)
		down_right_pos.x = top_left_pos.x + image.width;
	else
		down_right_pos.x = video_frame.width;
	if (top_left_pos.y + image.height < video_frame.height)
		down_right_pos.y = top_left_pos.y + image.height;
	else
		down_right_pos.y = video_frame.height;

	return true;
}

void OverlayImageOnFrameRows(const Frame& image, Frame& video_frame, const Point top_left_pos, const Point down_right_pos, const int step, const int initial_shift)
{
	for (unsigned int height_index = top_left_pos.y + initial_shift; height_index < down_right_pos.y; height_index += step)
	{
		// Overlaying Y block.
		memcpy(&video_frame.data[video_frame.width * height_index + top_left_pos.x], &image.data[image.width * (height_index - top_left_pos.y)], down_right_pos.x - top_left_pos.x);
		//Overlaying U block.
		memcpy(&video_frame.data[video_frame.start_of_block_U + height_index / 2 * video_frame.width / 2 + top_left_pos.x / 2],
			&image.data[image.start_of_block_U + image.width * (height_index / 2 - top_left_pos.y / 2) / 2],
			(down_right_pos.x - top_left_pos.x) / 2);
		//Overlaying V block.
		memcpy(&video_frame.data[video_frame.start_of_block_V + height_index / 2 * video_frame.width / 2 + top_left_pos.x / 2],
			&image.data[image.start_of_block_V + image.width * (height_index / 2 - top_left_pos.y / 2) / 2],
			(down_right_pos.x - top_left_pos.x) / 2);
	}
}

void OverlayImageOnFrame(const Frame& image, Frame& video_frame, const Point top_left_pos, const Point down_right_pos)
{
	std::vector<std::thread> threads;
	int threads_count = std::thread::hardware_concurrency();
	for (int i = 0; i < threads_count; i++)
	{
		threads.push_back(std::thread(OverlayImageOnFrameRows, std::ref(image), std::ref(video_frame), top_left_pos, down_right_pos, threads_count, i));
	}
	for (int i = 0; i < threads_count; i++)
	{
		threads[i].join();
	}
}

bool OverlayImageOnVideo(const Frame& image, Frame& video_frame, const Point top_left_pos)
{
	Point down_right_pos;
	if (!CheckIntersections(image, video_frame, top_left_pos, down_right_pos))
		return false;
	OverlayImageOnFrame(image, video_frame, top_left_pos, down_right_pos);
	return true;
}