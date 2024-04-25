#!/bin/bash

make clean

make

./asym.out keygen

./asym.out encrypt

./asym.out decrypt

#valgrind --leak-check=full \
#         --show-leak-kinds=all \
#         --track-origins=yes \
#         --verbose \
#         ./asym.out pipe