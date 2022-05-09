/* Mytar Implementation by Mira Shlimenzon 
   and Emma Kester.
   A program to mimic the tar UNIX function
   by bundling files or  directories into a
   single file.
 */

int main(int argc, char *argv[]) {
  char main_flag = 0;
  int vflag = 0, fflag = 0, Sflag = 0;
  int i = 0;
  if(argc < 3)  {
    fprintf(stderr, "too little arguments"); 
    fprintf(stderr, "usage: mytar [ctxvS]f tarfile");
    exit(-1);
  }
  for(; i < strlen(argv[1]); i++) {
    if(argv[1][i] == 'c') {
      main_flag += 'c';
    }
    else if(argv[1][i] == 't') {
      main_flag += 't';
    }
    else if(argv[1][i] == 'x') {
      main_flag += 'x';
    }
    else if(argv[1][i] == 'v') {
      vflag ++;
    }
    else if(argv[1][i] == 'S') {
      Sflag ++;
    }
    else if(argv[1][i] == 'f') {
      fflag ++;
    }
    else {
      fprintf(stderr, "letter not from list"); 
      fprintf(stderr, "usage: mytar [ctxvS]f tarfile");
      exit(-1);
    }
  }
  /* main flag empty */
  if(!main_flag) {
   fprintf(stderr, "need one of the options from ctv"); 
   fprintf(stderr, "usage: mytar [ctxvS]f tarfile");
   exit(-1);
  }
  /* if more than one letter selected */
  if(main_flag > 121) {
    fprintf(stderr, "only choose one from ctv"); 
    fprintf(stderr, "usage: mytar [ctxvS]f tarfile");
    exit(-1);
  }
  /* missing f flag */
  if(fflag == 0) {
     fprintf(stderr, "f flag is missing"); 
     fprintf(stderr, "usage: mytar [ctxvS]f tarfile");
     exit(-1);
  }
  if((fflag - 1 > 0) || (vflag - 1 > 0) || (Sflag - 1 > 0)) {
    fprintf(stderr, "only output vSf once"); 
    fprintf(stderr, "usage: mytar [ctxvS]f tarfile");
  }
  
  
  

return 0;
}

int xtar(..., int vFlag, int SFlag) {}
int ctar(... ,int vFlag, int SFlag) {}
int ttar(..., int vFlag, int SFlag) {}


