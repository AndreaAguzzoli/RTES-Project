#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include"Queue.h"
using namespace std;

size_t dim(){
    return DIM;
}
int best_effort(){
    return BEST_EFFORT;
}
int reliability(){
    return RELIABILITY;
}
int threads(){
    return THREADS;
}
size_t int_to_sizet(int num){
    return (size_t)num;
}

//Implementazione costruttori
template<class T>
Queue<T>::Queue(bool reliability, int levels, size_t dim){
    if(dim <= 0){
        cout << "Impossibile avere una coda di dimensione minore o uguale 0." << endl;
        return;
    }
    if(levels <= 0){
        cout << "Impossibile avere una coda con 0 o meno livelli di priorità." << endl;
        return;
    }

    this->dim = dim;
    this->levels = levels;
    this->reliability = reliability;

    this->queue = (T**)malloc(this->levels*sizeof(T*));

    this->empty = (bool*)malloc(this->levels*sizeof(bool));
    this->full = (bool*)malloc(this->levels*sizeof(bool));

    this->pop_next = (int*)malloc(this->levels*sizeof(int));
    this->push_next = (int*)malloc(this->levels*sizeof(int));

    this->tot = (int*)malloc(this->levels*sizeof(int));
    
    this->sem_empty = (sem_t*)malloc(THREADS*sizeof(sem_t));
    this->sem_full = (sem_t**)malloc(this->levels*sizeof(sem_t*));
    sem_init(&this->mutex, 0, 1);
    sem_init(&this->mutex_pushblock, 0, 1);
    sem_init(&this->mutex_popblock, 0, 1);

    this->push_block = (int*)malloc(this->levels*sizeof(int));
    this->push_wakeup = (int*)malloc(this->levels*sizeof(int));
    this->pop_block = -1; 
    this->pop_wakeup = -1;

    for(int i=0; i<this->levels; ++i){
        this->queue[i] = (T*)malloc(this->dim*sizeof(T));
        this->sem_full[i] = (sem_t*)malloc((THREADS)*sizeof(sem_t));
        this->empty[i] = true;
        this->full[i] = false;
        this->tot[i] = 0;
        this->pop_next[i] = 0;
        this->push_next[i] = 0;
        this->push_block[i] = -1;
        this->push_wakeup[i] = this->dim -1;  
        for(int j=0; j<THREADS; ++j) {
            if(j < this->dim){
                sem_init(&this->sem_full[i][j], 0, 1);
                this->queue[i][j] = -1; //Inizializziamo la cosa SOLO A SCOPO DIMOSTRATIVO siccome siamo certi del fatto che verranno utilizzati int e double.
            }
            else
                sem_init(&this->sem_full[i][j], 0, 0);
        }
    }

    for(int i=0; i<THREADS; ++i)
        sem_init(&this->sem_empty[i], 0, 0);
}
template<class T>
Queue<T>::~Queue(){
    delete this->empty;
    delete this->full;
    delete this->pop_next;
    delete this->push_next;
    delete this->tot;
    delete this->push_block;
    delete this->push_wakeup;

    for(int i=0; i<this->levels; ++i){
        delete this->queue[i];
        delete this->sem_full[i];
    }
    delete this->queue;
    delete this->sem_full;
    delete this->sem_empty;
}

template<class T>
int Queue<T>::getLevels(){
    return this->levels;
}
template<class T>
size_t Queue<T>::getDim(){
    return this->dim;
}
template<class T>
bool Queue<T>::isReliability(){
    return this->reliability;
}
template<class T>
T** Queue<T>::getQueue(){
    return this->queue;
}
template<class T>
bool Queue<T>::isEmpty(){
    bool empty = true;
    for(int i=0; i<this->levels; ++i)
        if(!this->empty[i]) empty = false;
    return empty;
}

template<class T>
void Queue<T>::show(){
    for(int i=0; i<this->levels; ++i){
        cout << i << ":" << "{";
        for(int j=0; j<this->dim; ++j){
            cout << this->queue[i][j] << ", ";
        }
        cout << "} " << "EMPTY = " << this->empty[i] << " FULL = " << this->full[i] << "| NEXT PUSH = " << this->push_next[i] << " NEXT POP = " << this->pop_next[i] << " | NEXT PUSH BLOCK = " << (this->push_block[i]+1)%THREADS << " NEXT PUSH WAKEUP = " << (this->push_wakeup[i]+1)%THREADS << endl;
    }
    cout << "NEXT POP BLOCK: " << (this->pop_block+1)%THREADS << " | NEXT POP WAKEUP: " << (this->pop_wakeup+1)%THREADS << endl;
    cout << "------------------" << endl;
    cout << "------------------" << endl;
}

//Strategia di sincronizzazione: paradigma PRODUCER/CONSUMER
template<class T>
T Queue<T>::pop () {
    //Come prima cosa veririchiamo che la coda non sia vuota.
    if(this->reliability){
        sem_wait(&this->mutex_popblock);
        this->pop_block = (this->pop_block+1)%THREADS;
        int index = this->pop_block;
        sem_post(&this->mutex_popblock);
        //Le 4 istruzioni di cui sopra servono a prevenire una race condition molto particolare.
        sem_wait(&this->sem_empty[index]); //Per come è implementata la soluzione, se la coda è completamente vuota questa wait sarà bloccante.
        //Se c'è qualcosa da prelevare allora proviamo a prendere il mutex
    }
    
    sem_wait(&this->mutex);
    if(!this->reliability){
        //Controlliamo che la coda non sia vuota solo in modalità BEST EFFORT in quanto in modalità RELIABILITY è garantito dall'utilizzo della sincronizzazione.
        if(isEmpty()){
            cout << "Coda vuota!" << endl;
            sem_post(&this->mutex);
            return __null;
        }
    }

    cout << endl;
    cout << "POPPER IN" << endl;
    show();

    //Ora andiamo a cercare a quale livello della coda abbiamo il primo elemento da prelevare.
    int priority=0;
    while(priority < this->levels && this->empty[priority]) 
        ++priority;
    
    //Ora preleviamo l'elemento
    T ret = this->queue[priority][this->pop_next[priority]];
    this->queue[priority][this->pop_next[priority]] = -1;
    this->pop_next[priority] = (++this->pop_next[priority])%this->dim;
    --this->tot[priority];
        
    //Aggiorniamo eventuali informazioni sullo svuotamento utili per lo più al debug.
    if (this->pop_next[priority] == this->push_next[priority]) {
        //Se dopo una pop i due puntatori coincidono, allora l'array è vuoto
        this->empty[priority] = true;
    }
    if (this->full[priority]) { 
        //Se dopo una pop full è true allora devo cambiarlo
        this->full[priority] = false;
    }

    if(this->reliability){
        this->push_wakeup[priority] = (this->push_wakeup[priority]+1)%THREADS; //Aggiorniamo i contatori per il risveglio FIFO dei pusher prima di liberare il mutex per evitare RC.
        //Intanto noi abbiamo prelevato un elemento dalla coda, quindi possiamo dare la possibilità ad un pusher di non sospendersi sul semaforo.
        sem_post(&this->sem_full[priority][this->push_wakeup[priority]]); //Se c'era un pusher bloccato lo sveglieremo, altriementi quando arriverà troverà il semaforo a 1.
    }
    //Ora ci occupiamo di liberare opportunamente il mutex.

    cout << "POPPER OUT" << endl;
    show();
    cout << endl << endl;

    sem_post(&this->mutex);
    return ret;
}
template<class T>
void Queue<T>::push(T element, int priority) {
    if(priority >= this->levels || priority < 0){
        cout << "Impossibile eseguire la PUSH. Priorità specificata non valida." << endl;
        return;
    }
    if(this->reliability){
        //Come prima cosa verifichiamo che la coda non sia piena
        sem_wait(&this->mutex_pushblock);
        this->push_block[priority] = (this->push_block[priority]+1)%THREADS;
        int index = this->push_block[priority];
        sem_post(&this->mutex_pushblock);
        //Le 4 istruzioni di cui sopra servono a prevenire una race condition molto particolare.
        sem_wait(&this->sem_full[priority][index]);
        //Se c'è uno spazio per inserire, allora prendiamo il mutex.
    }

    sem_wait(&this->mutex);
    if(!this->reliability){
        //Controlliamo che la coda non sia piena solo in modalità BEST EFFORT in quanto in modalità RELIABILITY è garantito dall'utilizzo della sincronizzazione.
        if(this->full[priority]){
            cout << "Coda piena!" << endl;
            sem_post(&this->mutex);
            return;
        }
    }

    cout << endl;
    cout << "PUSHER IN" << endl;
    show();

    //Ora inseriamo l'elemento.
    this->queue[priority][this->push_next[priority]] = element;
    this->push_next[priority] = (this->push_next[priority] + 1) % this->dim;
    ++this->tot[priority];
        
    //Aggiorniamo eventuali informazioni sul riempimento utili al debug.
    if (this->pop_next[priority] == this->push_next[priority]) {
        //Se dopo una push i due puntatori coincidono, allora l'array è pieno
        this->full[priority] = true;                                           
    }
    if (this->empty[priority]) {
        //Se dopo una push empty è true lo devo cambiare
        this->empty[priority] = false;         
    }

    if(this->reliability){
        this->pop_wakeup = (this->pop_wakeup+1)%THREADS;
        //Intanto noi abbiamo inserito un elemento nella coda, quindi possiamo dare la possibilità ad un popper di non sospendersi sul semaforo.
        sem_post(&this->sem_empty[this->pop_wakeup]);
    }
    //Ora ci occupiamo di liberare opportunamente il mutex.

    cout << "PUSHER OUT" << endl;
    show();
    cout << endl << endl;

    sem_post(&this->mutex);
    return;
}