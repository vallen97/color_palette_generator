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
    double normR = r / 255.0;
    double normG = g / 255.0;
    double normB = b / 255.0;


    double cMax = std::max({ normR, normG, normB });
    double cMin = std::min({ normR, normG, normB });
    double delta = cMax - cMin;

    double h = 0.0;
    double s = 0.0;
    double v = cMax * 100.0; 


    if (delta == 0.0) {
        h = 0.0;
    }
    else if (cMax == normR) {
        h = 60.0 * std::fmod(((normG - normB) / delta), 6.0);
    }
    else if (cMax == normG) {
        h = 60.0 * (((normB - normR) / delta) + 2.0);
    }
    else if (cMax == normB) {
        h = 60.0 * (((normR - normG) / delta) + 4.0);
    }


    if (h < 0.0) {
        h += 360.0;
    }


    if (cMax != 0.0) {
        s = (delta / cMax) * 100.0;
    }

    return { h, s, v };
}


ColorRGB hsvToRgb(double h, double s, double v) {

    double normS = s / 100.0;
    double normV = v / 100.0;

    double c = normV * normS; 

    double x = c * (1.0 - std::abs(std::fmod(h / 60.0, 2.0) - 1.0));
    double m = normV - c; 

    double rPrime = 0, gPrime = 0, bPrime = 0;


    if (h >= 0 && h < 60) {
        rPrime = c; gPrime = x; bPrime = 0;
    }
    else if (h >= 60 && h < 120) {
        rPrime = x; gPrime = c; bPrime = 0;
    }
    else if (h >= 120 && h < 180) {
        rPrime = 0; gPrime = c; bPrime = x;
    }
    else if (h >= 180 && h < 240) {
        rPrime = 0; gPrime = x; bPrime = c;
    }
    else if (h >= 240 && h < 300) {
        rPrime = x; gPrime = 0; bPrime = c;
    }
    else if (h >= 300 && h < 360) {
        rPrime = c; gPrime = 0; bPrime = x;
    }


    int r = static_cast<int>(std::round((rPrime + m) * 255.0));
    int g = static_cast<int>(std::round((gPrime + m) * 255.0));
    int b = static_cast<int>(std::round((bPrime + m) * 255.0));

    return { r, g, b };
}


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
    int width, height, channels;


    unsigned char* img = stbi_load("forest.png", &width, &height, &channels, 0);

    if (img == NULL) {
        std::cerr << "Error: Could not load the image file." << std::endl;
        exit(1);
    }

    std::cout << "\nLoaded image: " << width << "px x " << height << "px, "
        << channels << " channels" << std::endl;


    std::vector<std::vector<ColorRGB>> pixelData(height, std::vector<ColorRGB>(width));

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int pixelIndex = (i * width + j) * channels;
            pixelData[i][j].r = img[pixelIndex];
            pixelData[i][j].g = img[pixelIndex + 1];
            pixelData[i][j].b = img[pixelIndex + 2];
        }
    }

    stbi_image_free(img);


    unsigned long long totalR = 0, totalG = 0, totalB = 0;

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


    ColorHSV baseHsv = rgbToHsv(avgGlobalColor.r, avgGlobalColor.g, avgGlobalColor.b);
    std::vector<double> harmoniousHues = getGoldenAngles(baseHsv.h);

    std::cout << "\n--- Golden Angle Palette ---" << std::endl;
    for (size_t i = 0; i < harmoniousHues.size(); i++) {

        ColorRGB finalColor = hsvToRgb(harmoniousHues[i], baseHsv.s, baseHsv.v);

        std::cout << "Color " << i + 1 << ": HSV("
            << std::fixed << std::setprecision(1) << harmoniousHues[i] << ", "
            << baseHsv.s << ", " << baseHsv.v << ")  =>  RGB("
            << finalColor.r << ", " << finalColor.g << ", " << finalColor.b << ")\n";
        // Write to console of the color
        std::cout << "\033[38;2;" << finalColor.r << ";" << finalColor.g << ";" << finalColor.b << "m";

    }
    std::cout << "\033[0m" << std::endl;
    std::cout << "This text is rendered in custom RGB color!" << std::endl;

    return 0;
}