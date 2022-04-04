# MakePicoProject
This program aims to make writing C/C++ code for the Raspberry Pi Pico easier, by automating the project creation.
Once executed, the project will be ready to compile, your code can be written inside the project C file.

`pico_init()`: This function is called once on startup.

`pico_loop()`: This function runs continuously.

## Compiling
This program can be compiled with any C/C++ compiler.

The resulting project will be compilable with CMake. Most IDEs can build the project out-of-the-box. 
If not, a build.sh script can be added to the project folder by adding "-b" to the end of the command.
