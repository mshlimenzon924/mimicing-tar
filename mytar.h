#ifndef MYTAR
#define MYTAR

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>
#include <utime.h>
#include <arpa/inet.h>
#include <grp.h>
#include <pwd.h>
#define BLOCK 512
#define BUFF_SIZE 4096
#define NAME_LENGTH 100
#define MAX_PATH 256
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
#define MAX_ID 07777777

void ctar(int argc, char *argv[], int v);
void readCPath(char *path, char *name, int output, int v);
int insert_special_int(char *where, size_t size, int32_t val);
void createHeader(char typeflag, struct stat sb,
		  char *path,char *link, int output, int v);
int NotatEnd(int tar_file);
int ttar(char *arguments[], int argc, int vFlag, int SFlag);
int trecurse_through_path(int tar_file, int verbose, int is_path, char* name);
int NotatEnd(int tar_file);
int xtar_recurse(int tar_file, int verbose, int is_path, 
char** path, int num_p);
int xtar(char *arguments[], int argc, int v, int S);

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


