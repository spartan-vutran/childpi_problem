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

#define MAX_CHAR 255
#define SERVICE "50003"
#define SIZE 10000
#define TURN 5
#define COLLECT_NUM 5
#define THREAD_NUM 5

// Data structure on server
int pis[SIZE];
pthread_mutex_t mutex;
pthread_cond_t full;
int is_full = 0;
#define EMPTY -1



struct client{
  int id;
};

struct client clts[THREAD_NUM];



int rand_value(int from, int to){
  return rand()%(to-from+1) + from;
}

void init_data(int range){
  for(int i = 0; i<SIZE; i++){
    pis[i] = rand_value(1,range);
  }
}


static void collection_test(void *arg)
{
  int fd = *((int*)arg);
  int clt_id = *((int*)(arg + sizeof(int)));
  char hello[255];
  int index= 0;

  sprintf(hello,"Hello client %d with %d",clt_id,fd);

  int numWrite = 0;
  while (1){
    numWrite = send(fd, hello, strlen(hello), 0);
    if (numWrite != strlen(hello)){
      close(fd);
      printf("Error occur when sending\n");
      return;
    }

    pthread_mutex_lock(&mutex); 
    while(is_full==0){
      pthread_cond_wait(&full, &mutex); 
    }
    
    index = rand_value(0, SIZE-1);
    if(pis[index] != EMPTY){
      
    }

    pthread_mutex_unlock(&mutex);
    
    break;
  }
}


int main(int argc, char *argv[])
{
  threadpool thpool = thpool_init(THREAD_NUM);
  int clt_cnt = 0, num_thread =THREAD_NUM;
  int ffd = inetListen(SERVICE, 10, NULL), cfd = 0;
  int *args = malloc(2*sizeof(int));

  // Init listener
  if (ffd == -1){
    printf("Could not create socket(%s)", strerror(errno));
    exit(-1);
  }

  // Init random time
  time_t t;
  srand((unsigned) time(&t));

  // Init mutex
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&full,NULL);

  while (1){
    cfd = accept(ffd, NULL, NULL);
    if (cfd == -1){
      printf("Failure in accept(%s)\n", strerror(errno));
      exit(-1);
    }

    args[0]=cfd;
    args[1] = clt_cnt;

    
    clt_cnt ++;
    if (clt_cnt > num_thread){
      pthread_mutex_lock(&mutex);
      is_full = 1;
      printf("is_full: %d\n", is_full);
      pthread_mutex_unlock(&mutex);
      pthread_cond_broadcast(&full);
      break;
    } 
    else {
      thpool_add_work(thpool, &collection_test, (void*)args);
    }
    
    
  }

  thpool_wait(thpool);

  if (pthread_cond_destroy(&full) != 0) {                                       
    perror("pthread_cond_destroy() error");                                     
    exit(2);                                                                    
  }  
  

 
}
