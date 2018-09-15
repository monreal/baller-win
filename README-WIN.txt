
    =================================
    =   Ballerburg SDL for Windows  =
    =================================


 Compiling for Windows
 ---------------------

This version of Ballerburg SDL has beed modified to be compiled for Microsoft
Windows using MinGW. The CMake setup is not perfect but it should assist you
in producing a working build.

In order to cross-compile the code on Fedora 28, first install the following
dependencies:

$ sudo dnf install mingw32-SDL2 \
                   mingw32-gettext

The following additianl packages are needed to build a static binary:

$ sudo dnf install mingw32-SDL2-static \
                   mingw32-gettext-static \
                   mingw32-win-iconv-static

You will need to set CMAKE_SYSTEM_NAME, CMAKE_C_COMPILER and probably
also CMAKE_FIND_ROOT_PATH when generating the build files:

$ mkdir build && cd build
$ cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_SDL2=true \
  -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=i686-w64-mingw32-gcc \
  -DCMAKE_FIND_ROOT_PATH=/usr/i686-w64-mingw32/sys-root/mingw \
  ..

