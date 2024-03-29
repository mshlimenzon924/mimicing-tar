/* Header file for mytar. 
   By Mira Shlimenzon and Emma Kester */

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
#define TWOBLOCKS -1024
#define ALPHABET 356
#define BUFF_SIZE 4096
#define MAX_PATH 256
#define NAME_LENGTH 100
#define MODE_LENGTH 8
#define UID_LENGTH 8
#define GID_LENGTH 8
#define SIZE_LENGTH 12
#define MTIME_LENGTH 12
#define CHKSUM_LENGTH 8
#define TYPE_LENGTH 1
#define LINKNAME_LENGTH 100
#define MAGIC_LENGTH 6
#define VERSION_LENGTH 2
#define UNAME_LENGTH 32
#define GNAME_LENGTH 32
#define DEVMAJOR_LENGTH 8
#define DEVMINOR_LENGTH 8
#define PREFIX_LENGTH 155
#define FILLED_UMASK 07777
#define EXCESS_LENGTH 12
#define MAX_ID 07777777
#define MAX_SIZE 077777777777
#define DIRTYPEFLAG '5'
#define LINKTYPEFLAG '2'
#define REGTYPEFLAG '0'
#define OTHERREGTYPEFLAG '\0'
#define LIST_PERMISSIONS 10
#define LIST_OW_GR 17
#define LIST_SIZE 8
#define ARGV_FIRST_PATH_INDEX 3
#define ASCII 48

void ctar(int argc, char *argv[], int v, int S);
void readCPath(char *path, int output, int v, int S);
int insert_special_int(char *where, size_t size, int32_t val);
void createHeader(char typeflag, struct stat sb,
char *path, int output, int v, int S);
int NotatEnd(int tar_file);
int ttar(char *arguments[], int argc, int vFlag, int SFlag);
int trecurse_through_path(int tar_file, int verbose, 
int is_path, int argc, char** path);
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


