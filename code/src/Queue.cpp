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
    
    this->sem_empty = (sem_t**)malloc(this->levels*sizeof(sem_t*));
    this->sem_full = (sem_t**)malloc(this->levels*sizeof(sem_t*));
    sem_init(&this->mutex, 1);

    this->push_block = (int*)malloc(this->levels*sizeof(int));
    this->pop_block = (int*)malloc(this->levels*sizeof(int));

    for(int i=0; i<this->levels; ++i){
        this->queue[i] = (T*)malloc(this->dim*sizeof(T));
        this->sem_full[i] = (sem_t*)malloc((THREADS)*sizeof(sem_t));
        this->sem_empty[i] = (sem_t*)malloc((THREADS)*sizeof(sem_t));
        this->empty[i] = true;
        this->full[i] = false;
        this->tot[i] = 0;
        this->pop_next[i] = 0;
        this->push_next[i] = 0;
        this->push_block[i] = 0;
        this->pop_block[i] = 0;

        if(this->type == FIFO)
            break;
    }
    for(int i=0; i<this->levels; ++i){        
        for(int j=0; j<THREADS; ++j) {
            sem_init(&this->sem_empty[i][j], 0, 0);
            sem_init(&this->sem_full[i][j], 0, 0);
        }
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

    this->queue = (T**)malloc(sizeof(T*));
    this->queue[0] = (T*)malloc(this->dim*sizeof(T));

    this->empty = (bool*)malloc(sizeof(bool));
    this->full = (bool*)malloc(sizeof(bool));

    this->pop_next = (int*)malloc(sizeof(int));
    this->push_next = (int*)malloc(sizeof(int));

    this->tot = (int*)malloc(sizeof(int));
    
    this->sem_empty = (sem_t**)malloc(sizeof(sem_t*));
    this->sem_full = (sem_t**)malloc(sizeof(sem_t*));
    sem_init(&this->mutex, 1);

    this->push_block = (int*)malloc(sizeof(int));
    this->pop_block = (int*)malloc(sizeof(int));

    for(int i=0; i<1; ++i){
        this->queue[i] = (T*)malloc(this->dim*sizeof(T));
        this->sem_full[i] = (sem_t*)malloc((THREADS)*sizeof(sem_t));
        this->sem_empty[i] = (sem_t*)malloc((THREADS)*sizeof(sem_t));
        this->empty[i] = true;
        this->full[i] = false;
        this->tot[i] = 0;
        this->pop_next[i] = 0;
        this->push_next[i] = 0;
        this->push_block[i] = 0;
        this->pop_block[i] = 0;

        if(this->type == FIFO)
            break;
    }
    for(int i=0; i<1; ++i){        
        for(int j=0; j<THREADS; ++j) {
            sem_init(&this->sem_empty[i][j], 0, 0);
            sem_init(&this->sem_full[i][j], 0, 0);
        }
    }
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
T Queue<T>::popReliability (int priority) {
    bool flag = false;

    if(priority >= this->levels || priority < -1){
        cout << "Impossibile eseguire la POP. Priorità specificata non valida." << endl;
        return NULL;
    }

    sem_wait(&this->mutex);

    if (this->type == FIFO) {
        if (priority != -1) {
            cout << "Impossibile specificare il livello di priorità in una coda FIFO." << endl;
            return NULL;
        }
        ++priority; //Priorità 0.
    }
    else if(priority == -1){
        //Comportamento di default del metodo per le code multiple.
        flag = true;
        ++priority;
        while(priority < this->levels && this->empty[priority]) 
            ++priority;
        if(priority >= this->levels){
            //Ora è necessario bloccarsi in attesa che qualcuno inserica un valore all'interno della coda.
            do {
                priority = 0;
                ++this->pop_block[priority];
                // mi sospendo sul semaforo relativo al livello di priorità e correttamente in fila
                sem_post(&this->mutex);
                sem_wait(&this->sem_empty[priority][this->pop_block[priority]-1]);
                sem_wait(&this->mutex);  
                //Nel momento in cui si viene risvegliati si ha la certezza che nella coda si dispone di un elemento da poppare.
                while(priority < this->levels && this->empty[priority]) 
                    ++priority;
            } while (priority >= this->levels);
        }
    }

    //se la coda è vuota devo sospendermi --> nel caso di default a code multiple è già garantito il fatto che la coda non è vuota quindi flag mi fa saltare il controllo
    if (this->empty[priority] && !flag) {
        //coda vuota
        do {
            ++this->pop_block[priority];
            // mi sospendo sul semaforo relativo al livello di priorità e correttamente in fila
            sem_post(&this->mutex);
            sem_wait(&this->sem_empty[priority][this->pop_block[priority]-1]);
            sem_wait(&this->mutex);        
        } while (this->empty[priority]); //il do/while è presente per evitare che tra il suo risveglio e l'acquisizione del mutex la coda torni vuota
    }

    T ret = this->queue[priority][this->pop_next[priority]];
    this->pop_next[priority] = (++this->pop_next[priority])%this->dim;
    --this->tot[priority];
        
    if (this->pop_next[priority] == this->push_next[priority]) {
        //Se dopo una pop i due puntatori coincidono, allora l'array è vuoto
        this->empty[priority] = true;
    }
    if (this->full[priority]) { 
        //Se dopo una pop full è true allora devo cambiarlo
        this->full[priority] = false;
    }

    // per evitare la starvation delle PUSH cerco di risvegliare prima una PUSH se c'è, altrimenti sveglio una POP
    // controllo prima nei livelli più alti di priorità
    for (int i = 0; i < this->levels; i++) {
        if (this->push_block[i] > 0) {            
            --this->push_block[i];
            sem_post(&this->sem_full[i][this->push_block[i]]);
            break;
        }
        else if (this->pop_block[i] > 0) {
            --this->pop_block[i];
            sem_post(&this->sem_empty[i][this->pop_block[i]]);
            break;
        }
    }
    
    sem_post(&this->mutex);
    return ret;
}


template<class T>
void Queue<T>::pushReliability (T element, int priority) {
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
        // coda piena
        do {
            ++this->push_block[priority];
            sem_post(&this->mutex);
            sem_wait(&this->sem_full[priority][this->push_block[priority]-1]);
            sem_wait(&this->mutex);            
        } while (this->full[priority]); //il do/while è presente per evitare che tra il suo risveglio e l'acquisizione del mutex la coda torni vuota
    }
    

    this->queue[priority][this->push_next[priority]] = element;
    this->push_next[priority] = (this->push_next[priority] + 1) % this->dim;
    ++this->tot[priority];
        
    if (this->pop_next[priority] == this->push_next[priority]) {
        //Se dopo una push i due puntatori coincidono, allora l'array è pieno
        this->full[priority] = true;                                           
    }
    if (this->empty[priority]) {
        //Se dopo una push empty è true lo devo cambiare
        this->empty[priority] = false;         
    }

    // per evitare la starvation delle POP cerco di risvegliare prima una POP se c'è, altrimenti sveglio una push
    // controllo prima nei livelli più alti di priorità
    for (int i = 0; i < this->levels; i++) {
        if (this->pop_block[i] > 0) {
            --this->pop_block[i];
            sem_post(&this->sem_empty[i][this->pop_block[i]]);
            break;
        }
        else if (this->push_block[i] > 0) {
            --this->push_block[i];
            sem_post(&this->sem_full[i][this->push_block[i]]);
            break;
        }
    }

    sem_post(&this->mutex);
}

template<class T>
T Queue<T>::popBestEffort (int priority) {
    sem_wait(&this->mutex);
    if(priority >= this->levels || priority < -1){
        cout << "Impossibile eseguire la POP. Priorità specificata non valida." << endl;
        sem_post(&this->mutex);
        return NULL;
    }
    if (priority == -1) {
        if (this->type == FIFO) {
            ++priority;
        }
        else {
            //code multiple default (posiziona sulla prima coda non vuota se c'è)
            do {
                ++priority;
                if (priority > this->levels) {
                    //le code sono tutte vuote (si deve attendere)
                    priority = -1;
                    break;
                }
            } while (this->empty[priority]);
        }
    }
    else {
        if (this->type == FIFO) {
            cout << "Non è possibile specificare il grado di priorità in una coda FIFO";
            sem_post(&this->mutex);
            return NULL;
        }
    }

    if (priority == -1) {
        // tutte le code sono vuote (PUSH non possibile)
        cout << "Non è stato possibile effettuare la POP perhè tutte le code sono vuote.\n";
        sem_post(&this->mutex);
        return NULL;
    }
    else if (this->empty[priority]) {
        //coda FIFO vuota, POP impossibile
        cout << "Non è stato possibile effettuare la POP perhè la coda è vuota.\n";
        sem_post(&this->mutex);
        return NULL;
    }

    T ret = this->queue[priority][this->pop_next[priority]];
    this->pop_next[priority] = (this->pop_next[priority] + 1) % dim;
    --this->tot[priority]; 

    if (this->pop_next[priority] == this->push_next[priority]) {
        //Se dopo una pop i due puntatori coincidono, allora l'array è vuoto
        this->empty[priority] = true;
    }
    else if(this->full[priority]){
        this->full[priority] = false;
    }
    
    sem_post(&mutex);
    
    return ret;
}

template<class T>
void Queue<T>::pushBestEffort (T element, int priority) {
    sem_wait(&this->mutex);
    if(priority >= this->levels || priority < -1){
        cout << "Impossibile eseguire la PUSH. Priorità specificata non valida." << endl;
        sem_post(&this->mutex);
        return;
    }
    else if(element == NULL){
        cout << "Si sta tentando di inserire un elemento NULL." << endl;
        sem_post(&this->mutex);
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
    else {
        //valore di priorità specificato
        if(this->type == FIFO){
            cout << "Non è possibile inserire un elemento in una coda FIFO specificando il livello di priorità." << endl;
            sem_post(&this->mutex);
            return;
        }
    }

    if(this->full[priority]){
        //La coda è piena (PUSH non possibile)
        cout << "Non è possibile eseguire la PUSH perchè la coda è piena." << endl;
        sem_post(&this->mutex);
        return;
    }
    
    this->queue[priority][this->push_next[priority]] = element;
    this->push_next[priority] = (this->push_next[priority] + 1) % dim;
    ++this->tot[priority];

    if (this->pop_next[priority] == this->push_next[priority]) {
        //Se dopo una push i due puntatori coincidono, allora l'array è pieno
        this->full[priority] = true;
    }
    if (this->empty[priority]) {
        this->empty[priority] = false;         
    }

    sem_post(&this->mutex);
}

int main(void){
    Queue<int>  q, q2((size_t)150), q3(FIXED_PRIORITY, 12);
    q3.setLevels(10);
}