// WAITING PROCESS
#include <sys/wait.h>

// SIGNAL library
#include <signal.h>

// ERROR library
#include <errno.h>

// Connection library
#include "./include/inet_sockets.h"

// Thread pool library
#include "./include/thpool.h"
#include <pthread.h>

// Others
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/syscall.h>

//Network definition
#define MAX_CHAR 255
#define SERVICE "50003"


// Store definition
#define PI_NUM 1000
#define RANGE_VALUE 10
#define TURN 5
#define BAG_SIZE 50
#define CHILD_NUM 5
#define SECTION_NUM 10

// #define COLLECT_NUM 5
// PI value definition
#define EMPTY -1
// #define PARTISION_SIZE 2000

// #define TIME_TURN 700

// Data definition on server
struct child{
  int gained_pis[TURN*BAG_SIZE];
  uint64_t offset;
  uint64_t sum;
  int turn;
};


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
pthread_cond_t full, is_end;
struct child* childs;
int is_full_slot = 0, child_count=0;




int rand_value(int from, int to){
  return rand()%(to-from+1) + from;
}

void store_init(int num_pi, int range,int section_num, int turn, int bag_size, int max_child){
  //Init game
  store.bag_size=bag_size;
  store.turn = turn;
  store.section_num=section_num;
  
  // Mutex init
  pthread_mutex_init(&start_lock, NULL);
  pthread_cond_init(&full, NULL);
  pthread_cond_init(&is_end, NULL);
  store.mutex = malloc(sizeof(pthread_mutex_t)*store.section_num);

  for(int i=0; i<store.section_num; i++){
    pthread_mutex_init(&(store.mutex[i]), NULL);
  }

  // Init pi
  store.num_pi = num_pi;
  store.pis = malloc(sizeof(int)*num_pi);
  for(int i=0;i<store.num_pi;i++){
    store.pis[i]= rand_value(1,range);
  }

  //Init random
  time_t t;
  srand((unsigned) time(&t));

  //Init child
  store.max_child = max_child;
  int length = sizeof(struct child)*store.max_child;
  childs = malloc(sizeof(struct child)*store.max_child);
  memset(childs,0,length);
}

void store_destroy(){
  free(childs);
  free(store.mutex);
  free(store.report);
}


void getPi (struct child * child_t){
  int index = rand_value(0, store.num_pi-1);
  int imutex = ((index*store.section_num)/store.num_pi);
  printf("Imutex %d\tIndex:%d\t%d\t%d\n", imutex,index,store.section_num,store.num_pi);
  int result = 0;
  int temp_offset = child_t->offset;
  
  pthread_mutex_lock(&(store.mutex[imutex]));  
  if(store.pis[index] != EMPTY){
      result = store.pis[index];
      store.pis[index] = EMPTY;
  }
  pthread_mutex_unlock(&store.mutex[imutex]);  
  
  //Update child status
  child_t->sum += result;    
  child_t->gained_pis[temp_offset] = result;
  child_t->offset++;
  
  return;
}

void print_iarray(char* buf,int max_size, const int*sequence , int from ,int to){
  memset(buf,0,max_size);
  char* result = malloc(max_size*sizeof(char));
  char* subresult = malloc(max_size*sizeof(char));
  char *temp;

  if(sequence == NULL || from >to){
    return;
  }

  snprintf(result, max_size, "%d", sequence[from]);
  for(int i = from+1;i<=to;i++){
    snprintf(subresult,max_size,"%s,%d",result,sequence[i]);
    temp = subresult;
    subresult = result;
    result=temp;
  }
  
  memcpy(buf,result,max_size);
  free(result);
  free(subresult);
}

static void collection_test(void *arg){
  // Get parameter
  int fd = *((int*)arg);
  int clt_id = *((int*)(arg + sizeof(int)));

  // Init temporary data
  char buf[255];
  int turn_count = 0;

  // Get child buffer
  struct child *this_child = &childs[clt_id];
  int pre_offset = this_child->offset;
  

  //Send WAITING
  int numWrite = 0;
  sprintf(buf,"Hello %d . Please Waiting...", clt_id+1);
  numWrite = send(fd,buf,strlen(buf),0);
  if(numWrite != strlen(buf)){
    printf("Error while sending waiting\n");
    close(fd);
    return;
  }


  //Increase child count
  pthread_mutex_lock(&start_lock);    
  child_count++;
  pthread_mutex_unlock(&start_lock);

  while (turn_count<store.turn){
    pthread_mutex_lock(&start_lock);    // Wait for other childs
    while(is_full_slot==0){
      pthread_cond_wait(&full, &start_lock); 
    }
    pthread_mutex_unlock(&start_lock);

    // Get pi 
    
    int  bag_count=0;
    while (bag_count<store.bag_size) {      // Set time out
      getPi(this_child);    
      bag_count++;
    }

    //Send report after 3s
    char seqstr[100];
    print_iarray(seqstr, MAX_CHAR, this_child->gained_pis, pre_offset, this_child->offset-1);
    

    snprintf(buf, MAX_CHAR,"\n{\n\tPis: %s;\n\tSum: %ld; \n}", seqstr, this_child->sum);
    numWrite = send(fd,buf,strlen(buf),0);
    if(numWrite != strlen(buf)){
      printf("Error while sending waiting\n");
      close(fd);
      return;
    }

    // Increase turn
    turn_count++;
    pre_offset = this_child->offset;
  }

  // Wait for sending the report
  
  pthread_mutex_lock(&start_lock); 
  child_count--;
  pthread_cond_signal(&is_end);
  while(is_full_slot==1){
    pthread_cond_wait(&full, &start_lock); 
  }
  pthread_mutex_unlock(&start_lock);

  numWrite = send(fd,store.report,strlen(store.report),0);
  if(numWrite != strlen(store.report)){
    printf("Error while sending report\n");
  }

  close(fd);
  return;
}


void prepare_report(){
  pthread_mutex_lock(&start_lock);
  while(child_count>0){
    pthread_cond_wait(&is_end,&start_lock);
  }

  store.report = malloc(MAX_CHAR*sizeof(char));
  
  // Find the winner
  int winnerId = 0;
  int max_sum=0;
  for(int i=0; i<store.max_child;i++){
    if(childs[i].sum>max_sum){
      max_sum= childs[i].sum;
      winnerId = i;
    }
  }

  snprintf(store.report,MAX_CHAR,"\n\n----------------------\nWinner is %d with the score of %ld\n", winnerId+1, childs[winnerId].sum);
  is_full_slot = 0;
  pthread_mutex_unlock(&start_lock);
  pthread_cond_broadcast(&full);
}


int main(int argc, char *argv[]){

  // store_init(PI_NUM,RANGE_VALUE,SECTION_NUM,TURN,BAG_SIZE,CHILD_NUM);
  // threadpool thpool = thpool_init(store.max_child);
  // int clt_cnt = 0;
  // int *args = malloc(2*sizeof(int));
  

  // // Init listener
  // int ffd = inetListen(SERVICE, 10, NULL), cfd = 0;
  // if (ffd == -1){
  //   printf("Could not create socket(%s)", strerror(errno));
  //   exit(-1);
  // }

  // while (1){
  //   cfd = accept(ffd, NULL, NULL);
  //   if (cfd == -1){
  //     printf("Failure in accept(%s)\n", strerror(errno));
  //     exit(-1);
  //   }

  //   args[0]=cfd;
  //   args[1] = clt_cnt;

  //   clt_cnt ++;
  //   if (clt_cnt > store.max_child){
  //     pthread_mutex_lock(&start_lock);
  //     is_full_slot = 1;
  //     printf("is_full_slot: %d\n", is_full_slot);
  //     pthread_mutex_unlock(&start_lock);
  //     pthread_cond_broadcast(&full);
  //     break;
  //   } 
  //   else {
  //     thpool_add_work(thpool, &collection_test, (void*)args);
  //   }
  // }

  // close(ffd);
  // prepare_report();
  // thpool_wait(thpool);


  // // Release resources
  // store_destroy();
  // free(args);
  // if (pthread_cond_destroy(&full) != 0) {                                       
  //   perror("pthread_cond_destroy() error");                                     
  //   exit(2);                                                                    
  // }  


  

  // Init listener
  int ffd = inetListen(SERVICE, 10, NULL), cfd = 0;
  if (ffd == -1){
    printf("Could not create socket(%s)", strerror(errno));
    exit(-1);
  }

  while (1){
    cfd = accept(ffd, NULL, NULL);
    if (cfd == -1){
      printf("Failure in accept(%s)\n", strerror(errno));
      exit(-1);
    }

    char buf[MAX_CHAR];
    int numRead = read(cfd,buf,MAX_CHAR);

    if(numRead == -1){
      printf("Failure in read(%s)\n", strerror(errno));
      exit(-1);
    }

    if(numRead == 0){
      close(cfd);
      break;
    }

    buf[numRead]='\0';
    printf("%s\n",buf);
    close(cfd);
    break;
  }

  close(ffd);
}
