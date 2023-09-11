#ifndef QUEUE_H
#define QUEUE_H

#include<pthread.h>
#include<semaphore.h>
using namespace std;

//Macro per identificare il tipo di gestione della coda
#ifndef RELIABILITY
#define RELIABILITY true
#endif
#ifndef BEST_EFFORT
#define BEST_EFFORT false
#endif
#ifndef DIM
#define DIM 100
#endif
#ifndef THREADS
#define THREADS 1000
#endif

//Servono per utilizzare le macro anche nel modulo python
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

template<class T>
class Queue{
    public:
    /**
     * I costruttori senza parametri o i cui parametri hanno tutti un valore di default specificato sono detti "DEFAULT o ZERO CONSTRUCTOR".
     * Nel nostro caso il costruttore di default è Queue(bool, int, size_t) in cui viene associato il valore di default ad i parametri non specificati.
     * È fortemente dipendente dalla posizione nel senso che:
     * Queue<int> q; --> Queue(RELIABILITY, 3, DIM);
     * Queue<int> q(BEST_EFFORT) --> Queue(BEST_EFFORT, 3, DIM);
     * Queue<int> q(BEST_EFFORT, 5) --> Queue(BEST_EFFORT, 5, DIM);
     * Queue<int> q(BEST_EFFORT, 5, 30) --> Queue(BEST_EFFORT, 5, 30);
     * Non può essere specificato il parametro dim senza aver prima definito i due parametri che lo precedono.
     **/

        Queue(bool reliability = RELIABILITY, int levels = 3, size_t dim = DIM);
        ~Queue();

        int getLevels(); //Ritorna il numero di livelli della coda (che coincide con la priorità massima)
        size_t getDim(); //Ritorna la dimensione delle code
        T** getQueue(); //Ritorna il puntatore alla coda
        bool isReliability(); //Rtorna se la coda è in Reliability o Best Effort
        bool isEmpty(); //Ritorna TRUE se tutti livelli di priorità sono completamente vuoti; FALSE altrimenti.
        void show(); //Stampa le caratteristiche della coda ed il suo contenuto 


        T pop();/*
        Estrae il primo elemento della coda NON VUOTA di più alta priorità.
        */
        void push(T element, int priority = 0);/*
        Inserisce l'elemento passato nella coda di priorità specificata (se a code multiple). Per utilizzare la push sulla coda FIFO, NON SPECIFICARE la priorità, oppure usare 0.
        */

    private:

        bool* empty;
        bool* full;/*
        EMPTY e FULL sono due array lunghi quanto il numero di livelli di priorità presenti. Ogni posizione indica se la coda del corrispondente livello di priorità è piena o vuota.
        */

        bool reliability; //Tipo di gestione della coda
        int levels;
        T **queue;
        int dim;

        int *pop_next; //Indici degli elementi per ogni livello di priorità da poppare
        int *push_next; //Indice del primo spazio libero per ogni livello di priorità; ovvero l'indice della posizione in cui eventualmente inserire un nuovo elemento.
        int *tot; /*
        Numero di elementi per ogni livello di priorità (utile per debugging, quindi lasciato anche agli utilizzatori). Si potrebbe utilizzare al posto di empty e full,
        ma per coerenza alla letteratura si mantengono anche quelli.
        */

        int* push_block; //Array che contiene in ogni posizione l'indice del semaforo su cui si dovrà bloccare il thread.
        int* push_wakeup; //Array che contiene in ogni posizione l'indice del prossimo thread da svegliare.
        int pop_block; //Indice del semaforo si cui si dovrà bloccare il thread.
        int pop_wakeup; //Indice del prossimo thread da svegliare.
        sem_t mutex; //Per accedere in maniera mutuamente esclusiva alla coda.
        sem_t mutex_pushblock; //Sono due semafori che servono a garantire mutua esclusione nell'accesso agli indici pop_block e push_block.
        sem_t mutex_popblock;
        sem_t* sem_empty; //Semaforo su cui si blocca chi vuole prelevare ma la coda è vuota.
        sem_t** sem_full; /*
        È una matrice perché i pushers si bloccano selettivamente su uno specifico livello di priorità. È questa matrice è l'unica soluzione che mi è venuta in mente per un risveglio
        selettivo sia in termini di livello di priorità che per mantenere l'ordine FIFO.
        */
};
#endif