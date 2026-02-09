#pragma once

const char* src_header =
"#pragma once\n" \
"\n" \
"void pico_init();\n" \
"\n" \
"void pico_loop();\n";

const char* src_impl =
"#include \"pico/stdlib.h\"\n" \
"\n" \
"void pico_init() {\n" \
"    \n" \
"}\n" \
"\n" \
"void pico_loop() {\n" \
"    \n" \
"}\n" \
"\n";

const char* src_main =
"int main() {\n" \
"    pico_init();\n" \
"\n" \
"    while(1) pico_loop();\n" \
"}\n";

const char* src_cml =
"cmake_minimum_required(VERSION 3.13)\n" \
"\n" \
"# initialize the SDK based on PICO_SDK_PATH\n" \
"# note: this must happen before project()\n" \
"include(pico_sdk_import.cmake)\n" \
"\n" \
"project(%s C CXX ASM)\n" \
"\n" \
"# initialize the Raspberry Pi Pico SDK\n" \
"pico_sdk_init()\n" \
"\n" \
"add_executable(%s\n" \
"main.c\n" \
"%s.c\n" \
")\n" \
"\n" \
"# pull in common dependencies\n" \
"target_link_libraries(%s pico_stdlib)\n" \
"\n" \
"# create map/bin/hex file etc.\n" \
"pico_add_extra_outputs(%s)";
