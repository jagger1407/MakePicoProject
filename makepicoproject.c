#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    if(argc <= 1)
    {
        puts("SYNTAX: makepicoproject [Project Name]");
        puts("\nDescription: Creates a new project folder in current directory from template.");
        return 0;
    }
    else if(argc > 2)
    {
        puts("ERROR: Too many arguments.");
        return 0;
    }
    const size_t nameSize = strlen(argv[1]);
    char* projectName = (char*)calloc(nameSize+1, 1);
    strcpy(projectName, argv[1]);
    char* projectPath = (char*)calloc(nameSize+3, 1);
    strcat(projectPath,"./");
    strcat(projectPath, projectName);
    const size_t dirSize = strlen(projectPath);
    // make dir
    char* mkStr = (char*)calloc(7 + nameSize, 1);
    strcat(mkStr, "mkdir ");
    strcat(mkStr, projectName);
    system(mkStr);
    // copy pico import file
    char* cpStr = (char*)calloc(55 + dirSize, 1);
    strcat(cpStr,"cp /usr/share/pico-sdk/external/pico_sdk_import.cmake ");
    strcat(cpStr, projectPath);
    system(cpStr);
    // create main.c
    char* mainPath = (char*)calloc(dirSize+8, 1);
    strcat(mainPath, projectPath);
    strcat(mainPath, "/main.c");
    FILE *currentFile = fopen(mainPath, "w");
    fputs("int main() {\n    pico_init();\n\n    while(1) pico_loop();\n}", currentFile);
    fclose(currentFile);
    // create CMakeLists.txt
    char* cmlPath = (char*)calloc(dirSize + 16, 1);
    strcat(cmlPath, projectPath);
    strcat(cmlPath, "/CMakeLists.txt");
    currentFile = fopen(cmlPath, "w");
    fprintf(currentFile, "cmake_minimum_required(VERSION 3.13)\n\n# initialize the SDK based on PICO_SDK_PATH\n# note: this must happen before project()\ninclude(pico_sdk_import.cmake)\n\nproject(%s C CXX ASM)\n\n# initialize the Raspberry Pi Pico SDK\npico_sdk_init()\n\nadd_executable(%s\nmain.c\n%s.c\n)\n\n# pull in common dependencies\ntarget_link_libraries(%s pico_stdlib)\n\n# create map/bin/hex file etc.\npico_add_extra_outputs(%s)", projectName, projectName, projectName, projectName, projectName);
    fclose(currentFile);
    // create project c file
    char* projectSrc = (char*)calloc(dirSize + nameSize + 4, 1);
    strcat(projectSrc, projectPath);
    strcat(projectSrc, "/");
    strcat(projectSrc, projectName);
    strcat(projectSrc, ".c");
    currentFile = fopen(projectSrc, "w");
    fputs("#include \"pico/stdlib.h\"\n\nvoid pico_init()\n{\n    \n}\n\nvoid pico_loop()\n{\n    \n}", currentFile);
    fclose(currentFile);
    // create build script
    char* buildScript = (char*)calloc(dirSize + 10, 1);
    strcat(buildScript, projectPath);
    strcat(buildScript, "/build.sh");
    currentFile = fopen(buildScript, "w");
    fputs("cmake . -B ./build\ncd build\nmake", currentFile);

    return 0;
}
