#ifndef CHILDPI_DS
#define CHILDPI_DS


#define MAX_CHAR 255
#define DEFAULT_PI 100
#define DEFAULT_CHILD 10

struct child{
  char * name;
  int * pis;
  int pi_capacity;
  int count;
  int turn;
  int sum;
};

typedef struct child child_t;

// Allocate & dellocate
void init_child_ds();
void child_destroy(child_t* achild);
void destroy_child_ds();

// Child datastructure operations
int childds_find_by_name(child_t ** achild, const char*name);
child_t* childds_add(const char*name,const int length);
child_t* childds_find_winner();

// Pi operation
// -1 on error
// 1 ok
int add_pi_2child(const char* name, const int*pis, int length);

#endif