
// Network library
#include <pthread.h>
#include "./include/inet_sockets.h"

// Thread pool library
#include "./include/thpool.h"

// Child datastructure
#include "./include/child_ds.h"

// Others
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./include/cJSON.h"

// ERROR library
#include <errno.h>



#define MAX_CHAR 255
#define SERVICE "50003"
// #define MAX_TURN 5

// Store definition
#define PI_NUM 1000
#define RANGE_VALUE 10
#define TURN 5
#define BAG_SIZE 50
#define CHILD_NUM 5
#define SECTION_NUM 10

// COMMAND DEFINITION
#define SIGN_IN 1
#define CHECK_SRVSTATE 2
#define GET_PI 3 
#define GET_WINNER 4


// =============================Data structure=================================
struct pistore{
  int * pis;    // pis of store
  uint32_t num_pi;  // number of pi
  pthread_mutex_t* mutex; // mutex for each partition
  int section_num; //number of displayed section in the store
  int turn;   // number of times child could come to the store 
  int bag_size; //the bag that the store gives for children
  char * report;
  int max_child;
};

typedef struct pistore pistore_t;
// Data declaration
pistore_t store;
pthread_mutex_t start_lock;
// pthread_cond_t full, is_end;
// struct child* childs;
// int is_full_slot = 0, child_count=0;



void store_init(int num_pi, int range,int section_num, int turn, int bag_size, int max_child){
  //Init game
  store.bag_size=bag_size;
  store.turn = turn;
  store.section_num=section_num;
  
  // Mutex init
  pthread_mutex_init(&start_lock, NULL);
  // pthread_cond_init(&full, NULL);
  // pthread_cond_init(&is_end, NULL);
  store.mutex = malloc(sizeof(pthread_mutex_t)*store.section_num);

  for(int i=0; i<store.section_num; i++){
    pthread_mutex_init(&(store.mutex[i]), NULL);
  }
}


void print_child(const child_t* achild){
  printf("-------Child %s--------\n", achild->name);
  printf("Gained pis: ");
  printf("%d\n", achild->count);
  if(achild->count>0){
    printf("%d",(achild->pis)[0]);
  }
  for(int i =1; i<achild->count;i++){
    printf(", %d",(achild->pis)[i]);
  }
  printf("\n");
  printf("Sum: %d\tTurn: %d/%d\n",achild->sum,achild->turn,store.turn);
}



/*  Input
{
    command: 1,
    name: ""
}
*/


// =============================Server operation=================================

/*  Output
{ 
  status: ;
  sum: ;  //If is old user
  user_turn: ; 
  max_turn: 
}
*/

// Status:
// -1: error
//  0: The new user is created
//  1: Sign in as existed user
void sign_in(int fd, const char* user_name){
  //Get user
  child_t * achild;
  int flag = childds_find_by_name(&achild, user_name);
  
  // Case 1: Not found
  if(flag == - 1){
    achild = childds_add(user_name, strlen(user_name));
    
    achild->state=SIGN_IN;

    //MOCK: Send back to the remote user
    printf("{\"status\": %d,\"sum\":%d,\"user_turn\":%d, \"max_turn\": %d}\n",0,achild->sum,achild->turn,store.turn);
    return;
  }

  //MOCK: Send back to the remote user
  achild->state=SIGN_IN;
  printf("{\"status\": %d,\"sum\":%d,\"user_turn\":%d, \"max_turn\": %d}\n",1,achild->sum,achild->turn,store.turn);
  // Return
}


// Status:
// -1: error
//  0: Waitting
//  1: Playing

/*  Input
{
    command: 2;
    is_sending: 0 | 1;
}
*/

/*  Output
{
  status: -1 | 0 | 1;
}
*/
void check_srvstate(const int fd, const int isending){
  //TODO: Implement get user state
  printf("{\"status\": %d}\n",1);
}



/*  Input
{
    command: 3;
    name:;
}
*/

/*  Output
{
  status: -1 | 1;
  pis: [1,2,3,4,5,6];
  length: 6;
  user_sum: 150;
  user_turn: 3
}
*/
void get_pi(const int fd, const char* name){
  //TODO: Implement get_pi
  printf("{\"status\": 1, \"pis\": [1,2,3,4,5,6], \"length\": 6, \"user_sum\": 150, \"user_turn\': 3}\n");
  return;
}



/*  Input
{
    command: 4;
}
*/

/* Response from the server
{
  status:
  name: --if status ok
  sum: -- if status ok
}
*/
void get_winner(const int fd){
  printf("{\"status\": %d, \"name\": \"%s\", \"sum\": %d}\n",1,"Hello",10);
}
//

int parse_request(const char* json, int * command, void**arg){
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

  // Field "command"
  temp_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "command");
  if(!cJSON_IsNumber(temp_cJ)){
    fprintf(stderr, "[Parse_request] The command field is not a number\n");
    goto end;
  }
  *command = temp_cJ->valueint;


  char * name;
  switch(*command){
    /*  Input
      {
          command: 1,
          name: ""
      }
    */
    case SIGN_IN:
      //Parse Name
      temp_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "name");
      if (!cJSON_IsString(temp_cJ) || (temp_cJ->valuestring == NULL)){
        fprintf(stderr, "[SIGN_IN] The name field is not a string\n");
        goto end;
      }

      *arg = malloc(MAX_CHAR*sizeof(char));
      name = *arg;
      strncpy(name, temp_cJ->valuestring,MAX_CHAR);
      name[MAX_CHAR] = '\0';

      break;

    /*  Input
    {
        command: 2;
        is_ending: 0 | 1;
    }
    */ 
    case CHECK_SRVSTATE:
      temp_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "is_ending");
      if(!cJSON_IsNumber(temp_cJ)){
        fprintf(stderr, "[CHECK_SRVSTATE] The is_ending field is not a number\n");
        goto end;
      }
      *arg = malloc(sizeof(int));
      int *is_ending = *arg;
      *is_ending = temp_cJ->valueint;

      break;
    
    /*  Input
    {
        command: 3;
        name:;
    }
    */
    case GET_PI:
      //TODO: Implement get pi flow.
      temp_cJ = cJSON_GetObjectItemCaseSensitive(json_parser, "name");
      if (!cJSON_IsString(temp_cJ) || (temp_cJ->valuestring == NULL)){
        fprintf(stderr, "[GET_PI] The name field is not a string\n");
        goto end;
      }

      *arg = malloc(MAX_CHAR*sizeof(char));
      name = *arg;
      strncpy(name, temp_cJ->valuestring,MAX_CHAR);
      name[MAX_CHAR] = '\0';

      break;

    /*  Input
    {
        command: 4;
    }
    */
    case GET_WINNER:
      arg = NULL;
      break;

    default: 
      arg = NULL;
      break;
  }

  jstatus = 1;

  end:
  cJSON_Delete(json_parser);
  return jstatus;
}

void* request_process(void*arg){
  // FIXME: Remove init_child_ds after not being mocked
  init_child_ds();
  
  // 1. Argument is file descriptor.
  // int fd = *((int*)(arg));

  // 2. Receive request from the user
  // MOCK: Mock request from the user
  char buf[MAX_CHAR];
  snprintf(buf, MAX_CHAR,"{\"command\": %d,\"is_ending\":%d}",2,0);

  int command = 0;
  void * fields;
  char * name;

  int flag = parse_request(buf, &command, &fields);
  if(flag == -1){
    fprintf(stderr, "Error happen while parse user request\n");
    return NULL;
  }
  
  switch(command){
    case SIGN_IN:
      // Parse fields
      if(fields == NULL)
        return NULL;
      
      // Delegate the request
      name = fields;
      sign_in(0,name);  
    
      break;
       
    case CHECK_SRVSTATE:
      // Parse fields
      if(fields == NULL)
        return NULL;
      
      // Delegate the request
      int is_ending = *((int*)fields);
      check_srvstate(0,is_ending);

      break;  
    
    case GET_PI:
      // Parse fields
      if(fields == NULL)
        return NULL;
      
      // Delegate the request
      name = fields;
      get_pi(0,name);

      break;
    
    case GET_WINNER:

      get_winner(0);

      break;   
  }

  free(fields);

  // FIXME: Remove destroy_child_ds after not being mocked
  destroy_child_ds();
  return NULL;
  // 3. Parsing if it calls whatever functions.
  // 4. Delegating the response reponsible to that function with the fd and the input arguments.
}


// void init_app(){
//   //Init store
//   store_init(PI_NUM,RANGE_VALUE,SECTION_NUM,TURN,BAG_SIZE,CHILD_NUM);
//   threadpool thpool = thpool_init(store.max_child);
//   // int clt_cnt = 0;
//   int *args = malloc(2*sizeof(int));
  

//   // Init listener
//   //TODO: Number of client should be fixed
//   /*  Input
//   {
//       command: 1,
//       name: ""
//   }
//   */

//   int ffd = inetListen(SERVICE, 10, NULL), cfd = 0;
//   if (ffd == -1){
//     printf("Could not create socket(%s)", strerror(errno));
//     exit(-1);
//   }

//   while (1){
//     cfd = accept(ffd, NULL, NULL);
//     if (cfd == -1){
//       printf("Failure in accept(%s)\n", strerror(errno));
//       exit(-1);
//     }

//     args[0]=cfd;
//     thpool_add_work(thpool, &request_process, (void*)args);
//     // args[1] = clt_cnt;

//     // clt_cnt ++;
//     // if (clt_cnt > store.max_child){
//     //   pthread_mutex_lock(&start_lock);
//     //   is_full_slot = 1;
//     //   printf("is_full_slot: %d\n", is_full_slot);
//     //   pthread_mutex_unlock(&start_lock);
//     //   pthread_cond_broadcast(&full);
//     //   break;
//     // } 
//     // else {
//     //   thpool_add_work(thpool, &collection_test, (void*)args);
//     // }
//   }

//   close(ffd);
//   prepare_report();
//   thpool_wait(thpool);
// }


int main(int argc, char *argv[]){
  request_process(NULL);
  return -1;
}
