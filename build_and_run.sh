#!/bin/bash

set -ex

make
./image-viewer image.png
