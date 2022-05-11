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
int NotatEnd(int tar_file);
int ttar(char *arguments[], int vFlag, int SFlag);
int trecurse_through_path(int tar_file, int verbose);
#endif

/* Function Prototypes */
int NotatEnd(int tar_file);
int ttar(char *arguments[], int vFlag, int SFlag);
int trecurse_through_path(int tar_file, int verbose);
