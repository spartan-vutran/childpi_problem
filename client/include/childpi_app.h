#ifndef CHILDPI_APP
#define CHILDPI_APP


#include <stdint.h>

// Network configuration
#define SERVICE "50003"
#define SERVER "localhost"


// Type definition
#define MAX_CHAR 255

// App status 
#define NOT_SIGNIN 0
#define WAITTING 1
#define PLAYING 2

// Processing
#define POLLING_TIME 5
#define TIME_OUT 20

// Sending parser
struct childpi_app{
  char  user_name[MAX_CHAR];
  uint32_t state;
  uint32_t user_sum;
  uint32_t user_turn;
  uint32_t max_turn;
};

struct childpi_app * app;


void init_app();
void destroy_app();
void play();


#endif