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

    this->queue = (T**)malloc(this->levels*sizeof(T*));
    this->queue[0] = (T*)malloc(this->dim*sizeof(T));

    this->empty = (bool*)malloc(this->levels*sizeof(bool));
    this->full = (bool*)malloc(this->levels*sizeof(bool));

    this->pop_next = (int*)malloc(this->levels*sizeof(int));
    this->push_next = (int*)malloc(this->levels*sizeof(int));

    this->tot = (int*)malloc(this->levels*sizeof(int));
    
    this->sem_empty = (sem_t*)malloc(THREADS*sizeof(sem_t));
    this->sem_full = (sem_t**)malloc(this->levels*sizeof(sem_t*));
    sem_init(&this->mutex, 1);

    this->push_block = (int*)malloc(this->levels*sizeof(int));

    for(int i=0; i<this->levels; ++i){
        this->queue[i] = (T*)malloc(this->dim*sizeof(T));
        this->sem_full[i] = (sem_t*)malloc((THREADS)*sizeof(sem_t))
        this->empty[i] = true;
        this->full[i] = false;
        this->tot[i] = 0;
        this->pop_next[i] = 0;
        this->push_next[i] = 0;
        this->push_block[i] = 0;

        if(this->type == FIFO)
            break;
    }
    for(int i=0; i<THREADS; ++i){
        sem_init(&this->sem_empty[i], 0);
        for(int j=0; j<this->levels; ++j)
            sem_init(&this->sem_full[j][i], 0);
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

    this->push_block = (int*)malloc(this->levels*sizeof(int));
    for(int i=0; i<this->levels; ++i)
        this->push_block[i] = 0;

    this->sem_full = (sem_t**)malloc(sizeof(sem_t*));
    this->sem_full[0] = (sem_t*)malloc(THREADS*sizeof(sem_t));
    this->sem_empty = (sem_t*)malloc(THREADS*sizeof(sem_t));
    for(int i=0; i<THREADS; ++i){
        sem_init(&this->sem_empty, 0);
        sem_init(this->sem_full[0], 0);
    }
    sem_init(&this->mutex, 1);

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
template<calss T>::getLevels(){
    return this->levels;
}

template<class T>
T Queue<T>::pop (int priority) {
    if(priority >= this->levels || priority < -1){
        cout << "Impossibile eseguire la POP. Priorità specificata non valida." << endl;
        return NULL;
    }
    sem_wait(&this->mutex);
    if (this->type == FIFO)
        ++priority; //Priorità 0.
    else if(priority == -1){
        //Comportamento di default del metodo per le code multiple.
        ++priority;
        while(priority < this->levels && this->empty[priority]) 
            ++priority;
        if(priority >= this->levels){
            /*
            Ora è necessario bloccarsi in attesa che qualcuno inserica un valore all'interno della coda.
            */
            priority = 0;
            ++this->pop_block;
            sem_post(&this->mutex);
            sem_wait($this->sem_empty[this->pop_block-1]);
            sem_wait(&this->mutex);
            /*
            Nel momento in cui si viene risvegliati si ha la certezza che nella coda si dispone di un elemento da poppare.
            */
            while(priority < this->levels && this->empty[priority]) 
                ++priority;
        }
    }

    T ret = this->queue[priority][this->pop_next[priority]];
    this->pop_next[priority] = (++this->pop_next[priority])%this->dim;
    --this->tot[priority];
        
    if (this->pop_next[priority] == this->push_next[priority]) 
        //Se dopo una pop i due puntatori coincidono, allora l'array è vuoto
        this->empty[priority] = true;
    if(this->full[priority]){
        this->full[priority] = false;
        if(this->push_block[priority]){
            --this->push_block[priority];
            sem_post($this->sem_full[priority][this->push_block[priority]]);
        }
    }
    
    sem_post(&this->mutex);
    return ret;
}


template<class T>
void Queue<T>::push (T element, int priority) {
    if(priority >= this->levels || priority < -1){
        cout << "Impossibile eseguire la PUSH. Priorità specificata non valida." << endl;
        return;
    }
    else if(element == NULL){
        cout << "Si sta tentando di inserire un elemento NULL." << endl;
        return;
    }

    sem_wait(&this->mutex);

    if(priority == -1){
        //Consideriamo la coda gestita come FIFO
        if(this->type != FIFO){
            cout << "Non è possibile inserire un elemento in una coda multipla senza specificare il livello di priorità." << endl;
            sem_post(&this->mutex);
            return;
        }
        ++priority; //Priorità 0.
    }

    if(this->full[priority]){
        //La coda è piena ed è necessario bloccarsi.
        ++this->push_block[priority];
        sem_post(&this->mutex);
        sem_wait(&this->sem_full[priority][this->push_block[priority]-1]);
        sem_wait(&this->mutex);
        //Se si viene risvegliati si ha la certezza che vi è uno slot in cui inserire l'elemento. Per questo motivo non occorrono controlli.
    }
    

    this->queue[priority][this->push_next[priority]] = element;
    this->push_next[priority] = (this->push_next[priority] + 1) % this->dim;
    ++this->tot[priority];
        
    if (this->pop_next[priority] == this->push_next[priority]) {
        //Se dopo una push i due puntatori coincidono, allora l'array è pieno
        this->full[priority] = true;                                           
    }
    else if(this->empty[priority]){
        this->empty[priority] = false;
        if(this->pop_block){
            --this->pop_block;
            sem_post($this->sem_empty[this->pop_block]);
        } 
            
    }

    sem_post(&this->mutex);
}

int main(void){
    Queue<int>  q, q2((size_t)150), q3(FIXED_PRIORITY, 12);
    q3.setLevels(10);
}