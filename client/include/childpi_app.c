#include "childpi_app.h"
#include "childpi_server.h"
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


/*
Thread 1 (main thread):
  + If the user got no turn, It start waitting thread 2 (cond_wait).
  + If the app->status was modifed to ENDING by thread 2. It get to thread 2 to get the report.
*/

void init_app(){
  app = malloc(sizeof(struct childpi_app));
  app->state = NOT_SIGNIN;
  
  
  pthread_mutex_init(&app->status_mutex,NULL);
  pthread_cond_init(&app->is_end,NULL);
  init_server_stub(SERVER,SERVICE);
}

void destroy_app(){
  free(app);
  destroy_server_stub();
  pthread_mutex_destroy(&app->status_mutex);
  pthread_cond_destroy(&app->is_end);

}


static int poll_start(){
  int flag = 0;
  int first = 1;

  time_t start = time(NULL);  
  while ((time(NULL) - start)<TIME_OUT) {  //Time out
    
    
    flag =  check_srvstate(0);

    if(flag == -1){
      return -1;
    }

    if(flag == 1){
      return 1;
    }

    if(first){
      printf("Waitting for other users...\n");
      first--;
    }

    fflush(stdout);
    sleep(POLLING_TIME);

  }

  return -1;
}


//  Thread 2: The role is to ping the server if the game has ended. 
  //    + It starts at the beginning of thread 1.
  //    + After the server responded with 1 signal. It would modify the app to state ENDING (need mutex) and exit with the result at the paramter 2
static void* poll_ending(){
  int flag = 0;
  int count =0;
  time_t start = time(NULL);  
  while ((time(NULL) - start)<TIME_OUT) {  //Time out
    printf("\n--------------\nPing ending %d times\n----------------\n", ++count);
    flag =  check_srvstate(1);

    if(flag == -1){
      //Trigger the main thread to exit because the error
      pthread_mutex_lock(&app->status_mutex);
      app->state = -1;
      pthread_mutex_unlock(&app->status_mutex);
      pthread_cond_signal(&app->is_end);

      return NULL;
    }

    if(flag == 1){
       //Trigger the main thread to end because the server has ended 
      pthread_mutex_lock(&app->status_mutex);
      app->state = ENDING;
      pthread_mutex_unlock(&app->status_mutex);
      pthread_cond_signal(&app->is_end);

      return NULL;
    }

    fflush(stdout);
    sleep(POLLING_TIME);

  }

  //Trigger the main thread to exit because the error
  pthread_mutex_lock(&app->status_mutex);
  app->state = TIME_OUT_ERROR;
  pthread_mutex_unlock(&app->status_mutex);
  pthread_cond_signal(&app->is_end);

  return NULL;
}



static void print_status(){
  printf("Your sum now is %d with %d turns left\n",app->user_sum,app->max_turn - app->user_turn);
}


static void print_gainedpi(int * pis, int length){
  printf("Pi gained: %d", pis[0]);
  for(int i = 1; i<length;i++){
    printf(", %d", pis[i]);
  }
  printf("\n");
  print_status();
}

void play(){
  init_app();
  int flag = 0;
  int exit = 0;

  //TODO: Create a thread here to poll if the server has ended.
  pthread_t poll_ending_th;
  pthread_create(&poll_ending_th, NULL,poll_ending,NULL);

  while (1){
    pthread_mutex_lock(&app->status_mutex);
    printf("\n\n\n---------------------------------------------------\n");
    switch(app->state){
      case NOT_SIGNIN:
        printf("Enter your name for playing game: ");
        scanf("%s",app->user_name);
        
        //Check if the user name is exist
        flag = sign_in(app->user_name, &app->max_turn, &app->user_sum, &app->user_turn);
        if(flag == -1){
          printf("Error happended while sending to server\n");
          exit = 1;
          break;
        }

        if(flag == 0){
          printf("Welcome %s\n", app->user_name);
          print_status();
        }

        if(flag == 1){
          printf("Login as old user %s\n", app->user_name);
          print_status();
        }

        app->state = WAITTING;
        break;
      
      case WAITTING:
        flag = poll_start();

        // Time out or error
        if(flag == -1){
          printf("Error happened while polling start to the server\n");
          exit =1;
          break;
        }

        app->state = PLAYING;
        break;
      case PLAYING:
        // Print option
        print_status();
        printf("Choose your options:\n");
        printf("1. Get pi\n");
        printf("2. Exit\n");

        int option = 0, check = 0;
        printf("Your option is: ");
        check = scanf("%d", &option);



        //Processing
        printf("--------------\n");
        if(option == 1){
          int pis[100];
          int length = 0;
          int flag = get_pi(app->user_name, &pis[0], &length,&app->user_sum, &app->user_turn);
          if(flag == -1){
            printf("Error happened while get pi from the server\n");
            exit = 1;
            break;
          }

          print_gainedpi(pis, length);

          //Check if the user has got no turn
          if(app->user_turn >= app->max_turn){
            //TODO: Create a signal if the status has still not turned into ENDING
            //TODO: Consider to create time out.
            printf("You has got no turn. Waiting for the report...\n");
            while(app->state == PLAYING){
              pthread_cond_wait(&app->is_end,&app->status_mutex);
            }
          }
          break;
        }

        if(option == 2){
          exit = 1;
          break;
        }

        
        printf("Invalid option, please try again\n");
        break;

      case ENDING:
        printf("The game was ended.\n");
        print_status();
        
        //Print out the winner
        char name[MAX_CHAR];
        int sum = 0;
        int flag = get_winner(name,MAX_CHAR,&sum);

        if(sum > (app->user_sum)){
          printf("The winner is %s with %d points which is as higher %d than that of you.\n", name,sum,sum-app->user_sum );
          exit = 1;
          break;
        }
          
        
        if(sum == (app->user_sum)){
          if(!strcmp(name, app->user_name)){
            printf("You are the winner with %d points. Congratulation!!!\n", app->user_sum);
            exit = 1;
            break;
          }

          printf("You are the winner but ranked equally with %s. Both of you got %d points. Congratulation!!!\n", name, app->user_sum);
          exit = 1;
          break;
        }

        printf("Something goes wrong. App exit...\n");
        exit = 1;
        break;
      
      case TIME_OUT_ERROR:
        fprintf(stderr,"Time out polling server ending! Exit the app...");
        exit = 1;
        break;

      default:
        printf("Oops, Something goes wrong\n");
        exit = 1;
        break;
    }

    if(exit ==1){
      break;
    }
    pthread_mutex_unlock(&app->status_mutex);
  }

  destroy_app();
}