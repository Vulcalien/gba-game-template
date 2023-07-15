#!/bin/python

# Automatically generate a GBA palette with a 16-pixel image.
# The output is a C array of type 'const u16' and length 16.
#
# Usage: palette-to-array.py <palette_image_file> <array_name>

import sys, argparse
from PIL import Image

# Setup argparse
parser = argparse.ArgumentParser(
    description='Generate a GBA palette from a 16-pixel image'
)

parser.add_argument('palette_filename',
                    type=argparse.FileType('rb'),
                    help='specify the filename of the palette image')
parser.add_argument('array_name',
                    help='specify the name of the output array')

parser.add_argument('-o', '--output',
                    type=argparse.FileType('w'), default=sys.stdout,
                    help='specify the output file (default: stdout)')

args = parser.parse_args()

# Open and validate image
img = Image.open(args.palette_filename).convert('RGB')

if img.width * img.height != 16:
    print('Error: the palette image file must contain exactly 16 pixels')
    exit()

# Get and convert colors
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
f = args.output

f.write('const u16 ' + args.array_name + '[16] = {')
for i in range(16):
    if i % 8 == 0:
        f.write('\n    ')
    f.write(colors[i])
    if i != 15:
        f.write(', ')
f.write('\n};\n')
