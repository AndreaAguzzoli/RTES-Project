#ifndef QUEUE_H
#define QUEUE_H
using namespace std;

//Macro per identificare il tipo di gestione della coda
#ifndef FIFO
#define FIFO 0
#endif
#ifndef FIXED_PRIORITY
#define FIXED_PRIORITY 1
#endif
#ifndef DYNAMIC_PRIORITY
#define DYNAMIC_PRIORITY 2
#endif
#ifndef DIM
#define DIM 100
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

        Queue(int type = FIFO, int levels = 1, size_t dim = DIM);

        Queue(size_t dim);

        void setLevels(int);

        void setType(int);

        int getLevels(); //Ritorna il numero di livelli della coda (che coincide con la priorità massima)

        int getType(); //Ritorna il tipo di coda.

        void pop(int priority = -1); // estrae l'elemento in testa alla coda FIFO (default) o alla coda relativa ad un certo livello di priorità (code multiple)

        void push(T element, int priority = -1); // inserisce in fondo alla coda FIFO l'elemento indicato (default) o in fondo alla coda relativa ad un certo livello di priorità (code multiple)

    private:
        bool empty=true, full=false;
        int type;
        int levels;
        T **queue;
        int dim;

        /*
        I seguenti 3 interi servono per la gestione dell'array circolare che compone la coda. "head" indica il primo elemento inserito, mentre "tail" indica l'ultimo inserito. Significa che nel momento in cui non ci sono elementi o nel momento in cui ce n'è 1 solo hanno lo stesso valore. Mentre nel momento in cui tail è il precedente di head significa che l'array è pieno. Per distinguere se l'array è pieno o vuoto abbiamo quindi bisogno della variabile "numElementi".
        Quando viene estratto un elemento il valore di "head" si incrementa di una unità, mentre nel momento il cui viene inserito un elemento si incrementa il valore di "tail".
        */
        int* head; //indica il primo elemento occupato
        int* tail; //indica l'ultimo elemento occupato
        int* numElementi; //indica il numero di elementi presenti nell'array per capire se è pieno o vuoto
};
#endif