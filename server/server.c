
// Connection library
#include "./include/inet_sockets.h"

// Thread pool library
#include "./include/thpool.h"
#include <pthread.h>

// ERROR library
#include <errno.h>

#define MAX_CHAR 255
#define SERVICE "50003"


/*  Input
{
    command: 1,
    name: ""
}
*/

/*  Output
{ 
  status: ;
  sum: ;  //If is old user
  user_turn: ; 
  max_turn: 
}
*/
int sign_in(const char* user_name,int* max_turn, int *user_sum, int * user_turn);


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
int check_srvstate(const int fd, const int isending);



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
void get_pi(const int fd);



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

}
//

void* request_process(void*arg){
  // 1. Argument is file descriptor.
  // 2. Receive request from the user
  // 3. Parsing if it calls whatever functions.
  // 4. Delegating the response reponsible to that function with the fd and the input arguments.
}




int main(int argc, char *argv[]){

}
