/* Mytar Implementation by Mira Shlimenzon 
   and Emma Kester.
   A program to mimic the tar UNIX function
   by bundling files or  directories into a
   single file.
 */

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
  if(f== 0) {
    printf ("Missing -f\n"); 
    printf("Usage: mytar [ctxvS]f tarfile\n");
    exit(-1);
  }
  if((f - 1 > 0) || (v - 1 > 0) || (S- 1 > 0)) {
    printf ("Only choose one for -v, -S, and -f\n"); 
    printf("Usage: mytar [ctxvS]f tarfile\n");
  }
  
  if(c) {
    ctar(argv[], vFlag, SFlag);

  }

  if(x){}

  if(t){}
  
return 0;
}

/* Creates tar file with given path list */
int ctar(char *argv[], int vFlag, int SFlag) {
  int output; 
  int i;

  /* create the tar file */
  if((output = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
    perror("open");
    exit(-1);
  }
  /* calls for each of the paths readCPath() */
  for(i = 3; i < argc; i++){
    readCPath(argv[i], output, vFlag, sFlag);
  }
  
  // create the tar file 
  //check for v and S
  //calls for each of the paths readPath(path);
  //then writes the 2 null blocks at the end
}

/* Reads all directories and files in path and calls other function createHeader for it */
readCPath(char *path, int output, int vFlag, int SFlag){
  /* recursively goes through directories/files and calls createHeader() on everything */
  /* print out if v when you call createHeader() */
  createHeader(); 
} 

/* Using given path, takes all data and puts it into a struct */
/* struct that's a header- where we fill in the correct information */
/* at end of header format it and place it into the tar file */
createHeader() {
  /* if S then do weird stuff with magic number + version */
}

int xtar(..., char *path_list, int vFlag, int SFlag) {
  //given tar file 
  //check if given multiple paths or none!! 
  // given path - read header
  /* how to find secific targets */
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

int ttar(..., int vFlag, int SFlag) {
  //
}
