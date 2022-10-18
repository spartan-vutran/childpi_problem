#include <arpa/inet.h>
// #include <inet_sockets.h>
#include <unistd.h>

#include <string.h>
#include <stdlib.h>
#include "childpi_server.h"

void init_server_stub(const char*ip, const char*service){
  server_stub = malloc(sizeof(struct childpi_server));
  strcpy(server_stub->server_ip,ip);
  strcpy(server_stub->service,service);
}

void destroy_server_stub(){
  free(server_stub);
}





/* Sample:
  Sender:
    {
      request: ; 
      arg...
    }
  
  Receiver:
    { 
      response: ;
      result...
    }
*/


//   Response:
//   { 
//     status: ;
//     sum: ;  //If is old user
//     user_turn: ; 
//     max_turn: 
//   }
// Status:
// -1: error
//  0: The new user is created
//  1: Sign in as existed user
int sign_in(const char* user_name,int* max_turn, int *user_sum, int * user_turn){
  // Open connection
  int fd = inetConnect(server_stub->server_ip,server_stub->service,SOCK_STREAM);
  if(fd == -1){
    printf("Error when open tcp socket\n");
    return -1;
  }

  // Compose and send
  char buf[MAX_CHAR];
  snprintf(buf, MAX_CHAR,"{command:%d;name:\"%s\";}",CHECK_ACCOUNT,user_name);
  int length = strlen(buf);
  int numrw = send(fd, buf,strlen(buf),0);
  if(numrw != length){
    printf("Error while sending to server: %s\n", buf);
    return -1;
  }

  
  //TODO: SEND AND RECEIVE PACKET FROM THE SERVER
  //Parsing the packet
  // numrw = read(fd,buf,MAX_CHAR);
  // if(numrw == -1 || numrw == 0 ){
  //   printf("Error while reading from server: %s\n", buf);
  //   return -1;
  // }

  //FIXME: MOCKING BUF
  snprintf(buf, MAX_CHAR,"\n{\nstatus: %d;\nsum: %d;\nuser_turn: %d;\nmax_turn:%d\n}", 1, 10, 2, 5);
  
  
  //Parsing the json
  buf[numrw] == '\0';
  

  // MOCKING
  // *user_sum = 10;
  // *max_turn = 5;
  // *user_turn = 1;
  // return 1;
}

// int x =1;
// int check_isready(){
//   if (x==1){
//     x--;
//     return 0;
//   }

//   return 1;
// }

// int get_pi(const char * user_name, int* pis, int* length, int* user_sum, int* user_turn){
//   *length = 10;
//   int max_turn = 5;
  
//   for(int i=0;i < *length;i++){
//     pis[i] = i;
//     *user_sum+= i;
//   }

//   int turn = *user_turn;
//   *user_turn= (turn>=max_turn)?turn:turn++; 
//   return 1;
// }