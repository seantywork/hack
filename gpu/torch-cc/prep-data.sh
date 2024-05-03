#!/bin/bash


git clone https://github.com/golbin/TensorFlow-MNIST.git

mv ./TensorFlow-MNIST/mnist/data ./data

rm -rf ./TensorFlow-MNIST

for FILE in ./data/*
do

    gzip -d "$FILE"

done

