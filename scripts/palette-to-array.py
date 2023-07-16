#!/bin/python

# Copyright 2023 Vulcalien
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# ==================================================================== #

# Convert an image into a GBA palette array.
# The output is a C array of type 'const u16' and the size of the image
# as length.
#
# Run 'palette-to-array.py -h' for help.

import sys, argparse
from sys import exit
from PIL import Image

# Setup argparse
parser = argparse.ArgumentParser(
    description='Generate a GBA palette from an image'
)

parser.add_argument('palette_filename',
                    type=argparse.FileType('rb'),
                    help='specify the filename of the palette image')
parser.add_argument('array_name',
                    help='specify the name of the output array')

parser.add_argument('-o', '--output',
                    type=argparse.FileType('w'), default=sys.stdout,
                    help='specify the output file (default: stdout)')
parser.add_argument('-s', '--static',
                    action=argparse.BooleanOptionalAction,
                    help='add the \'static\' modifier to the output ' +
                         'array')

args = parser.parse_args()

# Open and validate image
img = Image.open(args.palette_filename).convert('RGB')

palette_size = img.width * img.height

# Get and convert colors
colors = []
for i in range(palette_size):
    pix = img.getpixel( (i % img.width, i // img.width) )

    r = pix[0] >> 3
    g = pix[1] >> 3
    b = pix[2] >> 3

    col = (b << 10) | (g << 5) | r

    color_code = '0x' + hex(col)[2:].zfill(4)
    colors.append(color_code)

# Write output
f = args.output

if args.static:
    f.write('static ')
f.write('const u16 ' + args.array_name + '[')

# Write the array size as (A * 16 + B)
if palette_size // 16 != 0:
    f.write(str(palette_size // 16) + ' * 16')
    if palette_size % 16 != 0:
        f.write(' + ')
if palette_size % 16 != 0:
    f.write(str(palette_size % 16))

f.write('] = {')
for i in range(palette_size):
    # Add a whiteline every 16 colors
    if i % 16 == 0 and i != 0:
        f.write('\n')

    # Have a maximum of 8 colors per line
    if i % 8 == 0:
        f.write('\n    ')

    f.write(colors[i])

    # Add the comma after a color, if necessary
    if i != palette_size - 1:
        f.write(',')
        if i % 8 != 7:
            f.write(' ')
f.write('\n};\n')
