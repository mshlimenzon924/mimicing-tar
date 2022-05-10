#ifndef MYTAR
#define MYTAR

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#define BLOCK 512
void ctar(int argc, char *argv[], int v, int S);
void readCPath(char *path, int output, int v, int S);
void createHeader(struct stat *sb, char *path, int output, int v, int S);
#endif
