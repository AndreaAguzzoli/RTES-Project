# QUEUE API

### Queue.h

* **MACRO**:
	* **FIFO**$\rightarrow$ macro di valore 0 utile per essere passata come parametro del costruttore.
	* **FIXED_PRIORITY**$\rightarrow$ macro di valore 1 utile per essere passata come parametro del costruttore.
	* **DYNAMIC_PRIORITY**$\rightarrow$ macro di valore 2 utile per essere passata come parametro del costruttore.
	* **DIM**$\rightarrow$ macro di valore 100 utilizzata come dimensione di default all'interno dei costruttori.
	* **THREADS**$\rightarrow$ macro di valore 2000 usata per stimare il numero massimo di thread che si possono bloccare contemporaneamente. Da modificare al bisogno.
* **PARAMETRI**:
	* **queue(T\*\*, private)**:$\rightarrow$ coda gestita come matrice. Ogni riga (riga 0, massima priorità) è implementata come buffer circolare ed elementi della stessa priorità sono gestiti FIFO.
	* **type (int, private)**$\rightarrow$ identifica il tipo di coda (0=FIFO, 1=STATIC PRIORITY, 2=DYNAMIC PRIORITY, vedi macros).
	* **levels (int, private)**$\rightarrow$ identifica il numero di livelli di priorità per le code multiple.
	* **dim(int, private)**$\rightarrow$ identifica la dimensione della coda di ogni livello di priorità.
	* **pop_next(int\*, private)**$\rightarrow$ pop_next[i] contiene l'indice dell'elemento da prelevare dalla coda di priorità i-esima. *(Il prelievo sarà: queue[i][pop_next[i])*
	* **push_next(int\*, private)**$\rightarrow$ push_next[i] contiene l'indice del primo spazio libero della coda di priorità i-esima. *(l'inserimento sarà: queue[i][push_next[i])*
	* **empty(bool\*, private**$\rightarrow$ empty[i] è TRUE se la coda di i-esima priorità è vuota.
	* **full(bool\*, private**$\rightarrow$ full[i] è TRUE se la coda di i-esima priorità è piena.
	* **tot(int\*, private**$\rightarrow$ tot[i] indica quanti elementi sono presenti all'interno della coda di i-esima priorità.
	* **pop_block(int, private)**$\rightarrow$ indica quanti thread sono bloccati per effetto sospensivo di una POP.
	* **push_block(int\*, private)**$\rightarrow$ push_block[i] indica quanti thread sono bloccati per effetto sospensivo di una PUSH sulla coda di i-esima priorità.
	* **mutex(sem_t, private)**$\rightarrow$ semaforo per garantire l'accesso mutuamente esclusivo alla coda.
	* **sem_empty(sem_t\*, private)**$\rightarrow$ semaforo su cui si bloccano i thread nel tentativo di fare una POP con coda vuota.
	* **sem_full(sem_t\**, private)**$\rightarrow$ semaforo su cui si bloccano i thread nel tentativo di fare una POP con coda piena. Si tratta di una matrice per permettere il risveglio FIFO dei thread su ogni singolo livello di priorità.
* **COSTRUTTORI**:
	* **Queue(int type=FIFO, int levels=1, size_t dim=DIM)**$\rightarrow$ crea l'oggetto di tipo Queue. I valori di default portano alla creazione di una coda FIFO, mentre specificando i singoli parametri è possibile ottenere una gestione a code multiple.
	* **Queue(int type, size\_t dim)**$\rightarrow$ crea l'oggetto di tipo Queue con gestione FIFO e dimensione specificata. *(Nota: se si passa come parametro la macro DIM si ottiene lo stesso risultato del costruttore precedente con i parametri di default)*
* **METODI**
	* **setLevels(int levels)**$\rightarrow$ cambia il numero di livelli per le code a priorità fisse o dinamiche. Se $new\\_levels\lt old\\_levels$ viene eliminata la coda di priorità più bassa a patto che questa sia vuota.
	* **getType()**$\rightarrow$ ritorna il tipo di coda (0=FIFO, 1=STATIC PRIORITY, 2=DYNAMIC PRIORITY, vedi macros).
	* **getLevels()**$\rightarrow$ ritorna il numero di livelli di priorità.