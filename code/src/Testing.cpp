#include<iostream>
#include<unistd.h>
#include<string>
#include<stdlib.h>

#include"Queue.h"
#include"Queue.cpp" 

using namespace std;
Queue<int> q(RELIABILITY, 3, 3);

void mySleep(void)
{
  struct timespec t;
  t.tv_sec = 0;
  t.tv_nsec = (rand()%10+1)*1000000;
  nanosleep(&t,NULL);
}

void* pop_routine(void* p){
    int id = *((int *) p);
    int times = 25;
    while(times){
        int pop = q.pop();
        mySleep();
        --times;
    }
    cout << "POPPER DONE" << endl;
    return &id;
}

void* push_routine(void* p){
    int id = *((int *) p);
    int times = 25;
    while(times){
        int push = rand() % 150;
        int level = rand() % q.getLevels();
        q.push(push, level);
        mySleep();
        --times;
    }
    cout << "PUSHER DONE" << endl;
    return &id;
}

int main(){
    pthread_t *threads;
    int **ret;
    int *taskids;
    
    threads = (pthread_t *) malloc(THREADS*sizeof(pthread_t));
    taskids = (int *)malloc(THREADS*sizeof(int));

    srand(65);

    for(int i=0; i<THREADS; ++i){
        taskids[i] = i;
        if((i%2)==0)
            pthread_create(&threads[i], nullptr, pop_routine, (void*) &taskids[i]);
        else
            pthread_create(&threads[i], nullptr, push_routine, (void*) &taskids[i]);
    }

    for(int i=0; i<THREADS; ++i){
        pthread_join(threads[i], (void **) &ret);
    }

    return 0;
}