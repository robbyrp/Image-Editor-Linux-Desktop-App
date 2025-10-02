# Image-Editor-Linux-Desktop-App
--------
#### An image editor supporting PPM and PGM formats, with functions for different effects, written in C++ using GLFW3 and Dear ImGui for the frontend, and C for the backend
--------
## Features
* ##### Crop
* ##### Rotation
* ##### Sharpen, Edge detect, Box-blur, Gaussian-blur, Equalize effects
* ##### Saving and Loading images from filesystem
--------
## Pre-requisites
* Before building, make sure to install
```sh
zenity, libglfw3-dev
```
* On Debian-based systems, run:
```sh
sudo apt install zenity
sudo apt-get install libglfw3-dev
```
* On Fedora, run:
```sh
sudo dnf install glfw-devel
```
## Building

The project can be built using the ```make``` tool:
```sh
make
```
And can then be run:
```sh
./editor
```
- **To then clean the project, run ```make clean```.**

## External Dependencies
* imgui - https://github.com/ocornut/imgui
* glfw3 - https://github.com/glfw/glfw
