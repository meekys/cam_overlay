# Summary
**cam_overlay** is a simple program that reads a webcam and displays it on the screen along with an overlay image.

It's primary purpose is to offload as much of the image processing/decoding to the Raspberry Pi's GPU as possible and disply a reverse camera overlay.

# Dependencies
## Common
- v4l2-dev
- libpng-dev

## X11 (Debian, Ubuntu, etc...)
 - mesa-common-dev
 - libgles2-mesa-dev

TODO: Instructions to install dependencies

# Compiling
## Raspberry Pi
Simply run `make` from the command line

## X11 (Debian, Ubuntu, etc...)
Run `make DISPLAY=x11` from the command line

# Running
From the command line, simply run with the appropriate parameters

`./cam_overlay.bin`

# Command line parameters

Command line parameters allow you to tweak the behavior.

| Command | Long Command | Details |
|---------|--------------|---------
| -d       | --device    | v4l2 device name. Default: /dev/video0
| -m       | --mmap      | Use memory mapped buffers [default]
| -r       | --read      | Use read() calls
| -u       | --userp     | Use application allocated buffers
| -s       | --stretch   | Stretch image to screen
| -R       | --rotate    | Rotate image 180 degrees
| -h       | --fliph     | Flip image horizontally
| -v       | --flipv     | Flip image vertically
