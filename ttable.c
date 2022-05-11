
#include "mytar.h"


int main(){
    return 0;
}

int ttar(char *arguments[], int vFlag, int SFlag) {
  /* Declares Variables here */
  /* Check to make sure this is how you can open tar_file */
  int tar_file = open(arguments[2], O_RDONLY);

  /* Takes given archive file */
  /* Evaluates if there is a path or not */


  /* Recursively run through tree within archive */
  while (NotatEnd(tar_file)){
    lseek(tar_file, SEEK_CUR, -1024);
    trecurse_through_path(tar_file, vFlag);
  }


  /* Going through each file in order, if it is a directory, 
    open directory and recurse through it, until last file in root is read */
  
  /* Print out the files as you go */

  /* Return without errors */
  return 0;
}

int trecurse_through_path(int tar_file, int verbose){
  /* Declares Variables */
  char *delete_block = (char *)malloc(sizeof(char) * 100);
  char *output = (char*)malloc(sizeof(char) * 356);
  char *size_buff = (char*)malloc(sizeof(char) * 8);
  char *full_name = (char*)malloc(sizeof(char) * 256);
  int num_blocks = 0;
  int index = 0;
  char *eptr;
  int fail, i, mask, value, year, month, day, hour, minute, times;
  char *name = (char*)malloc(sizeof(char) * 100);
  char *mode = (char*)malloc(sizeof(char) * 8);
  char *size = (char*)malloc(sizeof(char) * 12);
  char *mtime = (char*)malloc(sizeof(char) * 12);
  char *type = (char*)malloc(sizeof(char));
  char *uname= (char*)malloc(sizeof(char) * 32);
  char *gname = (char*)malloc(sizeof(char) * 32);
  char *prefix = (char*)malloc(sizeof(char) * 8);

  read(tar_file, name, 100);
  read(tar_file, mode,  8);
  read(tar_file, delete_block, 16);
  read(tar_file, size, 12);
  read(tar_file, mtime, 12);
  read(tar_file, delete_block, 8);
  read(tar_file, type, 1);
  read(tar_file, delete_block, 108);
  read(tar_file, uname, 32);
  read(tar_file, gname, 32);
  read(tar_file, delete_block, 16);
  read(tar_file, prefix, 8);
  free(delete_block);


  /* PERMISSIONS */
  /* Get type for permissions */
  if (*type == '2'){
    /* Symbolic Link type */
    output[index++] = 'l';

  } else if (*type == '5'){
    /* Directory type */
    output[index++] = 'd';

  } else {
    /* Other file type */
    output[index++] = '-';
  }

  /* Convert octal to permission */
  /* I'm very unsure about the type format of this */
  for (i = 4; i < 7; i++){
    mask = mode[4] - 48;

    if (mask >= 4){
      output[index++] = 'r';
      mask -= 4;
    } else {
      output[index++] = '-';
    }

    if (mask >= 2){
      output[index++] = 'w';
      mask -= 2;
    } else {
      output[index++] = '-';
    }

    if (mask >= 1){
      output[index++] = 'x';
    } else {
      output[index++] = '-';
    }
  }

  /* OWNER / GROUP NAMES */
  i = 0;
  while (uname[i] && i < 8){
    output[index++] = uname[i++];
  }
  output[index++] = '/';
  i = 0;
  while (gname[i] && i < 8){
    output[index++] = gname[i++];
  }

  while(index != 27){
    output[index++] = NULL;
  }

  /* SIZE */
  /* Convert from octal to int and then into a string */
  value = strtol(size, &eptr, 8);
  itoa(value, size_buff, 10);
  for(i = 0; i < 10; i++){
    output[index++] = size_buff[i];
  }

  /* M TIME */
  value = strtol(mtime, &eptr, 8);

  /* Year */
  times = value / 31536000;
  value -= 31536000 * times;
  year = 1970 + times;

  /* Month */
  times = value / 2628000;
  value -= 2628000 * times;
  month = times;

  /* Day */
  times = value / 86400;
  value -= 86400 * times;
  day = times;

  /* Hour */
  times = value / 3600;
  value -= 3600 * times;
  hour = times;

  /* Minutes */
  times = value / 60;
  value -= 60 * times;
  minute = times;

  /* Put it all together */
  itoa(year, &output[index], 10);
  index += 4;
  itoa(month, &output[index], 10);
  index += 2;
  itoa(day, &output[index], 10);
  index += 2;
  itoa(hour, &output[index], 10);
  index += 2;
  itoa(minute, &output[index], 10);
  index += 2;

  /* Name */
  i = 0;
  while(prefix[i]){
    output[index++] = prefix[i++];
  }
  output[index++] = '/';
  i = 0;
  while(name[i]){
    output[index++] = name[i++];
  }

  output[index] = NULL;

  write(STDOUT_FILENO, output, strlen(output));

  /* Read out contents into nowhere */
  value /= 512;
  for(i = 0; i < value; i++){
    char *delete_block = (char*)malloc(sizeof(char) * 512);
    read(tar_file, delete_block, 512);
    free(delete_block);
  }

  free(name);
  free(mode);
  free(size);
  free(mtime);
  free(type);
  free(uname);
  free(gname);
  free(prefix);
  free(output);
  free(size_buff);
  free(full_name);
  
  return 0;

}

/* Function to see if we're at the end */
int NotatEnd(int tar_file){
  
  char *two_blocks = (char *)malloc(sizeof(char) * 1024);
  int fail, i;
  int allZero = 0;
  fail = read(tar_file, two_blocks, 1024);
  for (i = 0; i < 1024 && !allZero; i++){
    allZero = allZero || two_blocks[i];
  }

  free(two_blocks);

  return allZero;


}