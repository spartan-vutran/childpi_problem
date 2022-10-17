#include "./include/inet_sockets.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>


#define BUF_SIZE 255
#define SERVICE "50003"

//
/*
{
  state: WAITING;
}

{
  state: READY;
  maxpi: 100;
  cntdown: 10;
}

{
  state: PLAYING;
  turn:
  result: NULL || 4,1,3,2,1 ;
}

{
  state: ENDING;
  cntdown: 10;
}
*/
//

int main(int argc, char *argv[]){
  int fd=0, numRead=0;
  char readBuf[BUF_SIZE], writeBuf[BUF_SIZE];


  if(argc <2){
    printf("Invalid parameter\n");
    return -1;
  }

  // Connect to server
  fd = inetConnect(argv[1],SERVICE, SOCK_STREAM);
  if(fd == -1){
    printf("Can not connect to server\n");
    exit(-1);
  }

  while(1){
    numRead = read(fd,readBuf,BUF_SIZE);
    
    if(numRead == -1){
      printf("Error happen during reading\n");
      exit(-1);
    }

    if(numRead == 0){
      break;
    }

    readBuf[numRead]='\0';
    printf("[%d readed]: %s\n", numRead,readBuf);
  }
}