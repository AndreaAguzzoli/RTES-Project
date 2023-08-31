#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
using namespace std;

int righe;
int colonne;
int** myArray;


int type;
int* pop_next; //indica il primo elemento occupato
int* push_next; //indica l'ultimo elemento occupato
int* push_block;
int* pop_block;
int* tot; //indica il numero di elementi presenti nell'array per capire se è pieno o vuoto
int dim;
bool* empty;
bool* full;
sem_t mutex; //Per accedere in maniera mutuamente esclusiva alla coda.
sem_t** sem_empty; //Semaforo su cui si blocca chi vuole leggere ma la coda è vuota.
sem_t** sem_full; //matrice di semafori

int push(int element, int priority);

int stampaMatrice ();

class Interface {

    public: 

    int createMatrix (int r, int c);    

    int MAIN (int r, int c);

};