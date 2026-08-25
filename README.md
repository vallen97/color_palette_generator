# Golden Angle Color Palette Generator
## Project Overview
This project is a C++ command line tool designed to parse an image, extract its global average color, and procedurally generate a harmonious, organic color palette. This tool uses the [Golden Angle](https://en.wikipedia.org/wiki/Golden_angle) (137.5°) a mathematical proportion derived from the Fibonacci sequence to calculate complementary hues that feel naturally balanced.

Building an application that loads an image, samples a base color, and generates a palette. 

## Core Features
- Image Parsing: Loads standard image files using [stb](https://github.com/nothings/stb) and parses their raw 1D byte arrays into structured C++ objects.

- Color Space Conversion: Translates standard RGB pixel values into the HSV (Hue, Saturation, Value) color space. Image pixels are decoded as RGB bytes, but deriving tints, shades, and complementary colors is far simpler when transformed into HSB/HSL models.

- Golden Angle Mathematics: Applies full (137.5°) and half (68.75°) Golden Angle offsets to a base hue to generate four distinct, mathematically harmonious colors.

- RGB Reassembly: calculates HSV values back into the RGB color space for final output and rendering.


