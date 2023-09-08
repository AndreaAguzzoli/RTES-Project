#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include"Queue.h"
using namespace std;
#define NULL __null;

//Implementazione costruttori
template<class T>
Queue<T>::Queue(int gest, int type, int levels, size_t dim){
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
    this->gest = gest;

    if(this->type == FIFO && this->levels != 1){
        cout << "Impossibile avere una coda di " << levels << " livelli se gestita FIFO." << endl;
        return;
    }
    if(this->type != FIFO && this->levels == 1){
        cout << "La coda deve avere type=FIFO avendo inserito un solo livello di priorità." << endl;
        return;
    }

    this->queue = (T**)malloc(this->levels*sizeof(T*));

    this->empty = (bool*)malloc(this->levels*sizeof(bool));
    this->full = (bool*)malloc(this->levels*sizeof(bool));

    this->pop_next = (int*)malloc(this->levels*sizeof(int));
    this->push_next = (int*)malloc(this->levels*sizeof(int));

    this->tot = (int*)malloc(this->levels*sizeof(int));
    
    this->sem_empty = (sem_t*)malloc(THREADS*sizeof(sem_t));
    this->sem_full = (sem_t**)malloc(this->levels*sizeof(sem_t*));
    sem_init(&this->mutex, 0, 1);

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

        if(this->type == FIFO)
            break;
    }
    for(int i=0; i<this->levels; ++i){     
        for(int j=0; j<THREADS; ++j) {
            if(j < this->dim)
                sem_init(&this->sem_full[i][j], 0, 1);
            else
                sem_init(&this->sem_full[i][j], 0, 0);
        }
    }
    for(int i=0; i<THREADS; ++i)
            sem_init(&this->sem_empty[i], 0, 0);
}

template<class T>
Queue<T>::Queue(size_t dim, int gest){
    cout << "dim: " << dim << endl;
    cout << "gest: " << gest << endl;
    cout << "THREADS: " << THREADS << endl;
    if(dim <= 0){
        cout << "Impossibile avere una coda di dimensione minore o uguale 0." << endl;
        return;
    }
    this->gest = gest;
    this->type = FIFO;
    this->levels = 1;
    this->dim = dim;

    this->queue = (T**)malloc(sizeof(T*));
    this->queue[0] = (T*)malloc(this->dim*sizeof(T));

    this->empty = (bool*)malloc(sizeof(bool));
    this->full = (bool*)malloc(sizeof(bool));

    this->pop_next = (int*)malloc(sizeof(int));
    this->push_next = (int*)malloc(sizeof(int));

    this->tot = (int*)malloc(sizeof(int));
    
    this->sem_empty = (sem_t*)malloc(THREADS*sizeof(sem_t));
    this->sem_full = (sem_t**)malloc(sizeof(sem_t*));
    sem_init(&this->mutex, 0, 1);

    this->push_block = (int*)malloc(sizeof(int));
    this->push_wakeup = (int*)malloc(sizeof(int));
    this->pop_block = -1; 
    this->pop_wakeup = -1;

    for(int i=0; i<1; ++i){
        this->sem_full[i] = (sem_t*)malloc((THREADS)*sizeof(sem_t));
        this->empty[i] = true;
        this->full[i] = false;
        this->tot[i] = 0;
        this->pop_next[i] = 0;
        this->push_next[i] = 0;
        this->push_block[i] = -1;
        this->push_wakeup[i] = this->dim -1;

        if(this->type == FIFO)
            break;
    }
    for(int i=0; i<1; ++i){     
        for(int j=0; j<THREADS; ++j) {
            if(j < this->dim)
                sem_init(&this->sem_full[i][j], 0, 1);
            else
                sem_init(&this->sem_full[i][j], 0, 0);
        }
    }
    for(int i=0; i<THREADS; ++i)
            sem_init(&this->sem_empty[i], 0, 0);
}

/*void Queue::setLevels(int newlevels){
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
int Queue::getLevels(){
    return this->levels;
}
int Queue::getType(){
    return this->type;
}*/

template<class T>
void Queue<T>::show(){
    cout << "SHOWING ALL..." << endl;
    for(int i=0; i<this->levels; ++i){
        cout << i << ":" << "{";
        for(int j=0; j<this->dim; ++j){
            cout << this->queue[i][j] << ", ";
        }
        cout << "} " << "EMPTY= " << this->empty[i] << " FULL = " << this->full[i] << endl;
        cout << "NEXT SPACE FOR PUSH: " << this->push_next[i] << endl;
        cout << "NEXT ELEMENT FOR POP: " << this->pop_next[i] << endl;
        cout << "------------------" << endl;
        cout << "NEXT SPACE FOR PUSH WILL BLOCK: " << this->push_block[i] << endl;
        cout << "NEXT PUSH WILL WAKEUP: " << this->push_wakeup[i] << endl;
        cout << "------------------" << endl;
    }
    cout << "NEXT SPACE FOR POP WILL BLOCK: " << this->pop_block << endl;
    cout << "NEXT POP WILL WAKEUP: " << this->pop_wakeup << endl;
    cout << "------------------" << endl;
    cout << "------------------" << endl;
}
template<class T>
bool Queue<T>::generalEmpty(){
    bool empty = true;
    for(int i=0; i<this->levels; ++i)
        if(!this->empty[i]) empty = false;
    return empty;
}

//Strategia di sincronizzazione: paradigma PRODUCER/CONSUMER
template<class T>
T Queue<T>::pop () {
    //Come prima cosa veririchiamo che la coda non sia vuota.
    if(this->gest == RELIABILITY){
        this->pop_block = (++this->pop_block)%THREADS;
        sem_wait(&this->sem_empty[this->pop_block]); //Per come è implementata la soluzione, se la coda è completamente vuota questa wait sarà bloccante.
        //Se c'è qualcosa da prelevare allora proviamo a prendere il mutex
    }
    
    sem_wait(&this->mutex);
    if(this->gest == BEST_EFFORT){
        //Controlliamo che la coda non sia vuota solo in modalità BEST EFFORT in quanto in modalità RELIABILITY è garantito dall'utilizzo della sincronizzazione.
        if(generalEmpty()){
            sem_post(&this->mutex);
            return NULL;
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

    if(this->gest == RELIABILITY){
        this->push_wakeup[priority] = (++this->push_wakeup[priority])%THREADS; //Aggiorniamo i contatori per il risveglio FIFO dei pusher prima di liberare il mutex per evitare RC.
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
    if(priority >= this->levels || priority < -1){
        cout << "Impossibile eseguire la PUSH. Priorità specificata non valida." << endl;
        return;
    }

    if(priority == -1){
        //Consideriamo la coda gestita come FIFO
        if(this->type != FIFO){
            cout << "Non è possibile inserire un elemento in una coda multipla senza specificare il livello di priorità." << endl;
            sem_post(&this->mutex);
            return;
        }
        ++priority; //Priorità 0.
    }

    if(this->gest == RELIABILITY){
        //Come prima cosa verifichiamo che la coda non sia piena
        this->push_block[priority] = (++this->push_block[priority])%THREADS;
        sem_wait(&this->sem_full[priority][this->push_block[priority]]);
        //Se c'è uno spazio per inserire, allora prendiamo il mutex.
    }

    sem_wait(&this->mutex);
    if(this->gest == BEST_EFFORT){
        //Controlliamo che la coda non sia piena solo in modalità BEST EFFORT in quanto in modalità RELIABILITY è garantito dall'utilizzo della sincronizzazione.
        if(this->full[priority]){
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

    if(this->gest == RELIABILITY){
        this->pop_wakeup = (++this->pop_wakeup)%THREADS; //Aggiorniamo i contatori per il risveglio FIFO dei popper prima di liberare il mutex per evitare RC.
        //Intanto noi abbiamo inserito un elemento nella coda, quindi possiamo dare la possibilità ad un popper di non sospendersi sul semaforo.
        sem_post(&this->sem_empty[this->pop_wakeup]); //Se c'era un pusher bloccato lo sveglieremo, altriementi quando arriverà troverà il semaforo a 1.
    }
    //Ora ci occupiamo di liberare opportunamente il mutex.

    cout << "PUSHER OUT" << endl;
    show();
    cout << endl << endl;

    sem_post(&this->mutex);
    return;
}