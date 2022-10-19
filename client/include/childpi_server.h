#ifndef CHILDPI_SERVER
#define CHILDPI_SERVER

#define MAX_CHAR 255

// Command definitions
#define CHECK_ACCOUNT 0
#define TIME_OUT 20
#define RETRY 5

struct childpi_server{
  char server_ip[MAX_CHAR];
  char service[MAX_CHAR];
};

struct childpi_server* server_stub;

void init_server_stub(const char*ip, const char*service);
void destroy_server_stub();


////////// Command declaration

// Status:
// -1: error
//  0: The new user is created
//  1: Sign in as existed user
int sign_in(const char* user_name,int* max_turn, int *user_sum, int * user_turn);


// Status:
// -1: error
//  0: Waitting
//  1: Playing
int check_srvstate(const int isending);


// Status:
// -1: error
//  1: OK
int get_pi(const char * user_name, int* pis, int* length, int* user_sum, int* user_turn);

// Status:
// -1: error
//  0: No
//  1: OK
int get_winner(char* winner, const int max_length, int* sum);

#endif