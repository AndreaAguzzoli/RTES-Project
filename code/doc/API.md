# GENERIC_QUEUE API

### Class Generic_Queue

* **PARAMETRI**:
	* **FIFO (static const int, public)**: variabile di valore 0 utile per essere passata come parametro del costruttore.
	* **FIXED_PRIORITY (static const int, public)**: variabile di valore 1 utile per essere passata come parametro del costruttore.
	* **DYNAMIC_PRIORITY (static const int, public)**: variabile di valore 2 utile per essere passata come parametro del costruttore.
	* **type (int, private)**: indentifica il tipo di coda (0=FIFO, 1=STATIC PRIORITY, 2=DYNAMIC PRIORITY).
	* **levels (int, private)**: identifica il numero di livelli di priorità per le code a priorità fisse e dinamiche.
* **COSTRUTTORI**:
	* **Generic_Queue(int type)**: crea l'oggetto di tipo Generic_Queue e: se type=0 inizializza una coda FIFO implementata con buffer circolare di dimensione fissa di default, altrimenti è necessario usare i setter degli opportuni parametri per completare l'implementazione della coda a priorità multiple (vedi setLevels())
	* **Generic\_Queue(int type, size\_t dim)**: crea l'oggetto di tipo Generic_Queue e: se type=0 inizializza una coda FIFO implementata con buffer circolare di dimensione passata come secondo parametro, altrimenti è necessario usare i setter degli opportuni parametri per completare l'implementazione della coda a priorità multiple (vedi setLevels())
	* **Generic_Queue(int type, int levels)**: inizializza una coda a priorità fisse o dinamiche (in funzione del parametro type) con un numero di livelli specificato come secondo parametro. Ogni coda dello stesso livello verrà a sua volta gestita FIFO, implementata come buffer circolare con dimensione fissa di default.
	* **Generic\_Queue(int type, int levels, size\_t dim)**: accetta come primo parametro solamente 1 o 2 (FIXED o DYNAMIC priority) ed inizializza una coda del tipo specificato con "levels" livelli di priorità ognuno dei quali gestito FIFO ed implementato con buffer circolare di dimensione specificata come terzo parametro.
* **METODI**
	* **setType(int type)**: cambia il tipo di coda solo se questa è vuota.
	* **setLevels(int levels)**: cambia il numero di livelli per le code a priorità fisse o dinamiche. Se $new\\_levels\lt old\\_levels$ viene eliminata la coda di priorità più bassa a patto che questa sia vuota.
	* **getType()**: ritorna il tipo di coda (0=FIFO, 1=STATIC PRIORITY, 2=DYNAMIC PRIORITY).
	* **getLevels()**: ritorna il numero di livelli (se la coda è FIFO ritorna 0).