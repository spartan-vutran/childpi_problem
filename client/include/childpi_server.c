#include <arpa/inet.h>
#include "inet_sockets.h"
#include <unistd.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "childpi_server.h"
#include "cJSON.h"
#include <time.h>

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
int sign_in_jparser(const char* json,int* status ,int * sum, int*user_turn, int *max_turn){
  int jstatus = -1;
  cJSON * json_parser = cJSON_Parse(json);
  cJSON * status_cJ = NULL;
  cJSON * sum_cJ = NULL;
  cJSON * user_turn_cJ = NULL;
  cJSON * max_turn_cJ = NULL;

  if(json_parser == NULL){
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL){
        fprintf(stderr, "Error before: %s\n", error_ptr);
    }
    goto end;
  }

  // Field "status"
  status_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "status");
  if(!cJSON_IsNumber(status_cJ)){
    fprintf(stderr, "[SignIn] The status field is not a number\n");
    goto end;
  }
  *status = status_cJ->valueint;

  // Field "sum"
  sum_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "sum");
  if(!cJSON_IsNumber(sum_cJ)){
    fprintf(stderr, "[SignIn] The sum field is not a number\n");
    goto end;
  }
  *sum = sum_cJ->valueint;

  // Field "sum"
  user_turn_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "user_turn");
  if(!cJSON_IsNumber(user_turn_cJ)){
    fprintf(stderr, "[SignIn] The user_turn field is not a number\n");
    goto end;
  }
  *user_turn = user_turn_cJ->valueint;

  // Field "sum"
  max_turn_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "max_turn");
  if(!cJSON_IsNumber(max_turn_cJ)){
    fprintf(stderr, "[SignIn] The status field is not a number\n");
    goto end;
  }
  *max_turn = max_turn_cJ->valueint;
  jstatus = 1;

  end:
  cJSON_Delete(json_parser);
  return jstatus;
}


// Status:
// -1: error
//  0: The new user is created
//  1: Sign in as existed user
int sign_in(const char* user_name,int* max_turn, int *user_sum, int * user_turn){
  // // Open connection
  // int fd = inetConnect(server_stub->server_ip,server_stub->service,SOCK_STREAM);
  // if(fd == -1){
  //   printf("Error when open tcp socket\n");
  //   return -1;
  // }

  // // Compose and send
  char buf[MAX_CHAR+10];
  // snprintf(buf, MAX_CHAR,"{command:%d;name:\"%s\";}",CHECK_ACCOUNT,user_name);
  // int length = strlen(buf);
  // int numrw = send(fd, buf,strlen(buf),0);
  // if(numrw != length){
  //   printf("Error while sending to server: %s\n", buf);
  //   return -1;
  // }

  
  //TODO: SEND AND RECEIVE PACKET FROM THE SERVER
  //Parsing the packet
  // numrw = read(fd,buf,MAX_CHAR);
  // if(numrw == -1 || numrw == 0 ){
  //   printf("Error while reading from server: %s\n", buf);
  //   return -1;
  // }

  //FIXME: MOCKING BUF
  snprintf(buf, MAX_CHAR,"{\"status\": %d,\"sum\": %d,\"user_turn\": %d,\"max_turn\":%d}",1, 10, 2, 5);

  
  // Data preparation
  int status = 0;
  int flag = 0;

  //Parsing the json
  // buf[numrw] == '\0';
  flag = sign_in_jparser(buf, &status, user_sum, user_turn, max_turn);
  if(flag == -1){
    fprintf(stderr,"Parsing sign_in response fail\n");
    return -1;
  }

  return status;
}



int check_isready_jparser(const char* json,int* status){
  int jstatus = -1;
  cJSON * json_parser = cJSON_Parse(json);
  cJSON * status_cJ = NULL;

  if(json_parser == NULL){
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL){
        fprintf(stderr, "Error before: %s\n", error_ptr);
    }
    goto end;
  }

  // Field "status"
  status_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "status");
  if(!cJSON_IsNumber(status_cJ)){
    fprintf(stderr, "[IsReady] The status field is not a number\n");
    goto end;
  }
  *status = status_cJ->valueint;
  jstatus = 1;

  end:
  cJSON_Delete(json_parser);
  return jstatus;
}

   
// Status:
// -1: error
//  0: Waitting
//  1: Playing
int check_isready(){
  //TODO: SEND AND RECEIVE PACKET FROM THE SERVER 
 
  // FIXME: Mock buffer
  char buf[MAX_CHAR];
  snprintf(buf, MAX_CHAR,"{\"status\": %d}", 1);

  int status = 0, flag =0;
 
  flag = check_isready_jparser(buf, &status);
  if(flag == -1){
    fprintf(stderr,"Error while parsing check ready packet\n");
    return -1;
  }

  return status;
}

int get_pi(const char * user_name, int* pis, int* length, int* user_sum, int* user_turn){
  *length = 10;
  int max_turn = 5;
  
  for(int i=0;i < *length;i++){
    pis[i] = i;
    *user_sum+= i;
  }

  int turn = *user_turn;
  
  *user_turn= (turn>=max_turn)?turn:++turn; 
  return 1;
}





// Status:
// -1: error
//  0: Game is still not ended
//  1: OK

/* Reponse from the server
{
  status:
  name: --if status ok
  sum: -- if status ok
}
*/

int x = 1;
int get_winner(char* winner,int* sum){
  //TODO: Send to server and check if there is report
  if(x-- == 1)
    return 0;
  
  char name[MAX_CHAR];
  snprintf(name,MAX_CHAR,"assasin123");
  *sum = 350;
  return 1;
}
