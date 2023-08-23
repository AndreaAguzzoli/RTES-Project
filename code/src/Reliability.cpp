#include <iostream>
#include <stdlib.h>
#include "Queue.h"
#include <pthread.h>
#include <semaphore.h>
using namespace std;

sem_t mutex; // semaforo che va inizializzato a 1
sem_t semPush, semPop; // semafori che vanno inizializzato a 0
int pushBloccati = 0, popBloccati = 0;



void startPush () {
    sem_wait(&mutex);
    /*per garantire l'inserimento del dato all'interno della coda, il thread si blocca su un semaforo apposito se non ci sono le condizioni di inserimento (coda piena), altrimenti prosegue al suo inserimento. Questo garantisce che il dato verrà inserito nel momento in cui il thread verrà risvegliato perchè ci sono nuovamente condizioni favorevoli all'inserimento (coda non più piena).*/
    if (numElementi[0] != dim) {
        // ci sono le condizioni (coda NON piena)
        sem_post(&semPush);
    }
    else {
        pushBloccati++;        
    }
    sem_post(&mutex);

    sem_wait(&semPush);
}

void endPush () {
    sem_wait(&mutex);
    /* se ci sono estrazioni bloccate ne sblocco una per prima per evitare la starvation delle POP, se non ci sono e invece ci sono inserimenti bloccati sblocco uno di loro. */
    if (popBloccati > 0) {
        popBloccati--;
        sem_post(&semPop);
    } 
    else if (pushBloccati > 0) {
        pushBloccati--;
        sem_post(&semPush);
    }
    sem_post(&mutex);
}

void startPop () {
    sem_wait(&mutex);
    /*per garantire l'estrazione del dato all'interno della coda, il thread si blocca su un semaforo apposito se non ci sono le condizioni di inserimento (coda vuota), altrimenti prosegue alla sua estrazione. Questo garantisce che il dato verrà estratto nel momento in cui il thread verrà risvegliato perchè ci sono nuovamente condizioni favorevoli all'estrazione (coda non più vuota).*/
    if (numElementi[0] != 0) {
        // ci sono le condizioni (coda NON vuota)
        sem_post(&semPop);
    }
    else {
        popBloccati++;        
    }
    sem_post(&mutex);

    sem_wait(&semPop);
}

void endPop () {
    sem_wait(&mutex);
    /* se ci sono inserimenti bloccati ne sblocco uno per primo per evitare la starvation delle PUSH, se non ci sono e invece ci sono estrazioni bloccate sblocco una di loro. */
    if (pushBloccati > 0) {
        pushBloccati--;
        sem_post(&semPush);
    } 
    else if (popBloccati > 0) {
        popBloccati--;
        sem_post(&semPop);
    }
    sem_post(&mutex);
}


template<class T>
void Queue<T>::push (T element, int priority) {
    if (type == FIFO) {
        // in questo caso la coda è di tipo FIFO
        startPush();

        sem_wait(&mutex);
        if ( ((tail[0] + 1) % dim) != head[0] ) { // se l'array non è pieno allora ...
            tail[0] = (tail[0] + 1) % dim;
            queue[tail[0]] = element;
            ++numElementi[0];
            // controllo se aggiornare i valori delle variabili 'empty' e 'full'
            if (numElementi[0] == dim) 
                full = true;
            else if ((numElementi[0] != 0) && (empty == true)){
                empty = false;
            }
        }
        sem_post(&mutex);

        endPush();
    }
    else {
        // in questo caso la funzione deve lavorare sulla coda relativa ad un certo livello di priorità
        for (int i=0; i<this->levels; i++) {
            //scorro i livelli di priorità finchè non trovo quello specificato come parametro
            if (priority == i) {
                startPush();

                sem_wait(&mutex);
                if ( ((tail[i] + 1) % dim) != head[i] ) { // se l'array non è pieno allora ...
                    tail[i] = (tail[i] + 1) % dim;
                    queue[tail[i]] = element;
                    ++numElementi[i];
                    // controllo se aggiornare i valori delle variabili 'empty' e 'full'
                    if (numElementi[i] == 0) 
                        full = true;
                    else if ((numElementi[i] != 0) && (empty == true)){
                        empty = false;
                    }
                }
                sem_post(&mutex);

                endPush();
                break;
            }
        }
    }
}

template<class T>
void Queue<T>::pop (int priority) {
    T result;
    if (type == FIFO) {
        // in questo caso la coda è di tipo FIFO
        startPop();

        sem_wait(&mutex);
        if (numElementi != 0) {     // se la coda non è vuota allora ...
            result = queue[head[0]];
            head[0] = (head[0] + 1) % dim;
            --numElementi[0];
            // controllo se aggiornare i valori delle variabili 'empty' e 'full'
            if (numElementi[0] == 0) 
                empty = true;
            else if ((numElementi[0] != dim) && (full == true)){
                full = false;
            }
        }
        sem_post(&mutex);

        endPop();
    }
    else {
        // in questo caso la funzione deve lavorare sulla coda relativa ad un certo livello di priorità
        for (int i=0; i<this->levels; i++) {
            //scorro i livelli di priorità finchè non trovo quello specificato come parametro
            if (priority == i) { 
                startPop();

                sem_wait(&mutex);
                if (numElementi[i] != 0) {  // se la coda indicata non è vuota allora ...
                    result = queue[head[i]];
                    head[i] = (head[i] + 1) % dim;
                    --numElementi[i];
                    // controllo se aggiornare i valori delle variabili 'empty' e 'full'
                    if (numElementi[i] == 0) 
                        empty = true;
                    else if ((numElementi[i] != dim) && (full == true)){
                        full = false;
                    }
                }
                sem_post(&mutex);

                endPop();
                break;
            }
        }
    }
}