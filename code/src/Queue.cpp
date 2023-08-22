#include<iostream>
#include<stdlib.h>
#include"Queue.h"
using namespace std;

//Implementazione costruttori
template<class T>
Queue<T>::Queue(int type, int levels, size_t dim){
    cout << "type: " << type << endl;
    cout << "levels: " << levels << endl;
    cout << "dim: " << dim << endl;
    if(dim <= 0){
        cout << "Impossibile avere una coda di dimensione minore o uguale 0." << endl;
        return;
    }
    if(levels <= 0){
        cout << "Impossibile avere una coda con 0 o meno livelli di priorità." << endl;
        return;
    }
    if(type<FIFO || type > DYNAMIC_PRIORITY){
        cout << "Valore non valido per il parametro type." << endl;
        return;
    }

    this->dim = dim;
    this->levels = levels;
    this->type = type;

    if(this->type == FIFO && this->levels != 1){
        cout << "Impossibile avere una coda di " << levels << " livelli se gestita FIFO." << endl;
        return;
    }
    if(this->type != FIFO && this->levels == 1){
        cout << "La coda deve avere type=FIFO avendo inserito un solo livello di priorità." << endl;
        return;
    }

    this->queue = (T**)malloc(this->dim*sizeof(T*));
    for(int i=0; i<this->levels; ++i){
        if(type>FIFO)
            this->queue[i] = (T*)malloc(this->dim*sizeof(T));
        else
            this->queue[i] = (T*)malloc(sizeof(T));
        this->queue[i][0] = 0;
    }
}

template<class T>
Queue<T>::Queue(size_t dim){
    cout << "Here I am" << endl;
    if(dim <= 0){
        cout << "Impossibile avere una coda di dimensione minore o uguale 0." << endl;
        return;
    }
    this->type = FIFO;
    this->levels = 1;
    this->dim = dim;

    // dichiarazione primoVuoto e primoPieno (array con un solo slot)
    head = (int*)malloc(1*sizeof(int));
    tail = (int*)malloc(1*sizeof(int));
    numElementi = (int*)malloc(1*sizeof(int));
    head[0] = 0;
    tail[0] = 0;
    numElementi[0] = 0;


    this->queue = (T**)malloc(this->dim*sizeof(T*));
    for(int i=0; i<this->dim; ++i)
        this->queue[i] = (T*)malloc(sizeof(T));
}

template<class T>
void Queue<T>::setLevels(int newlevels){
    if(!empty){
        cout << "Non è possibile modificare i livelli di priorità di una coda non vuota." << endl;
        return;
    }
    if(newlevels <= 0){
        cout << "Impossibile avere una coda con 0 o meno livelli di priorità." << endl;
        return;
    }
    else if(this->type == FIFO){
        cout << "Non è possibile modificare i livelli di priorità per una coda FIFO." << endl;
        return;
    }
    else if(newlevels < 2){
        cout << "Non è possibile avere una coda a priorità statica/dinamica con 1 solo livello di priorità." << endl;
        return;
    }
    for(int i=(this->levels-1); i>=newlevels; --i)
        free(this->queue[i]);
    this->levels = newlevels;
}

template<class T>
void Queue<T>::pop (int priority) {
    T result;
    if (type == FIFO) {
        // in questo caso la coda è di tipo FIFO
        //pthread_mutex_lock(&mutex);
        try {
            if (numElementi != 0) {     // se la coda non è vuota allora ...
                result = queue[head[0]];
                head[0]++;
                numElementi[0]--;
            }
            else {
                throw -1;
            }
        }
        catch (...) {
            cout << "ERRORE: La coda è vuota e non è possibile eseguire la POP.\n";
        }
        //pthread_mutex_unlock(&mutex);
    }
    else {
        // in questo caso la funzione deve lavorare sulla coda relativa ad un certo livello di priorità
        for (int i=0; i<this->levels; i++) {
            //scorro i livelli di priorità finchè non trovo quello specificato come parametro
            if (priority == i) { 
                //pthread_mutex_lock(&mutex);
                try {
                    if (numElementi[i] != 0) {  // se la coda indicata non è vuota allora ...
                        result = queue[head[i]];
                        head[i] = (head[i] - 1) % dim;
                        numElementi[i]--; 
                    }
                    else {
                        throw -1;
                    }
                }
                catch (...) {
                    cout << "ERRORE: La coda di priorità indicata è vuota, quindi non è possibile eseguire la POP.\n";
                }
                //pthread_mutex_unlock(&mutex);
                break;
            }
        }
    }
}

template<class T>
void Queue<T>::push (T element, int priority) {
    if (type == FIFO) {
        // in questo caso la coda è di tipo FIFO
        //pthread_mutex_lock(&mutex);
        try {            
            if ( ((tail[0] + 1) % dim) != head[0] ) { // se l'array non è pieno allora ...
                ++tail[0];
                queue[tail[0]];
                numElementi[i]++;
            }
            else {
                throw -1;
            }
        }
        catch (...) {
            cout << "ERRORE: La coda è piena, quindi non è possibile eseguire la PUSH.\n";
        }
        //pthread_mutex_unlock(&mutex);
    }
    else {
        // in questo caso la funzione deve lavorare sulla coda relativa ad un certo livello di priorità
        for (int i=0; i<this->levels; i++) {
            //scorro i livelli di priorità finchè non trovo quello specificato come parametro
            if (priority == i) { 
                //pthread_mutex_lock(&mutex);
                try {
                    if ( ((tail[i] + 1) % dim) != head[i] ) { // se l'array non è pieno allora ...
                        ++tail[i];
                        queue[tail[i]];
                        numElementi[i]++;
                    }
                    else {
                        throw -1;
                    }
                }                
                catch (...) {
                    cout << "ERRORE: La coda di priorità indicata è piena, quindi non è possibile eseguire la PUSH.\n";
                }
                //pthread_mutex_unlock(&mutex);
                break;
            }
        }
    }
}

int main(void){
    Queue<int>  q, q2((size_t)150), q3(FIXED_PRIORITY, 12);
    q3.setLevels(10);
}