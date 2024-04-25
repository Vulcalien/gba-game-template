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

from datawriter import DataWriter

# Setup argparse
parser = argparse.ArgumentParser(
    description='Convert a file into an array'
)

parser.add_argument('-i', '--input',
                    type=argparse.FileType('rb'), required=True,
                    help='specify the filename of the input file')
parser.add_argument('-n', '--name',
                    required=True,
                    help='specify the name of the output array')

parser.add_argument('-o', '--output',
                    type=argparse.FileType('w'), default=sys.stdout,
                    help='specify the output file (default: stdout)')
parser.add_argument('-s', '--static',
                    action=argparse.BooleanOptionalAction,
                    help='add the \'static\' modifier to the output ' +
                         'array')

args = parser.parse_args()

# Read file contents
contents = args.input.read()

# Write output
writer = DataWriter(args.output, 'u8')

writer.begin(args.name, args.static, len(contents))
for i in range(len(contents)):
    writer.write(contents[i])
writer.end()
