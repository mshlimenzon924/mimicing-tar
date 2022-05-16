/* Mytar Implementation by Mira Shlimenzon 
   and Emma Kester.
   A program to mimic the tar UNIX function
   by bundling files or  directories into a
   single file.
 */

#include "mytar.h"

/* Main function. Returns 0 on success */
int main(int argc, char *argv[]) {
  /* Flags */
  int c = 0, t = 0, x = 0;
  int v = 0, f= 0, S = 0;
  int i = 0; /* variable for for loop */

  /* If no arguments */
  if(argc < 3)  {
    perror("Missing argument/s\n"); 
    perror("Usage: mytar [ctxvS]f tarfile\n");
    exit(-1);
  }
  /* For loop to fill up flags */
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
      perror("Usage: mytar [ctxvS]f tarfile\n");
      exit(-1);
    }
  }
  /* if ctv empty */
  if((c + t + x) == 0) {
    perror("Need to specify one of -c, -t, or -x\n"); 
    perror("Usage: mytar [ctxvS]f tarfile\n");
    exit(-1);
  }
  /* if more than one letter selected */
  if((c + t + x) > 1){ 
    perror("Only choose one of -c, -t, or -x\n"); 
    perror("Usage: mytar [ctxvS]f tarfile\n");
    exit(-1);
  }
  /* if missing f */
  if(!f) {
    perror("Missing -f\n"); 
    perror("Usage: mytar [ctxvS]f tarfile\n");
    exit(-1);
  }
  /* If more than one is selected */
  if((f - 1 > 0) || (v - 1 > 0) || (S- 1 > 0)) {
    perror("Only choose one for -v, -S, and -f\n"); 
    perror("Usage: mytar [ctxvS]f tarfile\n");
  }
  
  if(c) {
    /* If no path name is given */
    if(argc == 3) {
      perror("No files or directories specified\n");
      perror("Usage: mytar [ctxvS]f tarfile path\n");
      exit(-1);
    }
    ctar(argc, argv, v);
  }
  else if(t){
    printf("t");    
  } else if(x){
    printf("x");
  } 
return 0;
}

/* Creates tar file with given path list. 
   Returns nothing.  */
void ctar(int argc, char *argv[], int v) {
  int output; 
  char block[BLOCK];
  int i;
  
  /* Block of 512 nulls */
  for(i = 0; i < BLOCK; i++) {
    block[i] = '\0';
  }

  /* create the tar file */
  if((output = open(argv[2],
      O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
    perror("open");
    exit(-1);  
  }
  /* Calls for each of the paths to be put through readCPath() 
     and will create a header + if regular file ouput contents */
  for(i = 3; i < argc; i++){
    readCPath(argv[i], output, v);
  }
  
  /* Write 2 null blocks at the end */
  if(write(output, block, BLOCK) == -1) {
    perror("write");
    exit(-1);
  }

  if(write(output, block, BLOCK) == -1) {
    perror("write");
    exit(-1);
  }

  if(close(output) == -1){
    perror("close");
    exit(-1);
  }
}


/* Recursively reads all directories, files, sym links in path 
   And calls other function createHeader for it.
   Returns nothing. */
void readCPath(char *path, int output, int v){
  DIR *d;
  struct stat lst_b;
  struct stat st_b;
  struct dirent *entry;
  char cur_path[PATH_MAX + 1];
  
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

    /* now looping through dir to call readCPath on it and all its files */
    while((entry = readdir(d)) != NULL) {
      if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")){
        strcpy(cur_path, path);
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
    perror("A file that it isn't supported with mytar was found.\n");
    perror("Mytar only supports regular files");
    perror(", directories, or sym links.\n");
  }
 
}  

/* Using given path, takes all data and puts it into a struct
   struct that's a header- where we fill in the correct information 
   at end of header format it and place it into the tar file.
   Returns nothing.
   */
void createHeader(char typeflag, struct stat sb, 
                  char *path, int output, int v) {
  header_struct header;
  struct passwd *pw;
  struct group *grp;
  int open_file;
  char *path_help;
  char *buffer;
  int num = 0;
  int i = 0, j = 0, sum = 0;
  int count = 0;

  buffer = (char *)malloc(BUFF_SIZE);
  if(!buffer) {
    perror("malloc");
    exit(-1);
  }

  /* Print out path if v */
  if(v) {
    printf("%s\n", path); 
  }

  /*fill header with correct stuff */
  memset(&header, 0, BLOCK);
  /* Name */
  if(strlen(path) <= NAME_LENGTH) {
    memcpy(header.name, path, strlen(path)); 
  }
  else if(strlen(path) > MAX_PATH) {
    perror("Path is too long.\n");
    return;
  }
  else {
    /* j is index that we have the slash */
    i = strlen(path) - 1;
    path_help = path + i; 
    /* Because slash is being taken out so 
       we check 101 indices instead */
    while(i > strlen(path) - NAME_LENGTH - 2) {  
      if(*path_help == '/') {
        j = i;
      }
      path_help--;
      i--;
    }
    /* If name cannot be partioned */
    if(j == 0) {
      perror("File path name cannot be partioned.\n");
      return;
    }
    memcpy(header.prefix, path, j);
    memcpy(header.name, path + j + 1, strlen(path) - j - 1);
  }
  /* Mode */
  snprintf(header.mode, MODE_LENGTH, "%07o", sb.st_mode & FILLED_UMASK);
  /* Uid */
  if(sb.st_uid < MAX_ID) {
    snprintf(header.uid, UID_LENGTH, "%07o", sb.st_uid);
  }
  else {
    if(insert_special_int(header.uid, UID_LENGTH, sb.st_uid)) {
      perror("Issue with shortening user ID.\n");
      return;
    }
  }
  /* Gid */
  snprintf(header.gid, GID_LENGTH, "%07o", sb.st_gid);
  /* chksum first filled with spaces */
  snprintf(header.chksum, CHKSUM_LENGTH, "        ");
  /* Size */
  if(S_ISREG(sb.st_mode)) {
    snprintf(header.size, SIZE_LENGTH, "%011o", (int)sb.st_size);
  }
  else {
    snprintf(header.size, SIZE_LENGTH, "%011o", 0);
  }
  /* Mtime */
  snprintf(header.mtime, MTIME_LENGTH, "%011o", (int)sb.st_mtime);
  header.typeflag = typeflag;
  /* Linkname */
  if(S_ISLNK(sb.st_mode)) {
    if(readlink(path, header.linkname, LINKNAME_LENGTH) < 0){
      perror("Issue with readlink.\n");
      return;
    }
  }
  /* Ustar and version */
  strcpy(header.magic, "ustar"); 
  header.version[0] = '0';
  header.version[1] = '0';
  /* Uname */
  if((pw = getpwuid(sb.st_uid)) == NULL) {
    perror("Issue with getpwuid of the uname.\n");
    return;
  }
  memcpy(header.uname, pw->pw_name, UNAME_LENGTH - 1);
  
  /* Gname */
  if((grp = getgrgid(sb.st_gid)) == NULL) {
    perror("Issue with getgruid of the gname.\n");
    return;
  }
  memcpy(header.gname, grp->gr_name, GNAME_LENGTH - 1);

  /* device number doesn't matter */

  /* Chksum */
  path_help = (char *)&header;
  for(i = 0; i < BLOCK; i++) {
    sum += (unsigned char)(path_help[i]);
  }
  snprintf(header.chksum, CHKSUM_LENGTH, "%07o", sum);

  /* For all file errors, it is my executive decision
     to exit and not ignore since we are touching
     memory and these errors appearing could be bigger 
     issues later */
  
  /* Write into output the header we just populated */
  if(write(output, path_help, BLOCK) == -1) {
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
      count = num;
    }

    if(num < 0) {
      perror("write");
      exit(-1);
    }
    /* If we didn't write in a full BLOCK, fill rest
       of block with zeroes */
    memset(buffer, 0, BUFF_SIZE);
    if(count % BLOCK != 0) {
      if(write(output, buffer, BLOCK - count % BLOCK) 
         != BLOCK - count % BLOCK) {
        perror("write");
        exit(-1);
      }
    }

    if(num < 0) {
      perror("write");
      exit(-1);
    }
  }

  if(close(open_file) == -1){
    perror("close");
    exit(-1);
  }
  free(buffer);
}


  /* Function Nico gave us */
  /* For interoperability with GNU tar. GNU seems to
   * set the high–order bit of the first byte, then
   * treat the rest of the field as a binary integer
   * in network byte order.
   * Insert the given integer into the given field
   * using this technique. Returns 0 on success, nonzero
   * otherwise.
   */
int insert_special_int(char *where, size_t size, int32_t val) {
  int err = 0;
  if(val < 0 || (size < sizeof(val))) { 
    err++;
  } 
  else {
    memset(where, 0, size); /* Clear out the buffer */
    *(int32_t *)(where + size - sizeof(val)) = htonl(val);
    /* place the int */
    *where |= 0x80; /* set that high–order bit */
  }
return err;
}


