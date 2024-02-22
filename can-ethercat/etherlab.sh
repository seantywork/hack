#!/bin/bash

git clone https://gitlab.com/etherlab.org/ethercat.git


cd ethercat 

git switch -c stable-1.6 origin/stable-1.6

cd ../