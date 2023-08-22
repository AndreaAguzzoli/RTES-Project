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
    if (priority == 0) {
        // in questo caso la coda è di tipo FIFO
        //pthread_mutex_lock(&mutex);
        try {
            if (empty == false) {
                for (int i=0; i<this->dim-1; i++) {
                    queue[i] = queue [i+1];
                }
                queue[dim-1] = NULL;
                //dopo aver eliminato un elemento guardo se ora la coda è vuota, in tal caso setto empty = true.
                if (queue.empty() == true) {
                    empty == true;
                }
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
                bool isEmpty = queue[i].empty(); // isEmpty = true --> vuoto ; isEmpty = false --> non vuoto
                try {
                    if (isEmpty == false) {
                        for (int j=0; j<this->dim-1; j++) {
                            queue[i][j] = queue [i][j+1];
                        }
                        queue[i][dim-1] = NULL;
                        //dopo aver eliminato un elemento guardo se ora la coda è vuota, in tal caso setto empty = true.
                        if (queue[i].empty() == true) {
                            empty == true;
                        }
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