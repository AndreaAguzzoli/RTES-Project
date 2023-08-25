#include<iostream>
#include<stdlib.h>
#include<pthread.h>
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

    this->queue = (T**)malloc(this->levels*sizeof(T*));
    this->queue[0] = (T*)malloc(this->dim*sizeof(T));

    this->empty = (bool*)malloc(this->levels*sizeof(bool));
    this->full = (bool*)malloc(this->levels*sizeof(bool));

    this->pop_next = (int*)malloc(this->levels*sizeof(int));
    this->push_next = (int*)malloc(this->levels*sizeof(int));

    this->tot = (int*)malloc(this->levels*sizeof(int));

    for(int i=0; i<this->levels; ++i){
        this->queue[i] = (T*)malloc(this->dim*sizeof(T));
        this->empty[i] = true;
        this->full[i] = false;
        this->tot[i] = 0;
        this->pop_next[i] = 0;
        this->push_next[i] = 0;

        if(this->type == FIFO)
            break;
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

    this->empty = (bool*)malloc(siezof(bool));
    this->empty[0] = true;
    this->full = (bool*)malloc(sizeof(bool));
    this->full[0] = false;

    this->tot = (int*)malloc(sizeof(int));
    this->tot[0] = 0;

    this->pop_next = (int*)malloc(sizeof(int));
    this->pop_next[0] = 0;
    this->push_next = (int*)malloc(sizeof(int));
    this->push_next[0] = 0;

    this->queue = (T**)malloc(sizeof(T*));
    this->queue[0] = (T*)malloc(this->dim*sizeof(T));
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
T Queue<T>::pop (int priority) {
    if(priority >= this->levels || priority < -1){
        cout << "Impossibile eseguire la POP. Priorità specificata non valida." << endl;
        return NULL;
    }
    if (this->type == FIFO)
        ++priority; //Priorità 0.
    else if(priority == -1){
        //Comportamento di default del metodo per le code multiple.
        ++priority;
        while(this->empty[priority]) 
            ++priority;
        if(priority >= this->levels){
            cout << "Tutte le code risultano vuote." << endl;
            return NULL;
        }
    }
    else{
        //Priorità specificata esplicitamente per le code multiple.
        if(this->empty[priority]){
            cout << "La coda da cui si vuole estrarre l'elemento risulta essere vuota." << endl;
            return NULL;
        }
    }

    T ret = this->queue[priority][this->pop_next[priority]];
    this->pop_next[priority] = (++this->pop_next[priority])%this->dim;
    --this->tot[priority];
        
    if (this->pop_next[priority] == this->push_next[priority]) 
        //Se dopo una pop i due puntatori coincidono, allora l'array è vuoto
        this->empty[priority] == true;
        if(this->full[priority])
        this->full[priority] = false;
    return ret;
}

template<class T>
void Queue<T>::push (T element, int priority) {
    if (priority == 0) {
        // in questo caso la coda è di tipo FIFO
        //pthread_mutex_lock(&mutex);
        try {            
            if (full == false) {
                // inserisci elemento quando trovo una posizione libera
                for (int i=0; i<this->dim; i++){                    
                    if (queue[i] == NULL) {
                        // trovata una posizione vuota, posso inserire qui il nuovo elemento
                        queue[i] = element;
                        if (empty == true) { // se la coda era vuota ora segno che non lo è più
                            empty = false;
                        }
                        break;
                    }                    
                }
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
                bool isFull = true;
                for (int j=0; j<this->dim; j++) {
                    if (queue[i][j] == NULL) {
                        isFull = false;
                        break;
                    }
                }
                try {
                    if (isFull == false) {
                        for (int j=0; j<this->dim; j++) {
                            if (queue[i][j] == NULL) {
                                // trovata una posizione vuota, posso inserire qui il nuovo elemento
                                queue[i][j] = element;
                                break;
                            }
                        }
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