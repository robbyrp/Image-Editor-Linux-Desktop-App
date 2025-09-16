#!/bin/bash
# Copyright 2024 Darius Neatu (neatudarius@gmail.com)

sudo apt-get update

sudo apt-get install                        \
    gcc                                     \
    python3 python3-pip                     \
    perl                                    \
    make gdb valgrind                       \
    imagemagick                             \
    rustc cargo

sudo pip3 install                           \
    argparse                                \
    pathlib                                 \
    recordclass

cargo build --release
cp target/release/imagecheck image_check
