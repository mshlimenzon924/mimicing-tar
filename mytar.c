/* Mytar Implementation by Mira Shlimenzon 
   and Emma Kester.
   A program to mimic the tar UNIX function
   by bundling files or  directories into a
   single file.
 */

// do we need to use unbuffered io?
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

  if(close(output)){
    perror("close");
    exit(-1);
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
    createHeader('0', lst_b, path, output, v);
  }
  /* Regular file sym link condition */
  else if(S_ISLNK(lst_b.st_mode) && S_ISREG(st_b.st_mode)){
    createHeader('2', lst_b, path, output, v); 
    createHeader('0', st_b, path, output, v); 
  }
  /* Directory condition */
  else if(S_ISDIR(st_b.st_mode)){
    /* Sym Link Directory */
    if(S_ISLNK(lst_b.st_mode)){ 
      createHeader('2', lst_b, path, output, v); 
    }
    createHeader('5', st_b, path, output, v);

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
        strcat(cur_path, "/");
        strcat(cur_path, entry->d_name);
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
void createHeader(char typeflag, struct stat sb, char *path, int output, int v) {
  header_struct header;
  struct passwd *pw;
  struct group *grp;
  int open_file;
  char *path_help;
  int num;
  char *buffer;
  int i = 0, sum = 0;
  int count = 0;

  buffer = (char *)malloc(BUFF_SIZE);
  if(!buffer) {
    perror("malloc");
    exit(-1);
  }

  if(v) {
    printf("%s\n", path); 
    return;
  }
  /*fill header with correct stuff */
  memset(&header, 0, BLOCK);
  if(strlen(path) <= NAME_LENGTH) {
    memcpy(header.name, path, strlen(path)); 
  }
  else {
    i = NAME_LENGTH - 1;
    path_help = path + i;
    while(i > 0) {
      if(*path_help == '/') {
        break;
      }
      path_help--;
      i--;
    }
    memcpy(header.name, path, i);
    memcpy(header.prefix, path + i, strlen(path) - i);
  }
  snprintf(header.mode, MODE_LENGTH, "%07o", sb.st_mode & 07777);
  snprintf(header.uid, UID_LENGTH, "%07o", sb.st_uid);
  snprintf(header.gid, GID_LENGTH, "%07o", sb.st_gid);
  /* chksum first filled with spaces */
  snprintf(header.chksum, CHKSUM_LENGTH, "        ");
  if(S_ISREG(sb.st_mode)) {
    snprintf(header.size, SIZE_LENGTH, "%011o", (int)sb.st_size);
  }
  else {
    snprintf(header.size, SIZE_LENGTH, "%011o", 0);
  }
  snprintf(header.mtime, MTIME_LENGTH, "%011o", (int)sb.st_mtime);
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
  if((pw = getpwuid(sb.st_uid)) == NULL) {
    perror("getpwuid");
    exit(-1);
  }
  memcpy(header.uname, pw->pw_name, UNAME_LENGTH - 1);
  
  if((grp = getgrgid(sb.st_gid)) == NULL) {
    perror("getgrgid");
    exit(-1);
  }
  memcpy(header.uname, grp->gr_name, UNAME_LENGTH - 1);

  /* device number doesn't matter */
  path_help = (char *)&header;
  for(i = 0; i < BLOCK; i++) {
    sum += (unsigned char)path_help[i];
  }
  snprintf(header.chksum, CHKSUM_LENGTH, "%07o", sum);

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
      count += num;
    }

    if(num < 0) {
      perror("read");
      exit(-1);
    }

    if(count % BUFF_SIZE != 0) {
      memset(buffer, 0, BUFF_SIZE);
      if(write(output, buffer, count % BUFF_SIZE) != 
         count % BUFF_SIZE) {
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

