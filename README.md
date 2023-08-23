# RTES-Project
Questo repository contiene il progetto di Aguzzoli e Mercoliano.

# libreria che gestisca oggetti GENERICI

# CODA FIFO	

- costruttore di default che genera una coda FIFO con dimensione limitata ad un certo valore di default 
- overload del costruttore con un parametro in più che indica la dimensione della coda da creare
- Possibilità di shift quando arriva un nuovo elemento e il buffer è pieno oppure shift quando specificato con l'aggiunta di un parametro
- gestione coda nel FIFO
- funzioni PUSH POP E PULL con mutua esclusione

# CODA A PRIORITÀ STATICA 

- con un override del costruttore vengono generate code multiple a priorità statica (una per ogni grado di priorità) ognuna gestita in modo FIFO
- il parametro da inserire sono il numero di gradi di priorità (quindi il numero di code)
- se un valore alto implica un grado di priorità alto o viceversa viene indicato nelle **specifiche della API**
- ?? **ANZIANITA'** cambio priorità sulla base del tempo di stazionamento nella coda (timer che allo scadere causa il cambio di priorità)

- ?? **OPZIONALE** --> unico buffer circolare composto da tanti buffer circolari per semplificare il cambio del valore di priorità

# QoS 

## Best Effort

"Attempt to deliver samples, but may lose them if the network is not robust".

Nel caso in questione si cerca con una PUSH di inserire un elemento all'interno della coda, ma a seconda dello stato in cui si trova la coda, non è detto che l'elemento in questione sia correttamente inserito all'interno della coda.

## Reliability

"Guarantee that samples are delivered, may retry multiple times".

Nel caso in questione quando con una PUSH si vuole inserire un elemento all'interno della coda, viene garantito il suo corretto inserimento all'interno di quest'ultima. Se nello stato attuale della coda questo non è possibile, riprova molteplici volte finchè non ci riesce.

