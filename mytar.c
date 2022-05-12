/* Mytar Implementation by Mira Shlimenzon 
   and Emma Kester.
   A program to mimic the tar UNIX function
   by bundling files or  directories into a
   single file.
 */
#include "mytar.h"

int main(int argc, char *argv[]) {
  int c = 0, t = 0, x = 0;
  int v = 0, f= 0, S = 0;
  int i = 0;

  if(argc < 3)  {
    printf ("Missing argument/s\n"); 
    printf("Usage: mytar [ctxvS]f tarfile\n");
    exit(-1);
  }
  for(; i < strlen(argv[1]); i++) {
    if(argv[1][i] == 'c') {
      c++;
    }
    else if(argv[1][i] == 't') {
      t++;
    }
    else if(argv[1][i] == 'x') {
      x++;
    }
    else if(argv[1][i] == 'v') {
      v++;
    }
    else if(argv[1][i] == 'S') {
      S++;
    }
    else if(argv[1][i] == 'f') {
      f++;
    }
    else {  
      printf("Usage: mytar [ctxvS]f tarfile\n");
      exit(-1);
    }
  }
  /* ctv empty */
  if((c + t + x) == 0) {
    printf ("Need to specify one of -c, -t, or -x\n"); 
    printf("Usage: mytar [ctxvS]f tarfile\n");
    exit(-1);
  }
  /* if more than one letter selected */
  if((c + t + x) > 1){ 
    printf ("Only choose one of -c, -t, or -x\n"); 
    printf("Usage: mytar [ctxvS]f tarfile\n");
    exit(-1);
  }
  /* missing f */
  if(!f) {
    printf ("Missing -f\n"); 
    printf("Usage: mytar [ctxvS]f tarfile\n");
    exit(-1);
  }
  if((f - 1 > 0) || (v - 1 > 0) || (S- 1 > 0)) {
    printf ("Only choose one for -v, -S, and -f\n"); 
    printf("Usage: mytar [ctxvS]f tarfile\n");
  }
  
  if(c) {
    if(argc == 3) {
      printf("No files or directories specified\n");
      printf("Usage: mytar [ctxvS]f tarfile path\n");
      exit(-1);
    }
    /*check here that we have an argv[3] or else say we need a path */
    ctar(argc, argv, v);

  }
  else if(t){
    ttar(argv, v, S);
  } else if(x){
    printf("x");
  }
  
return 0;
}

/* Creates tar file with given path list */
void ctar(int argc, char *argv[], int v) {
  int output; 
  int i;
  char block[BLOCK];
  
  for(i = 0; i < BLOCK; i++) {
    block[i] = '\0';
  }

  /* create the tar file */
  if((output = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
    perror("open");
    exit(-1);  
  }
  /* calls for each of the paths readCPath() */
  for(i = 3; i < argc; i++){
    readCPath(argv[i], output, v);
  }
  
  /* Write 2 null blocks at the end */
  for(i = 0; i < 2; i++) {
    if(write(output, block, BLOCK)) {
      perror("write");
      exit(-1);
    }
  }
}

/* Recursively reads all directories and files in path 
   And calls other function createHeader for it.
   Returns nothing. */
void readCPath(char *path, int output, int v){
  DIR *d;
  struct stat lst_b;
  struct stat st_b;
  struct dirent *entry;
  char *cur_path = NULL;
  
  // printf("path %s\n", path);
  /* getting information about link if link */
  if(lstat(path, &lst_b)){
    perror("lstat");
    exit(-1);
  }
  /* getting information about file links points to */
  if(stat(path, &st_b)){
    perror("stat");
    exit(-1);
  }
  /* Regular file condition */
  if(S_ISREG(lst_b.st_mode)) {
    createHeader('0', &lst_b, path, output, v);
  }
  /* Regular file sym link condition */
  else if(S_ISLNK(lst_b.st_mode) && S_ISREG(st_b.st_mode)){
    createHeader('2', &lst_b, path, output, v); 
    createHeader('0', &st_b, path, output, v); 
  }
  /* Directory condition */
  else if(S_ISDIR(st_b.st_mode)){
    /* Sym Link Directory */
    if(S_ISLNK(lst_b.st_mode)){ 
      createHeader('2', &lst_b, path, output, v); 
    }
    createHeader('5', &st_b, path, output, v);

    if((d = opendir(path)) == NULL) {
      perror("open");
      exit(-1);
    }

    /* now looping through directory to call readCPath on it and all its files */
    while((entry = readdir(d)) != NULL) {
      // printf("entry");
      if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")){
        // printf("dname %s\n", entry->d_name);
        cur_path = path;
        strcat(path, "/");
        strcat(path, entry->d_name);
        readCPath(cur_path, output, v); 
      }
    }
    if(closedir(d) == -1){
      perror("close");
      exit(-1);
    }
  }
  else{
    printf("A file that it isn't supported with mytar was found.\n");
    printf("Mytar only supports regular files, directories, or sym links.\n");
  }
 
}  

/* Using given path, takes all data and puts it into a struct */
/* struct that's a header- where we fill in the correct information */
/* at end of header format it and place it into the tar file */
void createHeader(char typeflag, struct stat *sb, char *path, int output, int v) {
  header_struct header;
  struct passwd *pw;
  struct group *grp;
  int open_file;
  int num;
  char *buffer;
  int i = 0, sum = 0;

  buffer = (char *)malloc(BUFF_SIZE);
  if(!buffer) {
    perror("malloc");
    exit(-1);
  }

  if(v) {
    printf("%s\n", path); 
  }
  /*fill header with correct stuff */
  memset(&header, 0, BLOCK);
  if(strlen(path) <= NAME_LENGTH) {
   memcpy(header.name, path, strlen(path));
  }
  else {
    memcpy(header.name, path, NAME_LENGTH);
    memcpy(header.prefix, path + NAME_LENGTH, strlen(path) - NAME_LENGTH);
  }
  //how do I tell how much I pad it with?
  snprintf(header.mode, 8, "%o", sb.st_mode & 07777); //&07?
  snprintf(header.uid, 8, "%o", sb.st_uid); //%07o?
  snprintf(header.gid, 8, "%o", sb.st_gid);
  if(S_ISREG(sb.st_mode)) {
    snprintf(header.size, 12, "%o", sb.st_size);
  }
  else {
    snprintf(header.size, 12, "%o", 0);
  }
  snprintf(header.mtime, 12, "%o", sb.st_mtime);
  header.typeflag = typeflag;
  if(S_ISLNK(sb.st_mode)) {
    if(readlink(header.linkname, path, LINKNAME_LENGTH) < 0){
      perror("readlink");
      exit(-1);
    }
  }
  strcpy(header.magic, "ustar"); 
  header.version[0] = '0';
  header.version[1] = '0';
  pw = getpwid(sb.st_uid);
  memcpy(header.uname, pw->pw_name, UNAME_LENGTH - 1);
  grp = getgrgrid(sb.st_gid);
  memcpy(header.uname, grp->gr_name, UNAME_LENGTH - 1);
  snprintf(header.devmajor, 12, "%o", sb.st_dev);
  snprintf(header.devminor, 12, "%o", sb.st_rdev);
  for(i = 0, i < BLOCK, i++) {
    sum += (unsigned char)(header[i]);
  }
  snprintf(header.chksum, 12, "%o", sum);

  /*write into output the header */
  if(write(output, header.name, BLOCK)) {
      perror("write");
      exit(-1);
    } 

  /* If regular file, add all of files contents */
  if(S_ISREG(sb.st_mode)) {
    if((open_file = open(path, O_RDONLY)) < 0) {
      perror(path);
      exit(-1);
    }
    while((num = read(open_file, buffer, BUFF_SIZE)) > 0){
      if(write(output, buffer, num) != num) {
        perror("write");
        exit(-1);
      }
    }
    if(num < 0) {
      perror("read");
      exit(-1);
    }
  } 
  free(buffer);
}


int ttar(char *arguments[], int vFlag, int SFlag) {
  /* Declares Variables here */
  /* Check to make sure this is how you can open tar_file */
  int tar_file = open(arguments[2], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);
  int offset;
  if (tar_file < 0){
    printf("File unable to be opened.\n");
    exit(EXIT_FAILURE);
  }

  /* Takes given archive file */
  /* Evaluates if there is a path or not */


  /* Recursively run through tree within archive */
  while (NotatEnd(tar_file)){
    offset = lseek(tar_file, -1024, SEEK_CUR);
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
  char *delete_block, *eptr, *writer;
  char *output = (char*)malloc(sizeof(char) * 356);
  char *size_buff = (char*)malloc(sizeof(char) * 8);
  char typ;
  int fail, i, mask, value, year, month, day, hour, minute, times;
  char *name = (char*)malloc(sizeof(char) * 100);
  char *mode = (char*)malloc(sizeof(char) * 8);
  char *size = (char*)malloc(sizeof(char) * 12);
  char *mtime = (char*)malloc(sizeof(char) * 12);
  char *type = (char*)malloc(sizeof(char));
  char *uname= (char*)malloc(sizeof(char) * 32);
  char *gname = (char*)malloc(sizeof(char) * 32);
  char *prefix = (char*)malloc(sizeof(char) * 155);

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
  read(tar_file, prefix, 155);

  /* PERMISSIONS */
  /* Get type for permissions */
  typ = type[0];
  if (typ == '2'){
    /* Symbolic Link type */
    printf("l");

  } else if (typ == '5'){
    /* Directory type */
    printf("d");

  } else {
    /* Other file type */
    printf("-");
  }

  /* Convert octal to permission */
  /* I'm very unsure about the type format of this */
  for (i = 4; i < 7; i++){
    mask = mode[4] - 48;

    if (mask >= 4){
      printf("r");
      mask -= 4;
    } else {
      printf("-");
    }

    if (mask >= 2){
      printf("w");
      mask -= 2;
    } else {
      printf("-");
    }

    if (mask >= 1){
      printf("x");
    } else {
      printf("-");
    }
  }
  printf(" ");

  /* OWNER / GROUP NAMES */
  i = 0;
  while (uname[i] && i < 8){
    printf("%c", uname[i++]);
  }
  printf("/");
  i = 0;
  while (gname[i] && i < 8){
    printf("%c", gname[i++]);
  }

  printf(" ");

  /* SIZE */
  /* Convert from octal to int and then prints them out in 8 characters */
  value = strtol(size, &eptr, 8);
  sprintf(size, "%d", 8);

  /* Read out contents into nowhere */
  value /= 512;
  for(i = 0; i < value; i++){
    delete_block = (char*)malloc(sizeof(char) * 512);
    fail = read(tar_file, delete_block, 512);
    free(delete_block);
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
  writer = (char*)malloc(sizeof(char) * 6);
  sprintf(writer, " %d-", year);
  fail = write(STDOUT_FILENO, writer, 6);
  
  sprintf(writer, "%d-", month);
  fail = write(STDOUT_FILENO, writer, 3);
  sprintf(writer, "%d ", day);
  fail = write(STDOUT_FILENO, writer, 3);
  sprintf(writer, "%d:", hour);
  fail = write(STDOUT_FILENO, writer, 3);
  sprintf(writer, "%d ", minute);
  fail = write(STDOUT_FILENO, writer, 3);
  free(writer);

  /* Name */
  printf("%s/%s", prefix, name);

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
  
  return 0;

}

/* Function to see if we're at the end */
int NotatEnd(int tar_file){
  
  char *two_blocks = (char *)malloc(sizeof(char) * 1024);
  int fail, i;
  int allZero = 0;
  fail = read(tar_file, two_blocks, 1024);
  if (fail < 0){
    printf("Read failed.\n");
    exit(EXIT_FAILURE);
  }
  for (i = 0; i < 1024 && !allZero; i++){
    allZero |= two_blocks[i];
  }

  free(two_blocks);

  return allZero;


}
 

