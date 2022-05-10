/* Mytar Implementation by Mira Shlimenzon 
   and Emma Kester.
   A program to mimic the tar UNIX function
   by bundling files or  directories into a
   single file.
 */

int main(int argc, char *argv[]) {
  int cflag = 0, tflag = 0, xflag = 0;
  int vflag = 0, fflag = 0, Sflag = 0;
  int i = 0;
  char *path_list[10]; /* list to hold paths */

  if(argc < 3)  {
    printf ("Missing argument/s\n"); 
    printf("Usage: mytar [ctxvS]f tarfile\n");
    exit(-1);
  }
  for(; i < strlen(argv[1]); i++) {
    if(argv[1][i] == 'c') {
      cflag++;
    }
    else if(argv[1][i] == 't') {
      tflag++;
    }
    else if(argv[1][i] == 'x') {
      xflag++;
    }
    else if(argv[1][i] == 'v') {
      vflag++;
    }
    else if(argv[1][i] == 'S') {
      Sflag++;
    }
    else if(argv[1][i] == 'f') {
      fflag++;
    }
    else {
      
      printf("Usage: mytar [ctxvS]f tarfile\n");
      exit(-1);
    }
  }
  /* main flag empty */
  if((cflag + tflag + xflag) == 0) {
    printf ("Need to specify one of -c, -t, or -x\n"); 
    printf("Usage: mytar [ctxvS]f tarfile\n");
   exit(-1);
  }
  /* if more than one letter selected */
  if((cflag + tflag + xflag) > 1){ 
    printf ("Only choose one of -c, -t, or -x\n"); 
    printf("Usage: mytar [ctxvS]f tarfile\n");
    exit(-1);
  }
  /* missing f flag */
  if(fflag == 0) {
     printf ("Missing -f\n"); 
    printf("Usage: mytar [ctxvS]f tarfile\n");
     exit(-1);
  }
  if((fflag - 1 > 0) || (vflag - 1 > 0) || (Sflag - 1 > 0)) {
    printf ("Only choose one for -v, -S, and -f\n"); 
    printf("Usage: mytar [ctxvS]f tarfile\n");
  }
  
  if(cflag) {}

  if(xflag){}

  if(tflag){}
  
return 0;
}

/* Creates tar file with given path list */
int ctar(... ,char *path_list, int vFlag, int SFlag) {
  // create the tar file 
  //check for v and S
  //calls for each of the paths readPath(path);
  //then writes the 2 null blocks at the end
}

/* Reads all directories and files in path and calls other function createHeader for it */
readCPath(){
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
