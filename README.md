# Summary
**cam_overlay** is a simple program that reads a webcam and displays it on the screen along with an overlay image.

It's primary purpose is to offload as much of the image processing/decoding to the Raspberry Pi's GPU as possible and disply a reverse camera overlay.

# Dependencies
- v4l2-dev
- libpng-dev

TODO: Instructions to install dependencies

# Compiling
Simply run `make` from the command line

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

TODO: Options to control overlay position/perspective

