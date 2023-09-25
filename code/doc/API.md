# QUEUE API

### Queue.h

* **MACRO**:
	* **BEST\_EFFORT**$\rightarrow$ macro di valore false utile per essere passata come parametro del costruttore.
	* **RELIABILITY**$\rightarrow$ macro di valore true utile per essere passata come parametro del costruttore.
	* **DIM**$\rightarrow$ macro di valore 100 utilizzata come dimensione di default all'interno dei costruttori.
	* **THREADS**$\rightarrow$ macro di valore 1000 usata per stimare il numero massimo di thread che si possono bloccare contemporaneamente. Da modificare al bisogno.
* **PARAMETRI**:
	* **queue(T\*\*, private)**:$\rightarrow$ coda gestita come matrice. Ogni riga (riga 0, massima priorità) è implementata come buffer circolare ed elementi della stessa priorità sono gestiti FIFO.
	* **levels (int, private)**$\rightarrow$ identifica il numero di livelli di priorità.
	* **dim(int, private)**$\rightarrow$ identifica la dimensione della coda di ogni livello di priorità.
	* **reliability(bool, private)**$\rightarrow$ identifica se la coda deve lavorare in QoS reliability o best effort.
	* **pop_next(int\*, private)**$\rightarrow$ pop\_next[i] contiene l'indice dell'elemento da prelevare dalla coda di priorità i-esima. *(Il prelievo sarà: queue[i][pop_next[i])*
	* **push_next(int\*, private)**$\rightarrow$ push\_next[i] contiene l'indice del primo spazio libero della coda di priorità i-esima. *(l'inserimento sarà: queue[i][push_next[i])*
	* **empty(bool\*, private**$\rightarrow$ empty[i] è TRUE se la coda di i-esima priorità è vuota.
	* **full(bool\*, private**$\rightarrow$ full[i] è TRUE se la coda di i-esima priorità è piena.
	* **tot(int\*, private**$\rightarrow$ tot[i] indica quanti elementi sono presenti all'interno della coda di i-esima priorità.
	* **pop_block(int, private)**$\rightarrow$ contiene l'indice del semaforo su cui tentare la wait per una POP. *(Il controllo sarà: sem\_wait(&this->sem\_empty[this->pop\_block]))*
	* **push_block(int\*, private)**$\rightarrow$ push\_block[i] contiene l'indice del semaforo su cui tentare la wait per l'i-esimo livello di priorità. *(Il controllo sarà: sem\_wait(&this->sem\_full[priority][this->push\_block[priority]]))*
	* **pop_wakeup(int, private)**$\rightarrow$ contiene l'indice del prossimo thread da svegliare di quelli sospesi da una POP. *(il risveglio sarà: sem\_post(&this->sem\_empty[this->pop\_wakeup]))*
	* **push_wakeup(int\*, private)**$\rightarrow$ push\_wakeup[i]contiene l'indice del prossimo thread da svegliare di quelli sospesi da una PUSH sull'i-esimo livello di priorità. *(il risveglio sarà: sem\_post(&this->sem\_full[priority][this->push\_wakeup[priority]]))*
	* **mutex(sem_t, private)**$\rightarrow$ semaforo per garantire l'accesso mutuamente esclusivo alla coda.
	* **mutex_popblock(sem_t, private)**$\rightarrow$ semaforo per garantire l'accesso mutuamente esclusivo alla variabile pop_block.
	* **mutex_pushblock(sem_t, private)**$\rightarrow$ semaforo per garantire l'accesso mutuamente esclusivo all'array push_block.
	* **sem_empty(sem_t\*, private)**$\rightarrow$ semafori su cui si bloccano i thread nel tentativo di fare una POP con coda vuota. Si tratta di un array per permettere il risveglio FIFO dei thread.
	* **sem_full(sem_t\**, private)**$\rightarrow$ semafori su cui si bloccano i thread nel tentativo di fare una PUSH con coda piena. Si tratta di una matrice per permettere il risveglio FIFO dei thread su ogni singolo livello di priorità.
* **COSTRUTTORI**:
	* **Queue(bool reliability=RELIABILITY, int levels=3, size_t dim=DIM)**$\rightarrow$ Costruttore che di default genera delle CODE MULTIPLE su 3 livelli di priorità. Ogni livello di priorità è gestito FIFO ed ha dimensione DIM.
* **METODI**
	* **getLevels()**$\rightarrow$ ritorna il numero di livelli di priorità.
	* **getDim()**$\rightarrow$ ritorna la dimensione dei livelli di priorità.
	* **getQueue()**$\rightarrow$ ritorna il puntatore alla coda.
	* **isReliability()**$\rightarrow$ ritorna TRUE se la coda è gestita in modo tale che rispetti la QoS Reliability; FALSE se Best Effort.
	* **isEmpty()**$\rightarrow$ ritorna TRUE se tutti i livelli di priorità sono vuoti; FALSE altrimenti.
	* **show()**$\rightarrow$ stampa su STDOUT lo stato della coda.
	* **pop()**$\rightarrow$ ritorna il primo elemento disponibile nella coda di più alta priorità non vuota. L'esecuzione della pop è potenzialmente bloccante.
	* **push(T element, int priority=0)**$\rightarrow$ inserisce l'elemento passato come parametro nella coda di priorità specificata (default priorità massima). L'esecuzione della push è potenzialmente bloccante.
* **FUNZIONI**
	* **dim()**$\rightarrow$ ritorna la macro DIM.
	* **best_effort()**$\rightarrow$ ritorna la macro BEST_EFFORT.
	* **reliability()**$\rightarrow$ ritorna la macro RELIABILITY.
	* **threads()**$\rightarrow$ ritorna la macro THREADS.
	* **int\_to\_sizet()**$\rightarrow$ ritorna l'int passato come parametro castato in size\_t.