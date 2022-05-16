/* Mytar Implementation by Mira Shlimenzon 
   and Emma Kester.
   A program to mimic the tar UNIX function
   by bundling files or  directories into a
   single file.
 */

#include "mytar.h"

/* Main function. Returns 0 on success */
int main(int argc, char *argv[]) {
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
  /* missing f */
  if(!f) {
    perror("Missing -f\n"); 
    perror("Usage: mytar [ctxvS]f tarfile\n");
    exit(-1);
  }
  if((f - 1 > 0) || (v - 1 > 0) || (S- 1 > 0)) {
    perror("Only choose one for -v, -S, and -f\n"); 
    perror("Usage: mytar [ctxvS]f tarfile\n");
  }
  
  if(c) {
    if(argc == 3) {
      perror("No files or directories specified\n");
      perror("Usage: mytar [ctxvS]f tarfile path\n");
      exit(-1);
    }
    ctar(argc, argv, v, S);

  }
  else if(t){
    ttar(argv, argc, v, S);
  } else if(x){
    xtar(argv, argc, v, S);
  }
  
return 0;
}

/* Creates tar file with given path list 
  Returns nothing */
void ctar(int argc, char *argv[], int v, int S) {
  int output; 
  int i;
  char block[BLOCK];
  
  for(i = 0; i < BLOCK; i++) {
    block[i] = '\0';
  }

  /* create the tar file */
  if((output = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 
  S_IRUSR | S_IWUSR)) == -1) {
    perror("open");
    exit(-1);  
  }
  /* calls for each of the paths readCPath() 
    and will create a header + if regular file ouput contents */  
  for (i = 3; i < argc; i++){
    readCPath(argv[i], output, v, S);
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

/* Recursively reads all directories and files in path 
   And calls other function createHeader for it.
   Returns nothing. */
void readCPath(char *path, int output, int v, int S){
  DIR *d;
  struct stat lst_b;
  struct stat st_b;
  struct dirent *entry;
  char cur_path[PATH_MAX + 1];
  
  /* getting information about link if link */
  if(lstat(path, &lst_b)){
    perror("lstat");
    return;
  }
  /* getting information about file links points to */
  if(stat(path, &st_b)){
    perror("stat");
    return;
  }

  /* Regular file condition */
  if(S_ISREG(lst_b.st_mode)) {
    createHeader('0', lst_b, path, output, v, S);
  }


  /* Regular file sym link condition */
  else if(S_ISLNK(lst_b.st_mode) && S_ISREG(st_b.st_mode)){
    createHeader('2', lst_b, path, output, v, S); 
    /*createHeader('0', st_b, path, output, v);*/ 
  }
  /* Directory condition */
  else if(S_ISDIR(st_b.st_mode)){
    /* Sym Link Directory */
    if(S_ISLNK(lst_b.st_mode)){ 
      createHeader('2', lst_b, path, output, v, S); 
    } else {
      createHeader('5', st_b, path,  output, v, S);
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
          readCPath(cur_path, output, v, S); 
        }
      }
      if(closedir(d) == -1){
        perror("close");
        exit(-1);
      }
    }
  }
  else{
    perror("A file that it isn't supported with mytar was found.\n");
    perror("Mytar only supports regular files");
    perror(", directories, or sym links.\n");
  }
 
}  

/* Using given path, takes all data and puts it into a struct */
/* struct that's a header- where we fill in the correct information */
/* at end of header format it and place it into the tar file */
void createHeader(char typeflag, struct stat sb,
 char *path, int output, int v, int S){
  header_struct header;
  struct passwd *pw;
  struct group *grp;
  int open_file = 0;
  char *path_help;
  char *buffer;
  int num = 0;
  int i = 0, j = 0, sum = 0;
  int count = 0;
  int chksum = 0;

  buffer = (char *)malloc(BUFF_SIZE);
  if(!buffer) {
    perror("malloc");
    exit(-1);
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
    //look over this!
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
  if(sb.st_uid <= MAX_ID) {
    if(S) {
      free(buffer);
      return;
    }
    snprintf(header.uid, UID_LENGTH, "%07o", sb.st_uid);
  }
  else {
    if(insert_special_int(header.uid, UID_LENGTH, sb.st_uid)) {
      perror("Issue with shortening user ID.\n");
      return;
    }
  }

  /* Gid */
  if(sb.st_gid <= MAX_ID) {
    if(S) {
      free(buffer);
      return;
    }
    snprintf(header.gid, GID_LENGTH, "%07o", sb.st_gid);
  }
  else {
    if(insert_special_int(header.gid, GID_LENGTH, sb.st_gid)) {
      perror("Issue with shortening user GID.\n");
      return;
    }
  }

  /* chksum first filled with spaces */
  snprintf(header.chksum, CHKSUM_LENGTH, "        ");
  /* Size */
  if(S_ISREG(sb.st_mode)) {
    if((int)sb.st_size > MAX_SIZE) {
      if(insert_special_int(header.size, SIZE_LENGTH, (int)sb.st_size)) {
        perror("Issue with shortening size.\n");
        return;
      }
    }
    else {
      snprintf(header.size, SIZE_LENGTH, "%011o", (int)sb.st_size);
    }
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

  if(strlen(pw->pw_name) < UNAME_LENGTH) {
    memcpy(header.uname, pw->pw_name, strlen(pw->pw_name));
  }
  else {
    memcpy(header.uname, pw->pw_name, UNAME_LENGTH - 1);
  }
  
  /* Gname */
  if((grp = getgrgid(sb.st_gid)) == NULL) {
    perror("Issue with getgruid of the gname.\n");
    return;
  }

  if(strlen(grp->gr_name) < GNAME_LENGTH) {
    memcpy(header.gname, grp->gr_name, strlen(grp->gr_name));
  }
  else {
    memcpy(header.gname, grp->gr_name, GNAME_LENGTH - 1);
  }

  /* device number doesn't matter */

  /* Chksum */
  for(i = 0; i < 8; i++){
    header.chksum[i] = ' ';
  }
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
      return;
  } 

  /* If regular file, add all of files contents */
  if(S_ISREG(sb.st_mode)) {
    if((open_file = open(path, O_RDONLY)) < 0) {
      perror(path);
      return;
    }
    while((num = read(open_file, buffer, BUFF_SIZE)) > 0){
      if(write(output, buffer, num) != num) {
        perror("write");
        return;
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
        return;
      }
    }

    if(num < 0) {
      perror("write");
      exit(-1);
    }

    /* Print out path if v */
    if(v) {
      printf("%s\n", path); 
    }

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



int xtar(char *arguments[], int argc, int v, int S) {
  /* Declares Variables here */
  /* Check to make sure this is how you can open tar_file */
  int tar_file = open(arguments[2], O_RDONLY | O_CREAT,
   S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);
  int offset, i;
  if (tar_file < 0){
    printf("File unable to be opened.\n");
    exit(EXIT_FAILURE);
  }

  /*Recursively run through tree within archive  */
  if (argc <= 3){
    while (NotatEnd(tar_file)){
      offset = lseek(tar_file, -1024, SEEK_CUR);
      if (offset < 0){
        printf(stderr, "Lseek error %d.\n", offset);
      }
      xtar_recurse(tar_file, v, 0, NULL, argc);
    }
  } else {
    
    while (NotatEnd(tar_file)){
      offset = lseek(tar_file, -1024, SEEK_CUR);
      if (offset < 0){
        printf(stderr, "Lseek error %d.\n", offset);
      }
      xtar_recurse(tar_file, v, 1, arguments, argc);
    }
    lseek(tar_file, SEEK_SET, 0);
      
    }

  return 0;
}

int xtar_recurse(int tar_file, int verbose, int is_path, 
char** path, int num_p) {
  /* Declares Variables */
  char *delete_block, *eptr, *writer;
  char *output = (char*)malloc(sizeof(char) * 356);
  char *size_buff = (char*)malloc(sizeof(char) * 8);
  char typ, *beginning_name;
  time_t calendar_time;
  struct tm *local;
  long value;
  int fail, fd, i, path_length, name_length, prefix_length, times, mask;
  int is_in_path = 0;
  char *name = (char*)malloc(sizeof(char) * 102);
  char *mode = (char*)malloc(sizeof(char) * 8);
  char *uid = (char*)malloc(sizeof(char) * 8);
  char *gid = (char*)malloc(sizeof(char) * 8);
  char *size = (char*)malloc(sizeof(char) * 12);
  char *mtime = (char*)malloc(sizeof(char) * 12);
  char *checksum = (char*)malloc(sizeof(char) * 8);
  char *type = (char*)malloc(sizeof(char));
  char *linkname = (char*)malloc(sizeof(char) * 101);
  char *magic = (char*)malloc(sizeof(char) * 6);
  char *version = (char*)malloc(sizeof(char) * 2);
  char *uname= (char*)malloc(sizeof(char) * 32);
  char *gname = (char*)malloc(sizeof(char) * 32);
  char *devmajor = (char*)malloc(sizeof(char) * 8);
  char *devminor = (char*)malloc(sizeof(char) * 8);
  char *prefix = (char*)malloc(sizeof(char) * 156);
  char *excess = (char*)malloc(sizeof(char) * 12);
  char *copied = (char*)malloc(sizeof(char) * 512);
  char *buffer = (char *)malloc(sizeof(char) * 512);
  char *path_buffer = (char *)malloc(sizeof(char) * 512);
  struct utimbuf *ubuf = 
  (struct utimbuf *)malloc(sizeof(struct utimbuf));
  struct stat *l_buffer = (struct stat *)malloc(sizeof(struct stat));
  
  /* Reads Headers */
  read(tar_file, name, 100);
  read(tar_file, mode,  8);
  read(tar_file, uid,  8);
  read(tar_file, gid,  8);
  read(tar_file, size, 12);
  read(tar_file, mtime, 12);
  read(tar_file, checksum, 8);
  read(tar_file, type, 1);
  read(tar_file, linkname, 100);
  read(tar_file, magic, 6);
  read(tar_file, version, 2);
  read(tar_file, uname, 32);
  read(tar_file, gname, 32);
  read(tar_file, devmajor, 8);
  read(tar_file, devminor, 8);
  read(tar_file, prefix, 155);
  read(tar_file, excess, 12);
  free(excess);

  /* Null terminated copied and prefix, if needed */
  name[100] = NULL;
  prefix[155] = NULL;
  

  /* If there's a path, check to see if copied matches */
  if (is_path){

    /* If theres no prefix */
    if (strlen(prefix) == 0){
      for (i = 3; i < num_p; i++){
        if(strlen(name) >= strlen(path[i]) ){
          strncpy(copied, name, strlen(path[i]));
          copied[strlen(path[i])] = NULL;
          if(is_in_path = !strcmp(copied, path[i])){
            break;
          }

          /* Error comes from names that end in a slash */
          


        }
      }
    } else {
      for (i = 3; i < num_p; i++){
        if (strlen(name) + strlen(prefix) >= strlen(path[i])){
          strcpy(copied, prefix);
          copied[strlen(prefix)] = '/';
          strncpy(&copied[strlen(prefix) + 1], name, 
          strlen(path[i]) - (strlen(prefix)+1));
          copied[strlen(path[i])] = NULL;
          if (is_in_path = !strcmp(copied, path[i])){
            break;
          }
        }
      }
    }
  }
  
  /* Runs code if there's no path, or copied matches path */
  if (!is_path || is_in_path){

    /* Differentiates based on if its a file, dir, or symlink */
    if (type[0] == '5'){
      /* If its a directory */

        if (!strlen(prefix)){
          free(copied);
          copied = name;
          copied[strlen(name)] = NULL;
    
        } else{
          strcpy(copied, prefix);
          copied[strlen(prefix)] = '/';
          strcpy(&copied[strlen(prefix) + 1], name); /* THIS LINE */
          copied[strlen(name) + strlen(prefix) + 1] = NULL;

        }
        /* Gives execute permissions for all if any have it */
        value = strtol(mode, &eptr, 10);
        if (value % 2 || (value / 10) % 2 || (value / 100) % 2){
          fail = mkdir(copied, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP 
          | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
        } else {
          fail = mkdir(copied, S_IRUSR | S_IWUSR | S_IRGRP 
          | S_IWGRP | S_IROTH | S_IWOTH);
        }
        /*if (fail < 0){
          printf(stderr, "Mkdir failed.\n");
          exit(EXIT_FAILURE);
        }*/

    } else if (type[0] == '2'){
      /* If its a link */
      linkname[100] = NULL;
      if (strlen(prefix) == 0){
        fail = symlink(linkname, name);

      } else {
        strcpy(copied, prefix);
        copied[strlen(prefix)] = '/';
        strcpy(&copied[strlen(prefix) + 1], name);
        copied[strlen(name) + strlen(prefix) + 1] = NULL;
        fail = symlink(linkname, copied);
      }
      if (fail != 0){
        fprintf(stderr, "Symlink failed.\n");
        exit(EXIT_FAILURE);
      }



    } else {
      /* If its a regular file  */
      if (strlen(prefix) == 0){
        free(copied);
        copied = name;
      } else {
        strcpy(copied, prefix);
        copied[strlen(prefix)] = '/';
        strcpy(&copied[strlen(prefix) + 1], name);
        copied[strlen(name) + strlen(prefix) + 1] = NULL;
      }

      
      /* If any execute bits are set - give everone execute permissions */
      value = strtol(mode, &eptr, 10);
      if (value % 2 || (value / 10) % 2 || (value / 100) % 2){
        fd = open(copied, O_RDWR | O_CREAT | O_TRUNC, 
        S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP 
        | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
      } else {
        fd = open(copied, O_RDWR | O_CREAT | O_TRUNC, 
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
      }

      fail = lstat(copied, l_buffer);
      if (fail < 0){
        printf(stderr, "Lstat failed.\n");
        exit(EXIT_FAILURE);
      }
      value = strtol(size, &eptr, 8);
      while (value > 0){
        fail = read(tar_file, buffer, 512);
        if (fail < 0){
          printf(stderr, "Read failed.\n");
          exit(EXIT_FAILURE);
        }
        if (value >= 512){
          write(fd, buffer, 512);
        } else {
          write(fd, buffer, value);
        }
        value -= fail;
      }
      ubuf->actime = l_buffer->st_atime;
      value = strtol(mtime, &eptr, 8);
      ubuf->modtime = value;
      fail = utime(copied, ubuf);
      if (fail < 0){
        printf(stderr, "Utime failed.\n");
        exit(EXIT_FAILURE);
      }

    }

    /* If verbose is selected, prints out copied */
    if (verbose){ 
      write(STDOUT_FILENO, copied, strlen(copied));
      write(STDOUT_FILENO, "\n", 1);
    }
  } else if (type[0] == 0 || type[0] == '0'){
    /* If its a file and we don't read out the contents */
    value = strtol(size, &eptr, 8);
    value = value / 512 + 1;
    for (i = 0; i < value; i++){
      lseek(tar_file, 512, SEEK_CUR);
    }


  } else if (type[0] == '5'){
    /* If its a directory we need to create for the path */
    strncpy(path_buffer, path, strlen(copied));
    if (!strcmp(path_buffer, copied)){
      if (!strlen(prefix)){
        free(copied);
        copied = name;
        copied[strlen(name)] = NULL;
    
      } else{
        strcpy(copied, prefix);
        copied[strlen(prefix)] = '/';
        strcpy(&copied[strlen(prefix) + 1], name); /* THIS LINE */
        copied[strlen(name) + strlen(prefix) + 1] = NULL;

      }
      /* Gives execute permissions for all if any have it */
      value = strtol(mode, &eptr, 10);
      if (value % 2 || (value / 10) % 2 || (value / 100) % 2){
        fail = mkdir(copied, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP 
        | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
      } else {
        fail = mkdir(copied, S_IRUSR | S_IWUSR | S_IRGRP 
        | S_IWGRP | S_IROTH | S_IWOTH);
      }
      /*if (fail < 0){
        printf(stderr, "Mkdir failed.\n");
        exit(EXIT_FAILURE);
      }*/
    }

  }
  if (strlen(prefix)){
    free(name);
  }
  free(mode);
  free(uid);
  free(gid);
  free(size);
  free(mtime);
  free(checksum);
  free(type);
  free(linkname);
  free(magic);
  free(version);
  free(uname);
  free(gname);
  free(devmajor);
  free(devminor);
  free(prefix);
  free(output);
  free(size_buff);
  free(copied);
  free(buffer);
  free(ubuf);
  free(l_buffer);
  free(path_buffer);
  
  return 0;
}

int ttar(char *arguments[], int argc, int vFlag, int SFlag) {
  /* Declares Variables here */
  /* Check to make sure this is how you can open tar_file */
  int tar_file = open(arguments[2], O_RDONLY | O_CREAT,
   S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);
  int offset, i;
  if (tar_file < 0){
    printf("File unable to be opened.\n");
    exit(EXIT_FAILURE);
  }

  /* Takes given archive file */
  /* Evaluates if there is a path or not */


  /*Recursively run through tree within archive  */
  if (argc <= 3){
    while (NotatEnd(tar_file)){
      offset = lseek(tar_file, -1024, SEEK_CUR);
      if (offset < 0){
        printf(stderr, "Lseek error %d.\n", offset);
      }
      trecurse_through_path(tar_file, vFlag, 0, argc, NULL);
    }
  } else {
  
    while (NotatEnd(tar_file)){
      offset = lseek(tar_file, -1024, SEEK_CUR);
      if (offset < 0){
        printf(stderr, "Lseek error %d.\n", offset);
      }
      trecurse_through_path(tar_file, vFlag, 1, argc, arguments);
    }
    offset = lseek(tar_file, SEEK_SET, 0);
  }

  /* Return without errors */
  return 0;
}

int trecurse_through_path(int tar_file, int verbose, 
int is_path, int argc, char** path){
  /* Declares Variables */
  char *delete_block, *eptr, *writer;
  char *output = (char*)malloc(sizeof(char) * 356);
  char *size_buff = (char*)malloc(sizeof(char) * 8);
  char typ, *beginning_name;
  time_t calendar_time;
  struct tm *local;
  long value;
  int fail, i, path_length, name_length, prefix_length, times, mask;
  int is_in_path = 0;
  char *name = (char*)malloc(sizeof(char) * 101);
  char *mode = (char*)malloc(sizeof(char) * 8);
  char *uid = (char*)malloc(sizeof(char) * 8);
  char *gid = (char*)malloc(sizeof(char) * 8);
  char *size = (char*)malloc(sizeof(char) * 12);
  char *mtime = (char*)malloc(sizeof(char) * 12);
  char *checksum = (char*)malloc(sizeof(char) * 8);
  char *type = (char*)malloc(sizeof(char));
  char *linkname = (char*)malloc(sizeof(char) * 100);
  char *magic = (char*)malloc(sizeof(char) * 6);
  char *version = (char*)malloc(sizeof(char) * 2);
  char *uname= (char*)malloc(sizeof(char) * 32);
  char *gname = (char*)malloc(sizeof(char) * 32);
  char *devmajor = (char*)malloc(sizeof(char) * 8);
  char *devminor = (char*)malloc(sizeof(char) * 8);
  char *prefix = (char*)malloc(sizeof(char) * 156);
  char *excess = (char*)malloc(sizeof(char) * 12);
  char *copied = (char*)malloc(sizeof(char) * 256);

  read(tar_file, name, 100);
  read(tar_file, mode,  8);
  read(tar_file, uid,  8);
  read(tar_file, gid,  8);
  read(tar_file, size, 12);
  read(tar_file, mtime, 12);
  read(tar_file, checksum, 8);
  read(tar_file, type, 1);
  read(tar_file, linkname, 100);
  read(tar_file, magic, 6);
  read(tar_file, version, 2);
  read(tar_file, uname, 32);
  read(tar_file, gname, 32);
  read(tar_file, devmajor, 8);
  read(tar_file, devminor, 8);
  read(tar_file, prefix, 155);
  read(tar_file, excess, 12);
  free(excess);

  magic[5] = NULL;
  if (strcmp(magic, "ustar")){
    perror("Bad tar file, listing.\n");
    exit(EXIT_FAILURE);
  }

  name[100] = NULL;
  prefix[155] = NULL;

  if (is_path){
    for (i = 3; i < argc; i++){
      path_length = strlen(path[i]);
      name_length = strlen(name);
      /* Come back to this */
      prefix_length = strlen(prefix);
      if (prefix_length + name_length >= path_length){

        if (prefix_length != 0){
          strcpy(copied, prefix);
          strncpy(&copied[strlen(prefix)], name, path_length - prefix_length);
        } else {
          strncpy(copied, name, path_length);
        }
        copied[path_length] = NULL;

        if (strcmp(copied, path[i]) == 0){
          is_in_path = 1;
        }
        
      }
    }

  }
  free(copied);

  /* If it doesn't have path requirements, or meets those requirements */
  if (!is_path || is_in_path){

    /* If varbose options is set */
    if (verbose){
      /* PERMISSIONS */
      /* Get type for permissions */
      typ = type[0];
      if (typ == '2'){
        /* Symbolic Link type */
        write(STDOUT_FILENO, "l", 1);

      } else if (typ == '5'){
        /* Directory type */
        write(STDOUT_FILENO, "d", 1);

      } else {
        /* Other file type */
        write(STDOUT_FILENO, "-", 1);
      }

      /* Convert octal to permission */
      /* I'm very unsure about the type format of this */
      for (i = 4; i < 7; i++){
        mask = mode[i] - 48;

        if (mask >= 4){
          write(STDOUT_FILENO, "r", 1);
          mask -= 4;
        } else {
          write(STDOUT_FILENO, "-", 1);
        }

        if (mask >= 2){
          write(STDOUT_FILENO, "w", 1);
          mask -= 2;
        } else {
          write(STDOUT_FILENO, "-", 1);
        }

        if (mask >= 1){
          write(STDOUT_FILENO, "x", 1);
        } else {
          write(STDOUT_FILENO, "-", 1);
        }
      }
      write(STDOUT_FILENO, " ", 1);

      /* OWNER / GROUP NAMES */
      i = snprintf(output, 17, "%s/%s", uname, gname);
      output[17] = NULL;
      i = strlen(output);
      fail = write(STDOUT_FILENO, output, i);
      for(; i < 18; i++){
        write(STDOUT_FILENO, " ", 1);
      }
      if (fail < 0){
        printf(stderr, "Write failed.\n");
        exit(EXIT_FAILURE);
      }

    /* SIZE */
    /* Convert from octal to int and then
    prints them out in 8 characters */
    value = strtol(size, &eptr, 8);
    snprintf(size, 9, "%8d", value);
    write(STDOUT_FILENO, size, 8);
    write(STDOUT_FILENO, " ", 1);


    /* M TIME */
    value = strtol(mtime, &eptr, 8);
    calendar_time = value;
    local = localtime(&calendar_time);
    strftime(output, 18, "%Y-%m-%d %H:%M ", local);
    write(STDOUT_FILENO, output, 17);


    } else {

      value = strtol(size, &eptr, 8);
      snprintf(size, 9, "%8d", value);
    }

    /* Name */
    if (strlen(prefix) == 0){
      i = snprintf(output, 101, "%s", name);
      output[101] = NULL;
      write(STDOUT_FILENO, output, strlen(output));
    } else {
      i = snprintf(output, 156, "%s", prefix);
      output[156] = NULL;
      write(STDOUT_FILENO, output, strlen(output));
      write(STDOUT_FILENO, "/", 1);
      i = snprintf(output, 101, "%s", name);
      output[101] = NULL;
      write(STDOUT_FILENO, output, strlen(output));
    }
    write(STDOUT_FILENO, "\n", 1);

    
  } else {
    value = strtol(size, &eptr, 8);
    snprintf(size, 9, "%8d", value);
  }
  /* SIZE */
  /* Convert from octal to int and then
   prints them out in 8 characters */
  value = strtol(size, &eptr, 10);

  /* Read out contents into nowhere */
  value /= 512;
  if (type[0] != '2' && type[0] != '5'){
    value += 1;
  }
  lseek(tar_file, 512 * value, SEEK_CUR);

  free(name);
  free(mode);
  free(uid);
  free(gid);
  free(size);
  free(mtime);
  free(checksum);
  free(type);
  free(linkname);
  free(magic);
  free(version);
  free(uname);
  free(gname);
  free(devmajor);
  free(devminor);
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
 

