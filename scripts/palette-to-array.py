#!/bin/python

# Automatically generate a GBA palette with a 16-pixel image.
# The output is a C array of type 'const u16' and length 16.
#
# Usage: palette-to-array.py <palette_image_file> <array_name>
# The output is printed to stdout.

from sys import argv, exit
from PIL import Image

if len(argv) < 3:
    print('Usage: ' + argv[0] + ' <palette_image_file> <array_name>')
    exit()

img = Image.open(argv[1]).convert('RGB')

if img.width * img.height != 16:
    print('Error: the palette image file must contain exactly 16 pixels')
    exit()

colors = []
for i in range(16):
    pix = img.getpixel( (i % img.width, i // img.width) )

    r = pix[0] >> 3
    g = pix[1] >> 3
    b = pix[2] >> 3

    col = (b << 10) | (g << 5) | r

    color_code = '0x' + hex(col)[2:].zfill(4)
    colors.append(color_code)

# Print output
print('const u16 ' + argv[2] + '[16] = {', end='')
for i in range(16):
    if i % 8 == 0:
        print('\n    ', end='')
    print(colors[i], end='')
    if i != 15:
        print(', ', end='')
print('\n};')
