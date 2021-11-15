# Turtle

A Python to C++ transcompiler

## Build
### Prerequisites
If you would like to test it out you would need:

 - `cmake`
 - `clang`
 - `libboost-all-dev`
 - `git`
 - `build-essential`
```Bash
sudo apt install -y cmake clang libboost-all-dev build-essential
```

If you would like to contribute you would need

 - `pandoc`
```Bash
sudo apt install -y cmake clang libboost-all-dev build-essential pandoc
```
### Build instructions
```Bash
git clone https://github.com/angeletakis/turtle.git
cd turtle/build
cmake ..
make
```
