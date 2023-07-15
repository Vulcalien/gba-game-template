#!/bin/python

# Convert a 16-pixel image into a GBA palette array.
# The output is a C array of type 'const u16' and length 16.
#
# Run 'palette-to-array.py -h' for help.

import sys, argparse
from sys import exit
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
    exit('Error: the palette image file must contain exactly 16 pixels')

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

# Write output
f = args.output

f.write('const u16 ' + args.array_name + '[16] = {')
for i in range(16):
    if i % 8 == 0:
        f.write('\n    ')
    f.write(colors[i])
    if i != 15:
        f.write(', ')
f.write('\n};\n')
