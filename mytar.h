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
#define BUFF_SIZE 4096
void ctar(int argc, char *argv[], int v, int S);
void readCPath(char *path, int output, int v, int S);
void createHeader(struct stat *sb, char *path, int output, int v, int S);
int NotatEnd(int tar_file);
int ttar(char *arguments[], int vFlag, int SFlag);
int trecurse_through_path(int tar_file, int verbose);
int NotatEnd(int tar_file);
int ttar(char *arguments[], int vFlag, int SFlag);
int trecurse_through_path(int tar_file, int verbose);

typedef struct header_struct {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char empty[12];
} header_struct;
#endif
