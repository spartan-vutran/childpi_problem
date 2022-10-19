#ifndef CHILDPI_APP
#define CHILDPI_APP


#include <stdint.h>
#include <pthread.h>

// Network configuration
#define SERVICE "50003"
#define SERVER "localhost"


// Type definition
#define MAX_CHAR 255

// App status 
#define NOT_SIGNIN 0
#define WAITTING 1
#define PLAYING 2
#define ENDING 3
#define TIME_OUT_ERROR 4
// Processing
#define POLLING_TIME 2
#define TIME_OUT 40

// Sending parser
struct childpi_app{
  char  user_name[MAX_CHAR];
  uint32_t state;
  uint32_t user_sum;
  uint32_t user_turn;
  uint32_t max_turn;

  //Mutex
  pthread_mutex_t status_mutex;
  pthread_cond_t is_end;
};

struct childpi_app * app;


void init_app();
void destroy_app();
void play();

#endif