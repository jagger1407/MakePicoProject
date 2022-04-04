#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int DoesFileExist(const char * filename)
{
    FILE *inputFile;
    inputFile = fopen(filename, "r");
    if (inputFile != NULL)
    {
        fclose(inputFile);
        return 1;
    }
    return 0;
}

int main(int argc, char** argv)
{
    if(argc <= 1)
    {
        puts("SYNTAX: makepicoproject <Project Name> [options]");
        puts("\nDescription: Creates a new project folder in current directory from template.");
        puts("\nOptions:"
             "\n  -sdk  Sets custom Pico SDK path (Default: /usr/share/pico-sdk)"
             "\n  -b    Tells program to create build.sh script"
             "\n  -v    Prints out what is currently being done");
        return 0;
    }
    
    // Creating strings
    const size_t nameSize = strlen(argv[1]);
    char* projectName = (char*)calloc(nameSize+1, 1);
    strcpy(projectName, argv[1]);
    char* projectPath = (char*)calloc(nameSize+3, 1);
    strcat(projectPath,"./");
    strcat(projectPath, projectName);
    const size_t dirSize = strlen(projectPath);
    char* picoImportpath = (char*)calloc(51, 1);
    strcat(picoImportpath, "/usr/share/pico-sdk/");
    
    // Check Arguments
    unsigned char argFlags = 0x00;
    if(argc > 2)
    {
        for(int i=2;i<argc;i++)
        {
            if(strcmp(argv[i], "-sdk") == 0)
            {
                free(picoImportpath);
                picoImportpath = (char*)calloc(strlen(argv[i+1])+32, 1);
                strcpy(picoImportpath, argv[i+1]);
            }
            else if(strcmp(argv[i], "-b") == 0) argFlags += 0x02;
            else if(strcmp(argv[i], "-v") == 0) argFlags += 0x01;
        }
    }
    if(picoImportpath[strlen(picoImportpath)-1] != '/') strcat(picoImportpath, "/");
    strcat(picoImportpath, "external/pico_sdk_import.cmake");
    
    // make dir
    if(argFlags & 0x01) printf("Making project folder... ");
    char* mkStr = (char*)calloc(7 + nameSize, 1);
    strcat(mkStr, "mkdir ");
    strcat(mkStr, projectName);
    system(mkStr);
    free(mkStr);
    if(argFlags & 0x01) puts("DONE");
    
    // copy pico import file
    if(argFlags & 0x01) printf("Importing pico_sdk_import.cmake... ");
    char* cpStr = (char*)calloc(55 + dirSize, 1);
    if(!DoesFileExist(picoImportpath))
    {
        printf("ERROR!\n%s not found.\n", picoImportpath);
        return 1;
    }
    strcat(cpStr,"cp /usr/share/pico-sdk/external/pico_sdk_import.cmake ");
    strcat(cpStr, projectPath);
    system(cpStr);
    free(cpStr);
    if(argFlags & 0x01) puts("DONE");
    
    // create main.c
    if(argFlags & 0x01) printf("creating 'main.c'... ");
    char* mainPath = (char*)calloc(dirSize+8, 1);
    strcat(mainPath, projectPath);
    strcat(mainPath, "/main.c");
    FILE *currentFile = fopen(mainPath, "w");
    fputs("int main() {\n    pico_init();\n\n    while(1) pico_loop();\n}", currentFile);
    fclose(currentFile);
    free(mainPath);
    if(argFlags & 0x01) puts("DONE");
    
    // create CMakeLists.txt
    if(argFlags & 0x01) printf("creating 'CMakeLists.txt'... ");
    char* cmlPath = (char*)calloc(dirSize + 16, 1);
    strcat(cmlPath, projectPath);
    strcat(cmlPath, "/CMakeLists.txt");
    currentFile = fopen(cmlPath, "w");
    fprintf(currentFile, "cmake_minimum_required(VERSION 3.13)\n\n# initialize the SDK based on PICO_SDK_PATH\n# note: this must happen before project()\ninclude(pico_sdk_import.cmake)\n\nproject(%s C CXX ASM)\n\n# initialize the Raspberry Pi Pico SDK\npico_sdk_init()\n\nadd_executable(%s\nmain.c\n%s.c\n)\n\n# pull in common dependencies\ntarget_link_libraries(%s pico_stdlib)\n\n# create map/bin/hex file etc.\npico_add_extra_outputs(%s)", projectName, projectName, projectName, projectName, projectName);
    fclose(currentFile);
    free(cmlPath);
    if(argFlags & 0x01) puts("DONE");
    
    // create project c file
    if(argFlags & 0x01) printf("creating project C file... ");
    char* projectSrc = (char*)calloc(dirSize + nameSize + 4, 1);
    strcat(projectSrc, projectPath);
    strcat(projectSrc, "/");
    strcat(projectSrc, projectName);
    strcat(projectSrc, ".c");
    currentFile = fopen(projectSrc, "w");
    fputs("#include \"pico/stdlib.h\"\n\nvoid pico_init()\n{\n    \n}\n\nvoid pico_loop()\n{\n    \n}", currentFile);
    fclose(currentFile);
    free(projectSrc);
    if(argFlags & 0x01) puts("DONE");
    
    if(argFlags & 0x02)
    {
        if(argFlags & 0x01) printf("creating 'build.sh'... ");
        // create build script
        char* buildScript = (char*)calloc(dirSize + 10, 1);
        strcat(buildScript, projectPath);
        strcat(buildScript, "/build.sh");
        currentFile = fopen(buildScript, "w");
        fputs("cmake . -B ./build\ncd build\nmake", currentFile);
        free(buildScript);
        if(argFlags & 0x01) puts("DONE");
    }
    
    puts("Pico Project Created.");
    return 0;
}
