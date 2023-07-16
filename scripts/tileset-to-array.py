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

# Convert a tileset image into a GBA tileset array.
# The output is a C array of type 'const u16' and the exact amount of
# bytes required as length.
#
# Run 'tileset-to-array.py -h' for help.

# ==================================================================== #

# --- THE FORMAT --- #
#
# A tileset file is an image containing a 'metadata' section and a
# 'data' section. The image is composed of 8x8 tiles. These 8x8 tiles
# are arranged in bigger tiles with a certain width and height.
#
# -- METADATA --
#
# - Tile Size -
# The top-left 8x8 tile is used to specify how many 8x8 tiles make up a
# bigger tile. A rectangle of white (#ffffff) pixels in its top-left
# corner is used to specify this. Each white pixel represents one 8x8
# tile.
#
# - Palettes -
# The equivalent of a row of bigger tiles is reserved for palette tiles.
# Note: to ensure that there always is at least one palette tile, if the
# image is only one 8x8 tile in width and the size of a bigger tile is
# the same as one 8x8 tile, then the second row will be reserved for
# a palette tile.
#
# A palette tile is a 8x8 tile and contains 4 palettes each. A palette
# is used to map the RGB colors to palette indexes. Each palette is made
# up of 16 pixels (2 rows of 8 pixels). The colors are indexed
# left-to-right, then top-to-bottom. The palettes are processed
# top-to-bottom, while the palette 8x8 tiles are processed
# left-to-right, then top-to-bottom.
#
# If a color has already been indexed, it will be ignored.
#
# -- DATA --
#
# To generate the output, each bigger tile is processed individually.
# The scanning order for bigger tiles is left-to-right, then
# top-to-bottom. Each 8x8 tile that makes up the bigger tile is then
# processed with regard to the same scanning order.
#
# The output can be directly read by GBA. The 8x8 tiles forming a bigger
# tile are contiguous in the output.

# ==================================================================== #

import sys, argparse
from sys import exit
from PIL import Image

# Setup argparse
parser = argparse.ArgumentParser(
    description='Generate a GBA tileset from a tileset image'
)

parser.add_argument('tileset_filename',
                    type=argparse.FileType('rb'),
                    help='specify the filename of the tileset image')
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
img = Image.open(args.tileset_filename).convert('RGB')

tileset_8x8_w = img.width  // 8
tileset_8x8_h = img.height // 8

if tileset_8x8_w * tileset_8x8_h < 2:
    exit('Error: the tileset image must contain at least 2 8x8 tiles')

### READ TILESET METADATA ###

# Determine tile size by reading the top-left 8x8 tile
tile_width  = 0
tile_height = 0
for i in range(8):
    x_pix = img.getpixel( (i, 0) )
    y_pix = img.getpixel( (0, i) )

    if x_pix == (0xff, 0xff, 0xff):
        tile_width = 1 + i
    if y_pix == (0xff, 0xff, 0xff):
        tile_height = 1 + i

if tile_width == 0 or tile_width == 0:
    exit('Error: the specified tile size is "0 by 0"\n' +
         'Make sure that the top-left tile contains a rectangle made ' +
         'of white (#ffffff) pixels in its top-left corner.')

# Crop the image, if necessary
if img.width % (8 * tile_width):
    print('Warning: the tileset image contains columns of pixels ' +
          'that will be cropped',
          file=sys.stderr)
    tileset_8x8_w = (img.width // (8 * tile_width)) * tile_width

if img.height % (8 * tile_height):
    print('Warning: the tileset image contains rows of pixels ' +
          'that will be cropped',
          file=sys.stderr)
    tileset_8x8_h = (img.height // (8 * tile_height)) * tile_height

# Calculate how many tile rows are dedicated to the metadata. This is
# usually the same value of 'tile_height'. However, if both 'tile_width'
# and 'tile_height' are 1 and 'tileset_8x8_w' is 1, then 'metadata_rows'
# will be 2, to ensure that there is at least one palette tile.
# Otherwise, the "tile size" tile would be the only metadata tile.
if tile_width == 1 and tile_height == 1 and tileset_8x8_w == 1:
    metadata_rows = 2
else:
    metadata_rows = tile_height

tileset_w = tileset_8x8_w // tile_width
tileset_h = (tileset_8x8_h - metadata_rows) // tile_height

# Map the colors by reading the palettes
color_map = {}
for yt in range(metadata_rows):
    for xt in range(tileset_8x8_w):

        # skip the top-left tile (the one used to decide the tile size)
        if (xt, yt) == (0, 0):
            continue

        for pal in range(4):
            for i in range(16):
                pix = img.getpixel(
                    (xt * 8 + i % 8, yt * 8 + pal * 2 + i // 8)
                )

                if pix not in color_map:
                    color_map[pix] = i

### READ TILESET DATA ###

# Scan the tileset and write output
f = args.output

if args.static:
    f.write('static ')

f.write('const u8 ' + args.array_name + '[' +
        str(tileset_w * tileset_h) + ' * ' +
        str(tile_width * tile_height) + ' * 32] = {\n')

for yt in range(tileset_h):
    for xt in range(tileset_w):
        for ysubtile in range(tile_height):
            for xsubtile in range(tile_width):
                for ypix in range(8):
                    for xpix in (1, 0, 3, 2, 5, 4, 7, 6):
                        pix = img.getpixel((
                            (xt * tile_width                  + xsubtile) * 8 + xpix,
                            (yt * tile_height + metadata_rows + ysubtile) * 8 + ypix
                        ))

                        if pix not in color_map:
                            col = pix[0] << 16 | pix[1] << 8 | pix[2]
                            col = hex(col)[2:].zfill(6)
                            exit('Error: color not present in any ' +
                                 'palette (#' + col + ')')

                        if xpix & 1 == 1:
                            f.write('0x')

                        f.write(hex(color_map[pix])[2:])

                        if xpix & 1 == 0:
                            f.write(', ')
                    f.write('\n')
                f.write('\n')
f.write('};\n')
