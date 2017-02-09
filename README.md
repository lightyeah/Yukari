# Yukari

[![Build Status](https://travis-ci.com/DanNixon/Yukari.svg?token=hEeXj1er91qf6vBmhf9x&branch=master)](https://travis-ci.com/DanNixon/Yukari)

3D environment mapper

## Building on Ubuntu 16.04

1. Install `libproj-dev libboost-all-dev libpcl1.7 libpcl-dev`
2. `cmake ../Yukari -DCMAKE_BUILD_TYPE=Debug -DBOOST_LIBRARYDIR=/usr/lib/x86_64-linux-gnu/`
3. `make`

## Building on Windows

TODO

1. Build PCL and dependencies
  - https://github.com/boostorg/boost
  - https://github.com/mariusmuja/flann
  - https://github.com/Kitware/VTK
  - Qt
  - https://github.com/qhull/qhull
  - https://github.com/OpenNI/OpenNI
  - https://github.com/avin2/SensorKinect
  - https://github.com/PointCloudLibrary/pcl
2. `cmake ../Yukari -G "Visual Studio 14 2015 Win64" -DCMAKE_BUILD_TYPE=Debug`
