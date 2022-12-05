#include "stdafx.h"
#include "Frame.h"
#include "BMPImage.h"

int main(int argc, char* argv[])
{
	unsigned int video_resolution_width = 0;
	unsigned int video_resolution_height = 0;
	char* image_filename = NULL;
	char* input_filename = NULL;
	char* output_filename = NULL;

	if (argc == 1)
	{
		std::cout << "-w -h -i -fi -fo" << std::endl;
		std::cout << "-w Horizontal resolution of video." << std::endl;
		std::cout << "-h Vertical resolution of video." << std::endl;
		std::cout << "-i BMP image filename." << std::endl;
		std::cout << "-fi Input video filename." << std::endl;
		std::cout << "-fo Output video filename." << std::endl;
	}

	for (int i = 0; i < argc; i++)
	{
		if (!strcmp(argv[i], "-i"))
			if (i + 1 < argc)
				image_filename = argv[i + 1];
			else
			{
				std::cout << "Missing image file name." << std::endl;
				return -1;
			}

		if (!strcmp(argv[i], "-fi"))
			if (i + 1 < argc)
				input_filename = argv[i + 1];
			else
			{
				std::cout << "Missing input file name." << std::endl;
				return -1;
			}

		if (!strcmp(argv[i], "-fo"))
			if (i + 1 < argc)
				output_filename = argv[i + 1];
			else
			{
				std::cout << "Missing output file name." << std::endl;
				return -1;
			}

		if (!strcmp(argv[i], "-w"))
			if (i + 1 < argc)
			{
				int tmp = atoi(argv[i + 1]);
				if (tmp > 0)
					video_resolution_width = tmp;
				else
				{
					std::cout << "Incorrect horizontal resolution of video." << std::endl;
					return -1;
				}
			}
			else
			{
				std::cout << "Missing horizontal resolution of video." << std::endl;
				return -1;
			}

		if (!strcmp(argv[i], "-h"))
			if (i + 1 < argc)
			{
				int tmp = atoi(argv[i + 1]);
				if (tmp > 0)
					video_resolution_height = tmp;
				else
				{
					std::cout << "Incorrect vertical resolution of video." << std::endl;
					return -1;
				}
			}
			else
			{
				std::cout << "Missing vertical resolution of video." << std::endl;
				return -1;
			}

		if (!strcmp(argv[i], "-?"))
		{
			std::cout << "-w -h -i -fi -fo" << std::endl;
			std::cout << "-w Horizontal resolution of video." << std::endl;
			std::cout << "-h Vertical resolution of video." << std::endl;
			std::cout << "-i BMP image filename." << std::endl;
			std::cout << "-fi Input video filename." << std::endl;
			std::cout << "-fo Output video filename." << std::endl;
		}
	}

	if (video_resolution_width == 0 || video_resolution_height == 0 || image_filename == NULL || input_filename == NULL || output_filename == NULL)
	{
		std::cout << "Missing some of input arguments." << std::endl;
		return -1;
	}

	unsigned int frame_size = video_resolution_width * video_resolution_height * 3 / 2;
	int image_width;
	int image_height;
	int stride;

	std::unique_ptr<unsigned char> p_bitmap_colour_data;
	if (!ReadBMPImage(image_filename, p_bitmap_colour_data, image_width, image_height, stride))
		return -1;

	Frame image(image_width, image_height);

	ConvertRGBToYUV(p_bitmap_colour_data.get(), image, stride);

	std::ifstream input_video_file(input_filename, std::ifstream::binary);
	if (!input_video_file.is_open())
	{
		std::cout << "Cannot open input video." << std::endl;
		return -1;
	}
	std::ofstream output_video_file(output_filename, std::ofstream::binary | std::ofstream::trunc);
	if (!output_video_file.is_open())
	{
		std::cout << "Cannot open output video." << std::endl;
		return -1;
	}

	// Top left coordinate of image on video.
	Point top_left_pos{ 0, 0 };

	Frame video_frame(video_resolution_width, video_resolution_height);

	input_video_file.seekg(0, std::ios::end);
	int frames_count = input_video_file.tellg() / frame_size;
	if (input_video_file.tellg() % frame_size)
		std::cout << "The quantity of frames in the input YUV file is not integer." << std::endl;
	input_video_file.seekg(0, std::ios::beg);

	for (int i = 0; i < frames_count; i++)
	{
		input_video_file.read(video_frame.data, frame_size);
		if (input_video_file.fail())
		{
			std::cout << "An error occurred while reading input YUV file." << std::endl;
			return -1;
		}
		if (!OverlayImageOnVideo(image, video_frame, top_left_pos))
		{
			std::cout << "Failed overlaying image on video." << std::endl;
			return -1;
		}
		output_video_file.write(video_frame.data, video_frame.size);
	}

	input_video_file.close();
	output_video_file.close();

	return 0;
}
