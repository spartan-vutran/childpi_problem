#include "./child_ds.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Define the data structure
struct child_ds{
  child_t * childs;
  int capacity;
  int count;
};

struct child_ds * ds;




void destroy_child(child_t *childs,int length){
  for(int i =0; i<length ;i++){
    free((childs[i]).name);
    free((childs[i]).pis);
  }

  free(childs);
}
// Allocate & deallocate
void init_child_ds(){
  ds = malloc(sizeof(struct child_ds));
  ds->capacity = DEFAULT_CHILD;
  ds->childs = malloc((ds->capacity)*sizeof(child_t));
  ds->count = 0;
}


void destroy_child_ds(){
  destroy_child(ds->childs,ds->capacity);
  free(ds);
}


int childds_find_by_name(child_t ** achild, const char*name){
  for(int i = 0; i< ds->count; i++){
    if(!strcmp(name, (ds->childs)[i].name)){
      *achild =&((ds->childs)[i]);
      return 1;
    }
  }

  return -1;
}

child_t* childds_add(const char*name,const int length){
  if(ds == NULL)
    return NULL;
  

  //Add in ds
  child_t * a_child = NULL;
  if(ds->count < ds->capacity){
    a_child = &((ds->childs)[ds->count]);
  } 
  else{
    int old_capacity = ds->capacity;
    ds->capacity= (ds->capacity*3)/2;
    child_t * new_childs = malloc(sizeof(child_t)*(ds->capacity));
    memmove(new_childs,ds->childs,old_capacity*sizeof(child_t));

    free(ds->childs);
    ds->childs = new_childs;

    a_child = &((ds->childs)[ds->count]);
  }

  //Initialization
  a_child->name = malloc(sizeof(char)*MAX_CHAR);
  
  int count = (length < MAX_CHAR)?length:MAX_CHAR;
  memcpy(a_child->name, name, count);
  a_child->name[count] = '\0';
  
  
  a_child->pis = malloc(sizeof(int)*DEFAULT_PI);
  a_child->pi_capacity = DEFAULT_PI;
  a_child->count = 0;
  a_child->turn = 0;
  a_child->sum = 0;

  //Increase count of ds child
  (ds->count)++;

  return a_child;
}

child_t* childds_find_winner(){
  if(ds == NULL || ds->count == 0)
    return NULL;

  int sum = (ds->childs)[0].sum;
  child_t *winner = &(ds->childs)[0];
  for(int i = 1; i<(ds->count);i++){
    if(sum < (ds->childs)[i].sum){
      sum = (ds->childs)[i].sum;
      winner = &(ds->childs)[i];
    }
  }

  return winner;
}


int add_pi_2child(const char* name, const int*pis, int length){
  child_t *target_child;
  //The child does not exits.
  if(childds_find_by_name(&target_child, name) == -1){
    return -1;
  }



  if((target_child->count + length)>= target_child->pi_capacity){
    int old_capacity = target_child->pi_capacity;
    target_child->pi_capacity = ((target_child->count + length)*3)/2;
    int* new_pis = malloc(target_child->pi_capacity*sizeof(int));
    memmove(new_pis, target_child->pis, sizeof(int)*old_capacity);
    free(target_child->pis);
    target_child->pis = new_pis;
  } 

  for(int i = 0; i<length ;i++){
    (target_child->pis)[target_child->count]  = pis[i];
    (target_child->count)++;
    (target_child->sum)+= pis[i];
  }

  return 1;
}