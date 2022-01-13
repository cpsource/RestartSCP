#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "restart.h"
//#define TARG "offcorp@offcorp1:~/tmp1/."
// number of .sh files we copy down
#define I_CNT 4

int i_flag = 0;

void gen_out(FILE *outf,int cnt, char *wbuf, char *targ ) {
  fprintf(outf,"# Remaining %d\n",cnt);
  fprintf(outf,"echo \"Transfers Remaining %d\"\n",cnt);    
  fprintf(outf,"echo \"%s\" > restart.checkpoint\n",wbuf);
  fprintf(outf,"scp %s %s\n",wbuf,TARG);
  fprintf(outf,"while [ $? -ne 0 ]; do\n");
  fprintf(outf,"  echo \"...retry\"\n");
  fprintf(outf,"  sleep 15;\n");
  fprintf(outf,"  scp %s %s\n",wbuf,TARG);
  fprintf(outf,"done\n");
}

int main(int argc, char *argv[] ) {
  FILE *inf;
  FILE *outf;
  char wbuf[1024];
  char w1buf[4096];
  char *c;
  int found_flag = 0;
  int cnt = I_CNT;
  char restart_checkpoint[64];
  int copies_remaining;

  if ( argc > 1 && argv[1][0]=='-' && argv[1][1] == 'i' ) {
    i_flag = 1;
  }

  if ( ! i_flag ) {
    inf = fopen("restart.checkpoint","r");
    if ( !inf ) {
      if ( argc < 2 ) {
	printf("./restart <restart-file>\n");
	exit(0);
      }
    } else {
      fgets(restart_checkpoint,sizeof(restart_checkpoint),inf);
      c = strchr(restart_checkpoint,'\n'); if ( c ) *c = 0;
      fclose(inf);
    }
  } // if i_flag
  
  inf  = fopen("files.restart","r");
  outf = fopen("cp-restart.sh","w");

  fprintf(outf,"#!/bin/bash\n");
  fprintf(outf,"#\n");

  // get count remaining
  while ( fgets(wbuf,1024,inf) ) {
    c = strchr(wbuf,'\n'); if ( c ) *c = 0;
    if ( !i_flag ) {
      // find our place
      if ( !found_flag && 0 != strcmp(restart_checkpoint,wbuf) ) {
	// not found
	continue;
      } else {
	// found
	found_flag = 1;
      }
    }
    cnt += 1;
  } // while read
  fclose(inf);
  fclose(outf);
  
  outf = fopen("files.restart","a");
  // and these files too  
  fprintf(outf,"tst.sh\n");
  fprintf(outf,"sum.sh\n");
  fprintf(outf,"all-present.sh\n");
  fprintf(outf,"rebuild.sh\n");
  fclose(outf);
  
  inf = fopen("files.restart","r");
  outf = fopen("cp-restart.sh","a");
  
  copies_remaining = cnt;
  found_flag = 0;

  // now generate code
  while ( fgets(wbuf,1024,inf) ) {
    c = strchr(wbuf,'\n'); if ( c ) *c = 0;

    if ( !i_flag ) {
      // find our place
      if ( !found_flag && 0 != strcmp(restart_checkpoint,wbuf) ) {
	// not found
	continue;
      } else {
	// found
	found_flag = 1;
      }
    }
    
    // found here - main copy instructions per segment
    gen_out(outf,cnt,wbuf,TARG);

    cnt -= 1;
    
  } // while read

  fclose(inf);
  fclose(outf);
  
  system("chmod +x cp-restart.sh");
  
  printf("copies remaining = %d\n",copies_remaining);
  printf("To restart, do: ./cp-restart.sh\n");

  if ( !i_flag ) {
    exit(0);
  }
  
  // build all-present.sh
  outf = fopen("all-present.sh","w");
  fprintf(outf,"#!/usr/bin/bash\n");

  // now generate code
  inf = fopen("files.restart","r");
  while ( fgets(wbuf,sizeof(wbuf),inf) ) {
    c = strchr(wbuf,'\n'); if ( c ) *c = 0;
    
    // found here
    sprintf(w1buf,"if [ ! -f %s ]; then echo \"%s not found\"; exit 1; fi",wbuf,wbuf);
    fprintf(outf,"%s\n",w1buf);
    
  } // while read
  fprintf(outf,"echo \"All files present\"\n");
  fprintf(outf,"exit 0\n");
  fclose(outf);
  fclose(inf);
  system ( "chmod +x all-present.sh" );

  // rebuild.sh
  char *sha;
  char *fil;
  
  inf = fopen("source.sha256sum","r");
  fgets(wbuf,sizeof(wbuf),inf);
  fclose(inf);
  c = strchr(wbuf,'\n'); if ( c ) *c = 0;
  c = strchr(wbuf,' '); if ( c ) *c = 0;
  c += 1;
  while ( c && *c == ' ' ) c += 1;
  sha = strdup(wbuf);
  fil = strdup(c);

  // allow our file to be reconstructed at far end
  
  outf = fopen("rebuild.sh","w");
  fprintf(outf,"#!/usr/bin/bash\n");
  fprintf(outf,"./all-present.sh\n");
  fprintf(outf,"if [ $? -ne 0 ]; then\n");
  fprintf(outf,"  echo \"Can't rebuild because not all files are present\"\n");
  fprintf(outf,"  exit 1\n");
  fprintf(outf,"fi\n");
  fprintf(outf,"rm %s\n",fil);
  fprintf(outf,"for i in x*\n");
  fprintf(outf,"do\n");
  fprintf(outf,"  cat $i >> %s\n",fil);
  fprintf(outf,"done\n");
  fprintf(outf,"./sum.sh %s %s\n",sha,fil);
  fprintf(outf,"if [ $? -ne 0 ]; then\n");
  fprintf(outf,"  echo \"sha256 sum of %s incorrect\"\n",fil);
  fprintf(outf,"  exit 1\n");
  fprintf(outf,"fi\n");
  fprintf(outf,"# done\n");
  fprintf(outf,"exit 0\n");
  fclose(outf);

  system("chmod +x rebuild.sh");

  return 0;
}
