# Summary
**cam_overlay** is a simple program that reads a webcam and displays it on the screen along with an overlay image.

It's primary purpose is to offload as much of the image processing/decoding to the Raspberry Pi's GPU as possible and disply a reverse camera overlay.

# Dependencies
## Common
- v4l2-dev
- libpng-dev
- X11-dev [optional]

## Raspberry Pi
### ilclient
```
cd /opt/vc/src/hello_pi/libs/ilclient
make
```

## X11 (Debian, Ubuntu, etc...)
 - mesa-common-dev
 - libgles2-mesa-dev

TODO: Instructions to install dependencies

# Compiling
## Raspberry Pi
Simply run `cmake . && make`

# Running
From the command line, simply run with the appropriate parameters

`./bin/camoverlay.bin`

# Stopping
If running from the command line, Ctrl+C will stop.

If stopping from another console, using `killall camoverlay.bin` will stop.

If running from another application, sending a KILLTERM signal will stop.

# Command line parameters

Command line parameters allow you to tweak the behavior.

| Command | Long Command | Details |
|---------|--------------|---------
| -d       | --device    | v4l2 device name. Default: /dev/video0
| -i       | --input     | Input module: V4L2MMap[default], V4L2UserPtr, V4L2Read
| -o       | --output    | Output module: Shader
| -D       | --display   | Display module: Bmc, X11
| -u       | --userp     | Use application allocated buffers
| -s       | --stretch   | Stretch image to screen
| -R       | --rotate    | Rotate image 180 degrees
| -h       | --fliph     | Flip image horizontally
| -v       | --flipv     | Flip image vertically
