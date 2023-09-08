#ifndef QUEUE_H
#define QUEUE_H

#include<pthread.h>
#include<semaphore.h>
using namespace std;

//Macro per identificare il tipo di gestione della coda
#ifndef FIFO
#define FIFO 0
#endif
#ifndef FIXED_PRIORITY
#define FIXED_PRIORITY 1
#endif
#ifndef RELIABILITY
#define RELIABILITY 1
#endif
#ifndef DYNAMIC_PRIORITY
#define DYNAMIC_PRIORITY 2
#endif
#ifndef BEST_EFFORT
#define BEST_EFFORT 0
#endif
#ifndef DIM
#define DIM 100
#endif
#ifndef THREADS
#define THREADS 1000
#endif

template<class T>
class Queue{
    public:
    /**
     * I costruttori senza parametri o i cui parametri hanno tutti un valore di default specificato sono detti "DEFAULT o ZERO CONSTRUCTOR".
     * Nel nostro caso il costruttore di default è Queue(int, int, size_t) in cui viene associato il valore di default ad i parametri non specificati.
     * È fortemente dipendente dalla posizione nel senso che:
     * Queue<int> q; --> Queue(FIFO, 1, DIM);
     * Queue<int> q(1) --> Queue(1, 1, DIM);
     * Queue<int> q(1, 5) --> Queue(1, 5, DIM);
     * Queue<int> q(1, 5, 80) --> Queue(1, 5, 80);
     * Non può essere specificato il parametro dim senza aver prima definito i due parametri che lo precedono.
     * 
     * Spiegato ciò vieniamo al perché ho aggiunto il secondo costruttore. Se si vuole una coda di multipli livelli è intuititvo che il parametro levels vada
     * specificato. L'unico caso in cui si può desiderare specificare il parametro dim senza gli altri due è quando si vuole una coda FIFO con una certa
     * dimensione non di default. Quindi il secondo costruttore copre proprio questo caso: genera una coda FIFO della dimensione specificata come parametro.
     * Attenzione a castare l'input!
     * Queue<int> q(10); --> NO, viene chiamato il costruttore di default perché 10 viene considerato int.
     * Queue<int> q((size_t)10) --> SI
     * **/

        Queue(int gest = BEST_EFFORT, int type = FIFO, int levels = 1, size_t dim = DIM);

        Queue(size_t dim, int gest = BEST_EFFORT);

        void setLevels(int);

        int getLevels(); //Ritorna il numero di livelli della coda (che coincide con la priorità massima)
        int getType(); //Ritorna il tipo di coda

        T pop();/*
        Estrae il primo elemento della coda NON VUOTA di più alta priorità.
        */
        void push(T element, int priority = -1);/*
        Inserisce l'elemento passato nella coda di priorità specificata (se a code multiple). Per utilizzare la push sulla coda FIFO, NON SPECIFICARE la priorità, oppure usare 0.
        */

    private:
        void show();
        bool generalEmpty(); //Ritorna TRUE se tutti livelli di priorità sono completamente vuoti; FALSE altrimenti.

        bool* empty;
        bool* full;/*
        EMPTY e FULL sono due array lunghi quanto il numero di livelli di priorità presenti. Ogni posizione indica se la coda del corrispondente livello di priorità è piena o vuota.
        */

        int type;
        int gest;
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
        sem_t* sem_empty; //Semaforo su cui si blocca chi vuole leggere ma la coda è vuota.
        sem_t** sem_full; /*
        È una matrice perché i pushers si bloccano selettivamente su uno specifico livello di priorità. È questa matrice è l'unica soluzione che mi è venuta in mente per un risveglio
        selettivo sia in termini di livello di priorità che per mantenere l'ordine FIFO.
        */
};
#endif