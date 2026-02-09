#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filedata.h"

#define LOG(msg) do { if(Options.verbose) printf(msg); } while(0)
#define LOGF(msg, ...) do { if(Options.verbose) printf(msg, __VA_ARGS__); } while(0)
#define ERR(msg) do { fprintf(stderr, msg) } while(0)
#define ERRF(msg, ...) do { fprintf(stderr, msg, __VA_ARGS__); } while(0)

#ifdef _WIN32
#define IMPORT "\\external\\pico_sdk_import.cmake"
#define PATH_SEP '\\'
void mkdir(const char* dir) {
    int len = strlen(dir);
    char str[len+6];
    str[len+5] = 0x00;
    sprintf(str, "mkdir %s", dir);
    system(str);
}
#endif
#ifdef __linux__
#define PATH_SEP '/'
#define IMPORT "/external/pico_sdk_import.cmake"
void mkdir(const char* dir) {
    int len = strlen(dir);
    char str[len+9];
    str[len+8] = 0x00;
    sprintf(str, "mkdir -p %s", dir);
    system(str);
}
#endif

#define PICOFILE "pico_sdk_import.cmake"

struct options_s {
    unsigned char verbose;
} Options;

const char* helpStr =   "SYNTAX: MakePicoProject <Project Name> [options]\n" \
                        "Creates a new project folder for a Project to be ran on a Raspberry Pi Pico.\n" \
                        "Options:\n" \
                        "  -sdk=[path]  Sets custom Pico SDK path (Default: /usr/share/pico-sdk)\n" \
                        "  -v           Prints out what is currently being done";

int main(int argc, char** argv) {
    if(argc == 1) {
        puts(helpStr);
        return 0;
    }
    memset((void*)&Options, 0x00, sizeof(Options));

    char* importPath = NULL;

    char* projectName = NULL;

    for(int i=1;i<argc;i++) {
        if(strncmp(argv[i], "-sdk", 4) == 0) {
            int ilen = strlen(IMPORT);
            int slen = strlen(argv[i]+5);
            char path[slen+1];
            path[slen] = 0x00;
            strcpy(path, argv[i] + 5);
            if(path[slen-1] == PATH_SEP) {
                path[slen-1] = 0x00;
                slen--;
            }
            importPath = (char*)malloc(slen + ilen + 1);
            importPath[slen + ilen] = 0x00;
            strcpy(importPath, path);
            strcat(importPath, IMPORT);
        }
        else if(strcmp(argv[i], "-v") == 0) {
            Options.verbose = 1;
        }
        else {
            projectName = argv[i];
        }
    }

    if(importPath == NULL) {
        char* defaultImport = getenv("PICO_SDK_PATH");
        importPath = (char*)malloc(strlen(defaultImport) + strlen(IMPORT)+1);
        strcpy(importPath, defaultImport);
        strcat(importPath, IMPORT);
        importPath[strlen(defaultImport) + strlen(IMPORT)] = 0x00;
    }

    int projLen = strlen(projectName);

    LOG("Making project folder... ");
    mkdir(projectName);
    LOG("DONE\n");

    LOG("Importing pico_sdk_import.cmake... ");
    char picoImportPath[projLen + strlen(PICOFILE) + 2];
    sprintf(picoImportPath, "%s%c%s", projectName, PATH_SEP, PICOFILE);
    picoImportPath[projLen + strlen(PICOFILE)+1] = 0x00;
    FILE* fpImport = fopen(importPath, "r");
    if(fpImport == NULL)
    {
        ERRF("ERROR!\n%s not found.\n", importPath);
        return 1;
    }
    fseek(fpImport, 0, SEEK_END);
    unsigned long long size = ftell(fpImport);
    fseek(fpImport, 0, SEEK_SET);
    unsigned char* buf = (unsigned char*)malloc(size);
    fread(buf, 1, size, fpImport);
    fclose(fpImport);
    FILE* fpExport = fopen(picoImportPath, "w+");
    fwrite(buf, 1, size, fpExport);
    fclose(fpExport);
    free(buf);
    LOG("DONE\n");

    char fname[projLen * 2 + 4];
    strcpy(fname, projectName);
    fname[projLen] = PATH_SEP;
    strcpy(fname+projLen+1, projectName);
    fname[projLen*2+1] = '.';
    fname[projLen*2+3] = 0x00;
    FILE* fp = NULL;

    LOGF("Generating %s.h... ", projectName);
    fname[projLen*2+2] = 'h';
    fp = fopen(fname, "w+");
    if(fp == NULL)
    {
        ERRF("ERROR!\n%s couldn't be written to.\n", fname);
        perror("");
        return 1;
    }
    fprintf(fp, "%s", src_header);
    fclose(fp);
    LOG("DONE\n");

    LOGF("Generating %s.c... ", projectName);
    fname[projLen*2+2] = 'c';
    fp = fopen(fname, "w+");
    if(fp == NULL)
    {
        ERRF("ERROR!\n%s couldn't be written to.\n", fname);
        perror("");
        return 1;
    }
    fprintf(fp, "#include \"%s.h\"\n", projectName);
    fprintf(fp, "%s", src_impl);
    fclose(fp);
    LOG("DONE\n");

    LOG("Generating main.c... ");
    char mname[projLen + 1 + 7];
    strncpy(mname, fname, projLen);
    mname[projLen] = PATH_SEP;
    strcpy(mname+projLen+1, "main.c");
    mname[projLen+7] = 0x00;
    fp = fopen(mname, "w+");
    if(fp == NULL)
    {
        ERRF("ERROR!\n%s couldn't be written to.\n", fname);
        perror("");
        return 1;
    }
    fprintf(fp, "#include \"%s.h\"\n", projectName);
    fprintf(fp, "%s", src_main);
    fclose(fp);
    LOG("DONE\n");

    LOG("Generating CMakeLists.txt... ");
    char cmlname[projLen + 1 + 15];
    strncpy(cmlname, fname, projLen);
    cmlname[projLen] = PATH_SEP;
    strcpy(cmlname+projLen+1, "CMakeLists.txt");
    cmlname[projLen+15] = 0x00;
    fp = fopen(cmlname, "w+");
    if(fp == NULL)
    {
        ERRF("ERROR!\n%s couldn't be written to.\n", fname);
        perror("");
        return 1;
    }
    fprintf(fp, src_cml, projectName, projectName, projectName, projectName, projectName);
    fclose(fp);
    LOG("DONE\n");

    return 0;
}
