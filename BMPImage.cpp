#include "BMPImage.h"

void ConvertRGBRowToYUV(const unsigned char* bitmap_colour_data, Frame& image, const int stride, const int height_index)
{
	const unsigned int width = image.width;
	const unsigned int height = image.height;
	for (unsigned int width_index = 0; width_index < image.width; width_index++)
	{
		int RGB_point = height_index * stride + width_index * 3;

		BYTE B = bitmap_colour_data[RGB_point];
		BYTE G = bitmap_colour_data[RGB_point + 1];
		BYTE R = bitmap_colour_data[RGB_point + 2];

		// https://learn.microsoft.com/en-us/previous-versions/windows/embedded/ms893078(v=msdn.10)
		// Y = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16;
		// U = ((-38 * R - 74 * G + 112 * B + 128) >> 8) + 128;
		// V = ((112 * R - 94 * G - 18 * B + 128) >> 8) + 128;

		// Y block.
		image.data[width * (image.height - height_index - 1) + width_index] = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16;
		if (height_index % 2 == 0 && width_index % 2 == 0)			// Downsampling. Use only top-left pixel colour data to code colour of a block of four pixels.
		{
			// U block.
			image.data[width * height + width * (height - height_index - 1) / 4 + width_index / 2 - width / 4] = ((-38 * R - 74 * G + 112 * B + 128) >> 8) + 128;
			// V block.
			image.data[width * height * 5 / 4 + width * (height - height_index - 1) / 4 + width_index / 2 - width / 4] = ((112 * R - 94 * G - 18 * B + 128) >> 8) + 128;
		}
	}
}

void ConvertRGBRowsToYUV(const unsigned char* bitmap_colour_data, Frame& image, const int stride, const int step, const int start_index)
{
	for (int height_index = start_index; height_index >= 0; height_index -= step)
	{
		ConvertRGBRowToYUV(bitmap_colour_data, image, stride, height_index);
	}
}

void ConvertRGBToYUV(unsigned char* bitmap_colour_data, Frame& image, const int stride)
{
	std::vector<std::thread> threads;
	int threads_count = std::thread::hardware_concurrency();

	for (int i = 0; i < threads_count; i++)
	{
		threads.push_back(std::thread(ConvertRGBRowsToYUV, bitmap_colour_data, std::ref(image), stride, threads_count, image.height - i - 1));
	}
	for (int i = 0; i < threads_count; i++)
	{
		threads[i].join();
	}
}

bool ReadBMPImageHeaders(std::ifstream& image_file, int& image_width, int& image_height, int& stride, unsigned int& bitmap_size)
{
	BITMAPFILEHEADER bitmap_file_header;
	BITMAPINFOHEADER bitmap_info_header;

	WORD file_type = 0;
	image_file.read((char*)&(file_type), sizeof(WORD));
	if (image_file.fail())
	{
		std::cout << "An error occurred while reading BMP image." << std::endl;
		return false;
	}

	if (file_type != 0x4D42)							// 0x4D42 is BMP
	{
		std::cout << "Input image type is not .BMP" << std::endl;
		image_file.close();
		return false;
	}

	image_file.read((char*)&(bitmap_file_header), sizeof(BITMAPFILEHEADER));
	if (image_file.fail())
	{
		std::cout << "An error occurred while reading BMP image BITMAPFILEHEADER." << std::endl;
		return false;
	}

	image_file.read((char*)&bitmap_info_header, sizeof(BITMAPINFOHEADER));
	if (image_file.fail())
	{
		std::cout << "An error occurred while reading BMP image BITMAPINFOHEADER." << std::endl;
		return false;
	}

	image_file.seekg(bitmap_file_header.bfOffBits);

	if (bitmap_info_header.biWidth == 0 || bitmap_info_header.biHeight == 0)
	{
		std::cout << "An error occurred while reading BMP image. Image resolution must be at least 1x1." << std::endl;
		return false;
	}
	if (bitmap_info_header.biBitCount != 24)
	{
		std::cout << "BMP image is not 24 bpp." << std::endl;
		return false;
	}

	bitmap_size = bitmap_info_header.biSizeImage;
	if (bitmap_info_header.biSizeImage == 0)	// biSizeImage specifies the size, in bytes, of the image. This can be set to 0 for uncompressed RGB bitmaps.
		bitmap_size = bitmap_info_header.biWidth * bitmap_info_header.biHeight * bitmap_info_header.biBitCount / 8;

	image_width = bitmap_info_header.biWidth;
	image_height = bitmap_info_header.biHeight;
	stride = ((((bitmap_info_header.biWidth * bitmap_info_header.biBitCount) + 31) & ~31) >> 3);

	return true;
}

bool ReadBMPImage(const char* image_name, std::unique_ptr<unsigned char>& p_bitmap_colour_data, int& image_width, int& image_height, int& stride)
{
	std::ifstream image_file;
	image_file.open(image_name, std::fstream::binary);
	if (!image_file.is_open())
	{
		std::cout << "Cannot open BMP image.";
		return false;
	}

	unsigned int bitmap_size;
	if (!ReadBMPImageHeaders(image_file, image_width, image_height, stride, bitmap_size))
	{
		return false;
	}

	p_bitmap_colour_data.reset(new BYTE[bitmap_size]);
	image_file.read((char*)p_bitmap_colour_data.get(), bitmap_size);

	if (image_file.fail())
	{
		std::cout << "An error occurred while reading BMP image bitmap data." << std::endl;
		return false;
	}
	image_file.close();
	return true;
}