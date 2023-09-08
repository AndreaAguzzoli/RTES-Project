#include<iostream>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include"Queue.h"
#include"Queue.cpp"
 Queue<int> q((size_t)60, RELIABILITY);

void* pop_routine(void* p){
    int id = *((int *) p);
    int times = 50;
    //cout << "THREAD " << id << " POPPER STARS FOR " << times << " TIMES!" << endl;
    while(times){
        int pop = q.pop();
        --times;
    }
    return &id;
}

void* push_routine(void* p){
    int id = *((int *) p);
    int times = 50;
    //cout << "THREAD " << id << " PUSHER STARTS FOR " << times << " TIMES!" << endl;
    while(times){
        int push = rand() % 50;
        q.push(push);
        --times;
    }
    return &id;
}

int main(){
    pthread_t *threads;
    int *p;
    int *taskids;
    
    threads = (pthread_t *) malloc(2*THREADS*sizeof(pthread_t));
    taskids = (int *)malloc(2*THREADS*sizeof(int));

    srand(265);

    for(int i=0; i<THREADS*2; ++i){
        taskids[i] = i;
        if((i%2)==0)
            pthread_create(&threads[i], nullptr, pop_routine, (void*) &taskids[i]);
        else
            pthread_create(&threads[i], nullptr, push_routine, (void*) &taskids[i]);
    }

    for(int i=0; i<THREADS*2; ++i){
        int ret;
        pthread_join(threads[i], (void **) &p);
        cout << "MAIN: THREAD " << i << " ENDED WITH NO FAULT." << endl << endl;
    }

    return 0;
}