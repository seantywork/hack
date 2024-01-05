#!/bin/bash



# cd srs/trunk && ffmpeg -re -i ./doc/source.flv -c copy -f flv -y rtmp://localhost/live/livestream


ffmpeg -f v4l2 -i /dev/video0                                   \
  -f alsa -i hw:0,0                                               \
  -c:v libx264 -pix_fmt yuv420p -framerate 15 -g 30 -b:v 500k     \
  -c:a aac -b:a 128k -ar 44100 -ac 2                              \
  -f flv rtmp://localhost/live/livestream
#  -preset ultrafast -tune zerolatency                             \
