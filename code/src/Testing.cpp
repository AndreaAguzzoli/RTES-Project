#include<iostream>
#include<unistd.h>
#include<string>
#include<stdlib.h>
#include"Queue.h"
#include"Queue.cpp"

Queue<string> q(RELIABILITY, 1, 8);

void* pop_routine(void* p){
    int id = *((int *) p);
    int times = 23;
    //cout << "THREAD " << id << " POPPER STARS FOR " << times << " TIMES!" << endl;
    while(times){
        string pop = q.pop();
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
        string push = to_string(rand() % 26);
        int level = rand() % 3;
        q.push(push+"ciao");
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