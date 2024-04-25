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

# Resource List File example:
""" resources.json
{
    "tilesets": [
        {
            "name": "my_tileset",

            "input": "my/input/tileset.png",
            "output": "my/output/tileset.c",

            "tile_width": 2,
            "tile_height": 4,

            "palette": "my/input/palette.png",
            "transparent": "#ff00ff",
            "bpp": 4,

            "static": true
        },

        ...
    ],

    "palettes": [
        {
            "name": "my_palette",

            "input": "my/input/palette.png",
            "output": "my/output/palette.c",

            "static": false
        },

        ...
    ],

    "images": [
        {
            "name": "image_8bpp",

            "input": "my/input/8bpp_image.png",
            "output": "my/output/8bpp_image.c",

            "palette": "my/input/palette.png",
            "bpp": 8,

            "static": false
        },

        {
            "name": "image_16bpp",

            "input": "my/input/16bpp_image.png",
            "output": "my/output/16bpp_image.c",

            "bpp": 16,

            "static": false
        },

        ...
    ],

    "files": [
        {
            "name": "my_file",

            "input": "my/input/file.bin",
            "output": "my/output/file.c",

            "static": true
        },

        ...
    ]
}
"""
# Optional values:
#   - 'static', if absent, defaults to False
#   - in 'images', 'palette' is ignored and not required if 'bpp' is 16

import sys, os, argparse, json

try:
    import tomllib
    toml_support = True
except ImportError:
    toml_support = False

# Setup argparse
parser = argparse.ArgumentParser(
    description='Convert all resources listed in JSON or TOML files'
)

parser.add_argument('res_list_files', nargs='+',
                    type=argparse.FileType('rb'),
                    help='specify the resource list files')

args = parser.parse_args()

# Functions

def tileset_args(element):
    args = ''

    args += ' --tile-width "%d"'  % int(element['tile_width'])
    args += ' --tile-height "%d"' % int(element['tile_height'])

    args += ' --palette "%s"' % str(element['palette'])
    args += ' --bpp "%d"'     % int(element['bpp'])

    if 'transparent' in element:
        args += ' --transparent "%s"' % str(element['transparent'])

    return args

def palette_args(element):
    return '--bpp 16'

def image_args(element):
    args = ''

    bpp = int(element['bpp'])
    args += ' --bpp %d' % bpp

    if bpp in (4, 8):
        palette = str(element['palette'])
        args += ' --palette %s' % palette

    return args

FILE_TYPES = {
    'tilesets': {
        'script': '%s/res/tileset-to-array.py',
        'args_function': tileset_args
    },

    'palettes': {
        'script': '%s/res/image-to-array.py',
        'args_function': palette_args
    },

    'images': {
        'script': '%s/res/image-to-array.py',
        'args_function': image_args
    },

    'files': {
        'script': '%s/res/file-to-array.py',
        'args_function': None
    }
}

PARENT_PATH = os.path.relpath(sys.path[0])

def convert(element, file_type):
    input_file  = str(element['input'])
    output_file = str(element['output'])
    name        = str(element['name'])

    static = False
    if 'static' in element and bool(element['static']):
        static = True

    cmd = ' '.join((
        file_type['script'] % PARENT_PATH,
        '-i', input_file,
        '-o', output_file,
        '-n', name,
        '-s' if static else ''
    ))

    # Add script-specific arguments
    args_function = file_type['args_function']
    if args_function is not None:
        cmd += ' ' + args_function(element)

    print(cmd)
    os.system(cmd)

def parse_json(f):
    try:
        return json.load(f)
    except json.JSONDecodeError as e:
        print('Error: invalid JSON:', e)

def parse_toml(f):
    if toml_support:
        try:
            return tomllib.load(f)
        except tomllib.TOMLDecodeError as e:
            print('Error: invalid TOML:', e)
    else:
        print('Error: TOML is not supported by this version of Python')

def parse_file(f):
    parsers = {
        '.json': parse_json,
        '.toml': parse_toml
    }

    content = None
    ext = os.path.splitext(f.name)[1]

    try:
        content = parsers[ext](f)
    except KeyError:
        print('Error: unknown extension:', ext)

    return content

# Read resource list files
for f in args.res_list_files:
    print('Reading file:', f.name)

    content = parse_file(f)
    if content is None:
        continue

    for file_type in FILE_TYPES:
        if file_type in content:
            for element in content[file_type]:
                convert(element, FILE_TYPES[file_type])
