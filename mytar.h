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
#include <grp.h>
#include <pwd.h>
#define BLOCK 512
#define BUFF_SIZE 4096
#define NAME_LENGTH 100
#define MODE_LENGTH 8
#define UID_LENGTH 8
#define GID_LENGTH 8
#define SIZE_LENGTH 12
#define MTIME_LENGTH 12
#define CHKSUM_LENGTH 8
#define LINKNAME_LENGTH 100
#define MAGIC_LENGTH 6
#define UNAME_LENGTH 32
#define GNAME_LENGTH 32
#define DEVMAJOR_LENGTH 8
#define DEVMINOR_LENGTH 8
#define PREFIX_LENGTH 155
#define FILLED_UMASK 07777

void ctar(int argc, char *argv[], int v);
void readCPath(char *path, int output, int v);
void createHeader(char typeflag, struct stat sb, char *path, int output, int v);
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
