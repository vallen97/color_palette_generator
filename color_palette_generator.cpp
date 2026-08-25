// color_palette_generator.cpp : This file contains the 'main' function.

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <iomanip>

#define STB_IMAGE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"


struct ColorRGB {
	int r, g, b;
};

struct ColorHSV {
	double h, s, v;
};



ColorHSV rgbToHsv(int r, int g, int b) {
	// Normalize RGB values
	double normR = r / 255.0;
	double normG = g / 255.0;
	double normB = b / 255.0;

	//Getting the biggest and smallest from the normalized values
	double cMax = std::max({ normR, normG, normB });
	double cMin = std::min({ normR, normG, normB });
	double delta = cMax - cMin;

	double hue = 0.0;
	double saturation = 0.0;
	double value = cMax * 100.0;


	if (delta == 0.0) {
		hue = 0.0;
	}
	else if (cMax == normR) {
		hue = 60.0 * std::fmod(((normG - normB) / delta), 6.0);
	}
	else if (cMax == normG) {
		hue = 60.0 * (((normB - normR) / delta) + 2.0);
	}
	else if (cMax == normB) {
		hue = 60.0 * (((normR - normG) / delta) + 4.0);
	}

	if (hue < 0.0) {
		hue += 360.0;
	}

	if (cMax != 0.0) {
		saturation = (delta / cMax) * 100.0;
	}

	return { hue, saturation,value };
}


ColorRGB hsvToRgb(double hue, double saturation, double value) {
	// Normalize percentages
	double normS = saturation / 100.0;
	double normV = value / 100.0;

	double chroma = normV * normS;

	double x = chroma * (1.0 - std::abs(std::fmod(hue / 60.0, 2.0) - 1.0));
	// Brightness
	double brightness = normV - chroma;

	double rPrime = 0;
	double gPrime = 0;
	double bPrime = 0;


	if (hue >= 0 && hue < 60) {
		rPrime = chroma;
		gPrime = x;
		bPrime = 0;
	}
	else if (hue >= 60 && hue < 120) {
		rPrime = x;
		gPrime = chroma;
		bPrime = 0;
	}
	else if (hue >= 120 && hue < 180) {
		rPrime = 0;
		gPrime = chroma;
		bPrime = x;
	}
	else if (hue >= 180 && hue < 240) {
		rPrime = 0;
		gPrime = x;
		bPrime = chroma;
	}
	else if (hue >= 240 && hue < 300) {
		rPrime = x;
		gPrime = 0;
		bPrime = chroma;
	}
	else if (hue >= 300 && hue < 360) {
		rPrime = chroma;
		gPrime = 0;
		bPrime = x;
	}


	int r = static_cast<int>(std::round((rPrime + brightness) * 255.0));
	int g = static_cast<int>(std::round((gPrime + brightness) * 255.0));
	int b = static_cast<int>(std::round((bPrime + brightness) * 255.0));

	return { r, g, b };
}


// Returns the original hue, full clockwise, counter clockwise, half clockwise, and counter clockwise
std::vector<double> getGoldenAngles(double baseHue) {
	double goldenAngle = 137.5;
	double halfGolden = goldenAngle / 2.0;


	double cwFull = std::fmod((baseHue + goldenAngle), 360.0);
	double ccwFull = std::fmod((baseHue - goldenAngle + 360.0), 360.0);
	double cwHalf = std::fmod((baseHue + halfGolden), 360.0);
	double ccwHalf = std::fmod((baseHue - halfGolden + 360.0), 360.0);

	return { baseHue, cwFull, ccwFull, cwHalf, ccwHalf };
}


int main()
{
	int width = 0;
	int height = 0;
	int channels = 0;

	std::string imagePath;
	std::cout << "Enter the image file path: ";
	std::cin >> imagePath; // Stops reading at the first space

	// load image
	unsigned char* img = stbi_load(imagePath.c_str(), &width, &height, &channels, 0);

	if (img == NULL) {
		std::cerr << "Error: Could not load the image file." << std::endl;
		exit(1);
	}

	std::cout << "\nLoaded image: " << width << "px x " << height << "px, " << channels << " channels" << std::endl;


	// Parse image RGB data into vector
	std::vector<std::vector<ColorRGB>> pixelData(height, std::vector<ColorRGB>(width));

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int pixelIndex = (i * width + j) * channels;
			pixelData[i][j].r = img[pixelIndex];
			pixelData[i][j].g = img[pixelIndex + 1];
			pixelData[i][j].b = img[pixelIndex + 2];
		}
	}
	// Free the image now that we are done with the image.
	stbi_image_free(img);

	// Just incase the image loaded has a very large pixel count
	unsigned long long totalR = 0;
	unsigned long long totalG = 0;
	unsigned long long totalB = 0;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			totalR += pixelData[i][j].r;
			totalG += pixelData[i][j].g;
			totalB += pixelData[i][j].b;
		}
	}

	int totalPixels = height * width;
	ColorRGB avgGlobalColor = {
		static_cast<int>(totalR / totalPixels),
		static_cast<int>(totalG / totalPixels),
		static_cast<int>(totalB / totalPixels)
	};

	// Generate colors from golden angles
	ColorHSV baseHsv = rgbToHsv(avgGlobalColor.r, avgGlobalColor.g, avgGlobalColor.b);
	std::vector<double> harmoniousHues = getGoldenAngles(baseHsv.h);

	std::cout << "\n--- Golden Angle Palette ---" << std::endl;
	for (size_t i = 0; i < harmoniousHues.size(); i++) {

		ColorRGB finalColor = hsvToRgb(harmoniousHues[i], baseHsv.s, baseHsv.v);

		// Display the RGB values and colors to the console
		std::cout << "\033[38;2;" << finalColor.r << ";" << finalColor.g << ";" << finalColor.b << "m";

		std::cout << "Color " << i + 1 << ": HSV(" << std::fixed << std::setprecision(1) << harmoniousHues[i] << ", "
			<< baseHsv.s << ", " << baseHsv.v << ")  =>  RGB(" << finalColor.r << ", " << finalColor.g << ", " << finalColor.b << ")\n";

		 std::cout << "\033[0m";

	}

	return 0;
}