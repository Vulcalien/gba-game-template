#!/usr/bin/env python3

# Copyright 2023-2024 Vulcalien
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

import sys, argparse
from sys import exit
from PIL import Image

from utils import load_palette, DataWriter

# Setup argparse
parser = argparse.ArgumentParser(
    description='Generate a GBA tileset from a tileset image'
)

parser.add_argument('-i', '--input',
                    type=argparse.FileType('rb'), required=True,
                    help='specify the filename of the image file')
parser.add_argument('-n', '--name',
                    required=True,
                    help='specify the name of the output array')
parser.add_argument('--bpp',
                    type=int, choices=[4, 8], required=True,
                    help='specify the bits per pixel')
parser.add_argument('--tile-width',
                    type=int, required=True,
                    help='specify the width of a tile')
parser.add_argument('--tile-height',
                    type=int, required=True,
                    help='specify the height of a tile')

parser.add_argument('--palette',
                    type=argparse.FileType('rb'), required=True,
                    help='specify the filename of the palette file')
parser.add_argument('--transparent',
                    type=str, default=None,
                    help='specify a color to be considered transparent')

parser.add_argument('-o', '--output',
                    type=argparse.FileType('w'), default=sys.stdout,
                    help='specify the output file (default: stdout)')
parser.add_argument('-s', '--static',
                    action=argparse.BooleanOptionalAction,
                    help='add the \'static\' modifier to the output ' +
                         'array')

args = parser.parse_args()

# Open image
img = Image.open(args.input).convert('RGB')

tileset_w    = (img.width  // 8) // args.tile_width
tileset_h    = (img.height // 8) // args.tile_height
tileset_size = tileset_w * tileset_h

big_tile_size      = args.tile_width * args.tile_height
bytes_per_8x8_tile = (32 if args.bpp == 4 else 64)

# Read the palette file
color_map = load_palette(args.palette, args.bpp)

if args.transparent is not None:
    hex_code = args.transparent.lstrip('#')
    val = (
        int(hex_code[0:2], 16),
        int(hex_code[2:4], 16),
        int(hex_code[4:6], 16)
    )
    color_map[val] = 0

# Scan the tileset and write output
writer = DataWriter(args.output, 'u8')

def assert_color_in_palette(pix):
    if pix not in color_map:
        col = pix[0] << 16 | pix[1] << 8 | pix[2]
        col = hex(col)[2:].zfill(6)
        exit('Error: color not present in the palette: #' + col)

def scan_8x8_tile_8bpp(x0, y0):
    for y in range(8):
        for x in range(8):
            pix = img.getpixel( (x0 + x, y0 + y) )
            assert_color_in_palette(pix)
            writer.write(color_map[pix])

def scan_8x8_tile_4bpp(x0, y0):
    for y in range(8):
        for x in range(0, 8, 2):
            pix0 = img.getpixel( (x0 + x,     y0 + y) )
            pix1 = img.getpixel( (x0 + x + 1, y0 + y) )

            assert_color_in_palette(pix0)
            assert_color_in_palette(pix1)

            writer.write(color_map[pix1] << 4 | color_map[pix0])

def scan_8x8_tile(x0, y0):
    if args.bpp == 8:
        scan_8x8_tile_8bpp(x0, y0)
    elif args.bpp == 4:
        scan_8x8_tile_4bpp(x0, y0)

def scan_big_tile(x0, y0):
    for ysubtile in range(args.tile_height):
        for xsubtile in range(args.tile_width):
            scan_8x8_tile(
                x0 + xsubtile * 8,
                y0 + ysubtile * 8
            )

writer.begin(
    args.name, args.static,
    tileset_size * big_tile_size * bytes_per_8x8_tile
)
for yt in range(tileset_h):
    for xt in range(tileset_w):
        scan_big_tile(
            xt * args.tile_width  * 8,
            yt * args.tile_height * 8
        )
writer.end()
