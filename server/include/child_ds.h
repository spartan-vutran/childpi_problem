#ifndef CHILDPI_DS
#define CHILDPI_DS


struct child{
  char * name;
  int * pis;
  int offset;
  int turn;
  int sum;
};

typedef struct child child_t;

// Allocate & dellocate
void init_child_ds(const int length);
void child_destroy(child_t* achild);
void destroy_child_ds();

// Child datastructure operations
int childds_find_by_name(child_t * achild, const char*name);
child_t* childds_add(const char*name);
child_t* childds_find_winner();

#endif