#include<iostream>
#include<unistd.h>
#include<string>
#include<stdlib.h>
#include"Queue.h"
#include"Queue.cpp" 

using namespace std;
Queue<int> q(RELIABILITY, 1, 8);

void* pop_routine(void* p){
    int id = *((int *) p);
    int times = 23;
    //cout << "THREAD " << id << " POPPER STARS FOR " << times << " TIMES!" << endl;
    while(times){
        int pop = q.pop();
        --times;
    }
    cout << "POPPER DONE" << endl;
    return &id;
}

void* push_routine(void* p){
    int id = *((int *) p);
    int times = 23;
    //cout << "THREAD " << id << " PUSHER STARTS FOR " << times << " TIMES!" << endl;
    while(times){
        int push = rand() % 150;
        int level = rand() % q.getLevels();
        q.push(push, level);
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

    srand(2387);

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