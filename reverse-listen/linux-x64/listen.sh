#!/bin/bash

# ngrok tcp 8888

socat file:`tty`,raw,echo=0 tcp-listen:8888
