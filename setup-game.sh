#!/bin/sh

# Copy the necessary files from the template to a new directory and
# initialize a git repository.

TEMPLATE_DIRECTORY=$(dirname "$0")

if [ -z "$1" ]; then
    echo "Usage: $0 <game-name>"
    exit
fi

if test -e "$1"
then
    echo "Could not create directory $1: file already exists"
    exit
fi

echo "Creating the directory"
mkdir -p "$1"

echo "\nCopying template files:"

for item in src include res tools\
            Makefile COPYING .gitignore
do
    echo "- ${item}"
    cp -r "${TEMPLATE_DIRECTORY}/${item}" "$1"
done

echo "\nInitializing git repository"
git -C "$1" init

echo "\nRetrieving base library"
git -C "$1" submodule add\
    https://github.com/Vulcalien/gba-base-library.git lib/base

echo "\nInitializing all submodules"
git -C "$1" submodule update --init --recursive

echo "\nDone!"
