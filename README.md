# GBA Game Template

A template for making Game Boy Advance games containing code for
handling basic functionalities of the console and a simple level
structure.

## Getting Started

### Requirements
Make sure to have the `arm-none-eabi-gcc` compiler. This can be
installed through the system's package manager or downloaded from the
Internet.

You will also need a Game Boy Advance emulator to run the game. I would
recommend using
[mGBA](https://mgba.io/)
for the development and both mGBA and
[NanoBoyAdvance](https://github.com/nba-emu/NanoBoyAdvance)
for testing. Make sure that the `EMULATOR` variable in `Makefile` is set
to the emulator's command.

### Creating a new project
On Unix-like systems, a new project can be created using the
`setup-game.sh` script, specifying the directory of the new project. The
script will also initialize a new git repository in that directory.
```
./setup-game.sh <project-dir>
```

Alternatively, manually copy these files and directories to the new
project's root directory:
```
src include lib scripts Makefile COPYING .gitignore
```

## Features

### Compilation files
Two files are required to compile a GBA program using the GCC compiler:
`lnkscript` and `crt0.s`. Both files present in this template are
modified versions of the ones published by Jeff Frohwein many years ago.

`lnkscript` supplies the linker with information regarding the memory
layout of the GBA, specifying where code, data and all the various
sections should be placed. `crt0.s` contains the initialization code,
which handles copying data and code into memory and calling the
`AgbMain` function.

### ROM header
`header.s` contains the ROM header, a block of data used to specify
details about the ROM. Make sure to set these values in the ROM header:
  - Game Title
  - Game Code
  - Maker Code (optional)
  - Software Version (optional)
  - Header Checksum
  - Cart Backup ID (if the game uses save memory)

The 'header checksum' (also called complement check) is used by the BIOS
to determine if a ROM is valid. The value is calculated based on the ROM
header, so it changes when the ROM header is modified. You can use the
`header-checksum.py` to calculate it. To do so, compile the game once
and then run the script on the binary.

### Keypad input
By including `input.h`, three macros become available:
  - INPUT_DOWN(key)
  - INPUT_PRESSED(key)
  - INPUT_RELEASED(key)

'key' can be any of the macros defined in the `input.h` header file,
such as `KEY_UP`, `KEY_START`...

The down, pressed and released states are updated once every tick (i.e.
about 60 times a second).

### Video display
Display settings can be set by editing the `screen.c` file. To aid the
developer, macros for registers relevant to the use of the display
(palettes, tilesets, BG control...) are defined.

In `screen.h`, the `screen_Sprite` structure and `screen_set_sprite`
function can be used to display a sprite instead of manually setting the
bits in the OAM memory. This allows for more readable code, without
losing performance.

### Direct sound
The template includes a driver for controlling the two direct sound
channels. These channels use 'signed 8-bit PCM' data. The sample rate is
16384 by default, but can be set in `sound.h`. Because of a choice in
the implementation, both channels have the same sample rate.

After including `sound.h`, two functions are available:
  - sound_play(sound, length, channel, loop)
  - sound_stop(channel)

'channel' can be either `sound_channel_A` or `sound_channel_B`.

If 'sound', the array of samples, has a known size, the `SOUND_PLAY`
macro can be used to avoid passing the 'length' argument to
`sound_play`.

### Scenes
A scene is an object composed of a 'tick', 'draw' and (optionally)
'init' functions. The various parts of the game (menus, transitions,
cutscenes, the level screen...) can be split into different scenes.

Include `scene.h` to use the scene system. The `scene_set` function sets
the current scene, the only one being updated and drawn every tick.

### Helper scripts
A set of helper Python scripts contained in the 'scripts' directory can
be used to simplify certain operations.

`color-convert.py` converts 24-bit color codes into 15-bit color codes,
as used by the GBA. Both input and output are in hexadecimal numerals.

`header-checksum.py` calculates the header checksum of the given '.gba'
binary file. The resulting number can then be written in the ROM header.

`convert-resources.py` converts all resources listed in a JSON file into
a GBA-readable format. It supports images, tilesets, images and binary
files. See the example included in the script for information on how to
write the JSON file.

## License
This template is released under the GNU General Public License, either
version 3 of the License or any later version. Any work that uses this
template or parts of it must be released under a
[GPL-Compatible Free Software
license](https://www.gnu.org/licenses/license-list.html).
