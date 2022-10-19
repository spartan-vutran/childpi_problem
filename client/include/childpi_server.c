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
  cJSON * temp_cJ = NULL;

  if(json_parser == NULL){
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL){
        fprintf(stderr, "Error before: %s\n", error_ptr);
    }
    goto end;
  }

  // Field "status"
  temp_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "status");
  if(!cJSON_IsNumber(temp_cJ)){
    fprintf(stderr, "[SignIn] The status field is not a number\n");
    goto end;
  }
  *status = temp_cJ->valueint;

  // Field "sum"
  temp_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "sum");
  if(!cJSON_IsNumber(temp_cJ)){
    fprintf(stderr, "[SignIn] The sum field is not a number\n");
    goto end;
  }
  *sum = temp_cJ->valueint;

  // Field "sum"
  temp_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "user_turn");
  if(!cJSON_IsNumber(temp_cJ)){
    fprintf(stderr, "[SignIn] The user_turn field is not a number\n");
    goto end;
  }
  *user_turn = temp_cJ->valueint;

  // Field "sum"
  temp_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "max_turn");
  if(!cJSON_IsNumber(temp_cJ)){
    fprintf(stderr, "[SignIn] The status field is not a number\n");
    goto end;
  }
  *max_turn = temp_cJ->valueint;
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



/*
{
  status: -1 | 0 | 1;
}*/
static int check_srvstate_jparser(const char* json,int* status){
  int jstatus = -1;
  cJSON * json_parser = cJSON_Parse(json);
  cJSON * temp_cJ = NULL;

  if(json_parser == NULL){
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL){
        fprintf(stderr, "Error before: %s\n", error_ptr);
    }
    goto end;
  }

  // Field "status"
  temp_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "status");
  if(!cJSON_IsNumber(temp_cJ)){
    fprintf(stderr, "[Check_srvstate] The status field is not a number\n");
    goto end;
  }
  *status = temp_cJ->valueint;
  jstatus = 1;

  end:
  cJSON_Delete(json_parser);
  return jstatus;
}

   
/*
{
  status: -1 | 0 | 1;
}*/
int x = 10;
int check_srvstate(const int isending){
 
  // MOCK: Mock send and check ready to server
  // char buf[MAX_CHAR];
  // snprintf(buf, MAX_CHAR,"{\"status\": %d}", 1);

  // int status = 0, flag =0;
  // flag = check_srvstate_jparser(buf, &status);
  // if(flag == -1){
  //   fprintf(stderr,"Error while parsing check ready packet\n");
  //   return -1;
  // }

  // return status;
  
  if(!isending){
    return 1;
  }
  
  if(x--==0){
    return -1;
  }

  return 0;
}


/*
  {
    status: -1 | 1;
    pis: [1,2,3,4,5,6];
    length: 6;
    user_sum: 150;
    user_turn: 3
  }
*/
static int get_pi_jparser(const char* json,int * status ,int* pis, int* length, int* user_sum,int* user_turn){
  int jstatus = -1;
  cJSON * json_parser = cJSON_Parse(json);
  cJSON * temp_cJ = NULL;

  if(json_parser == NULL){
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL){
        fprintf(stderr, "Error before: %s\n", error_ptr);
    }
    goto end;
  }

  // Field "status"
  temp_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "status");
  if(!cJSON_IsNumber(temp_cJ)){
    fprintf(stderr, "[GetPi] The status field is not a number\n");
    goto end;
  }
  *status = temp_cJ->valueint;

  // Field "pis"
  temp_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "pis");
  cJSON * element_cJ = NULL;
  *length = 0;
  cJSON_ArrayForEach(element_cJ, temp_cJ){
    if(!cJSON_IsNumber(element_cJ)){
      fprintf(stderr, "[GetPi] The element of pis field is not a number\n");
      goto end;
    }
  
    pis[*length] = element_cJ->valueint;
    (*length)++;
  }
  

  // Field "user_sum"
  temp_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "user_sum");
  if(!cJSON_IsNumber(temp_cJ)){
    fprintf(stderr, "[IsReady] The user_sum field is not a number\n");
    goto end;
  }
  *user_sum = temp_cJ->valueint;

  // Field "user_sum"
  temp_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "user_turn");
  if(!cJSON_IsNumber(temp_cJ)){
    fprintf(stderr, "[IsReady] The user_turn field is not a number\n");
    goto end;
  }
  *user_turn = temp_cJ->valueint;
  jstatus = 1;

  end:
  cJSON_Delete(json_parser);
  return jstatus;
}


/*
  {
    status: -1 | 1;
    pis: [1,2,3,4,5,6];
    length: 6;
    user_sum: 150;
    user_turn: 3
  }
*/
int get_pi(const char * user_name, int* pis, int* length, int* user_sum, int* user_turn){
  
  char buf[MAX_CHAR];

  // MOCK: Mock the result got from the server
  snprintf(buf,MAX_CHAR,"{\"status\": 1,\"pis\": [1,2,3,4,5,6],\"length\": 6,\"user_sum\": 250,\"user_turn\": 5}"); 

  int status = 0;
  int flag = get_pi_jparser(buf, &status,&pis[0],length, user_sum,user_turn);
  

  if(flag == -1){
    fprintf(stderr,"Error happended while parsing get_pi json\n");
    return -1;
  }

  return status;
}






/* Response from the server
{
  status:
  name: --if status ok
  sum: -- if status ok
}
*/
static int get_winner_jparser(const char* json, int* status, char*name, const int max_length, int* sum){
  int jstatus = -1;
  cJSON * json_parser = cJSON_Parse(json);
  cJSON * temp_cJ = NULL;

  if(json_parser == NULL){
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL){
        fprintf(stderr, "Error before: %s\n", error_ptr);
    }
    goto end;
  }

  // Field "status"
  temp_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "status");
  if(!cJSON_IsNumber(temp_cJ)){
    fprintf(stderr, "[GetWinner] The status field is not a number\n");
    goto end;
  }
  *status = temp_cJ->valueint;

  
  if (*status == 1){
    // Field "name"
    temp_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "name");
    if (!cJSON_IsString(temp_cJ) || (temp_cJ->valuestring == NULL)){
      fprintf(stderr, "[GetWinner] The name field is not a string\n");
      goto end;
    }
    strncpy(name, temp_cJ->valuestring,max_length);


    // Field "sum"
    temp_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "sum");
    if(!cJSON_IsNumber(temp_cJ)){
      fprintf(stderr, "[GetWinner] The sum field is not a number\n");
      goto end;
    }
    *sum = temp_cJ->valueint;
  }
  
  jstatus = 1;

  end:
  cJSON_Delete(json_parser);
  return jstatus;
}


/* Response from the server
{
  status: 1 | -1
  name: --if status ok
  sum: -- if status ok
}
*/
int get_winner(char* winner, const int max_length, int* sum){
  //MOCK: Sending to the server later
  char buf[MAX_CHAR];
  snprintf(buf,MAX_CHAR,"{\"status\": %d,\"name\": \"%s\", \"sum\": %d}", 1,"Assasin123",350);
  
  
  //Parsing and get the result
  int status = 0;
  int flag = get_winner_jparser(buf, &status, winner, max_length, sum );

  if(flag == -1){
    fprintf(stderr,"Error happended while parsing get winner json\n");
    return -1;
  }
  
  return status;
}



