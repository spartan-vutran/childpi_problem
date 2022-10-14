#include <pthread.h>

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <errno.h>

pthread_mutex_t mutexFuel ;
pthread_cond_t condFuel ;    // creating the condition variable.
int fuel = 0;

void* fuelling(void* arg) {
  for (int i = 0; i < 5; i++) {
    pthread_mutex_lock(&mutexFuel);
    fuel += 15 ;
    printf ( " Got fuel... %d \n " ,  fuel ) ;
    pthread_mutex_unlock(&mutexFuel) ;
    pthread_cond_signal(&condFuel) ;
    sleep(1);
  }
}

void* vehicle(void* arg) {
  pthread_mutex_lock(&mutexFuel);
  while (fuel < 40) {
    printf ( " Zero fuel. Waiting... \n " ) ;
    pthread_cond_wait(&condFuel, &mutexFuel);
    // Equivalent to:
    // pthread_mutex_unlock(&mutexFuel);
    // wait for signal on condFuel
    // pthread_mutex_lock(&mutexFuel) ;
  }
  fuel -= 40 ;
  printf ( " fuel collected Now left: %d \n " , fuel ) ;
  pthread_mutex_unlock(&mutexFuel);
}

int main(int argc, char* argv[]) {
  pthread_t a[2] ;
  pthread_mutex_init(&mutexFuel, NULL) ;
  pthread_cond_init(&condFuel, NULL) ;

  for ( int i = 0 ; i < 2 ; i++ ) {
    if (i == 1) {
      if (pthread_create(&a[i], NULL, &fuelling, NULL) != 0) {
        perror("Failed to create thread");
      }
    } else {
      if (pthread_create(&a[i], NULL, &vehicle, NULL) != 0) {
        perror("Failed to create thread");
      }
    }
  }

  for ( int i = 0 ; i < 2 ; i++ ) {
    if (pthread_join(a[i], NULL) != 0) {
      perror("Failed to join thread") ;
    }
  }

  pthread_mutex_destroy(&mutexFuel) ;
  pthread_cond_destroy(&condFuel) ;   // destroying the threads.
  return 0 ;
}