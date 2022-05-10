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
  char *path_list[10]; /* list to hold paths */

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
    //check here that we have an argv[3] or else say we need a path 
    ctar(argv[], v, S);

  }
  else if(x || t){
    printf("x or t");
  }
  
return 0;
}

/* Creates tar file with given path list */
void ctar(char *argv[], int v, int S) {
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
    readCPath(argv[i], output, v, s);
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
void readCPath(char *path, int output, int v, int S){
  DIR *d;
  struct stat lst_b;
  struct stat st_b;
  struct dirent *entry;
  char *pass_path;
  
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
    createHeader(&lst_b, path, output, v, S);
  }
  /* Regular file sym link condition */
  else if(S_ISLNK(lst_b.st_mode) && S_ISREG(st_b.st_mode)){
    createHeader(&lst_b, path, output, v, S); 
    createHeader(&st_b, path, output, v, S); 
  }
  /* Directory condition */
  else if(S_ISDIR(st_b.st_mode)){
    /* Sym Link Directory */
    if(S_ISLNK(lst_b.st_mode)){
      createHeader(&lst_b, path, output, v, S); 
    }
    createHeader(&st_b, path, output, v, S);

    /* now looping through directory to call readCPath on it and all its files */
    while((entry = readdir(d)) != NULL) {
      pass_path = path;
      strcat(pass_path, "/");
      readCPath(strcat(pass_path, entry->d_name), output, v, S);
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
void createHeader(struct stat *sb, char *path, int output, int v, int S) {
  printf("%s\n", path);
  if(v) {
   // printout what we are doing rn
  }
  /* print out if v when you call createHeader() */
  /* if S then do weird stuff with magic number + version */
}


/*
int xttar(..., char *path_list, int v, int S) {
  //given tar file 
  //check if given multiple paths or none!! 
  // given path - read header
  // how to find secific targets 
}


readXPath() {
  //read the path
  //recusively traversing (reading through the tree)
  //print out the path if v before I call extract header
  //if directory then if it got deleted replicate but if not then 
  // just access it and add files

  //follow path as much as we can and if we can't find then create
  extractHeader();
}

extractHeader() {
  //fill struct with header 
  //if file exists already, truncate and write with new header 
  //return that struct
}

int ttar(..., int v, int S) {
  //
} */
