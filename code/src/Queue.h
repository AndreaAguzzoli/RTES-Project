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
        Queue(int = FIFO); //Costruttore(type) con valore di default FIFO (ovvero 0)
        Queue(int = FIFO, size_t = DIM); //Costruttore(type, dim) con valori di default FIFO (ovvero 0) e DIM
        Queue(int = FIFO, int = 1); //Costruttore(type, levels) con valori di default FIFO (ovvero 0) e 1
        Queue(int = FIFO, int = 1, size_t = DIM); //Costruttore(type, levels, dim) con valori di default FIFO (ovvero 0), 1 e DIM

        void setType(int); //Setter del parametro type per cambiare tipo di coda (funziona solamente se la coda è vuota)
        void setLevels(int); //Setter del parametro levels per cambiare il numero di livelli di priorità (funziona solamente se la coda è vuota e se il nuovo numero di livelli è minore del precedente, viene eliminata la coda con la priorità più bassa)

        int getType(); //Ritorna il tipo di coda.
        int getLevels(); //Ritorna il numero di livelli della coda (che coincide con la priorità massima)

    private:
        int type;
        int levels=1;
        smt **queue;
        int dim=DIM;
}
#endif