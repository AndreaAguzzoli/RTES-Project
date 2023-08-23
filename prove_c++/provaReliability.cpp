/*gcc provaReliability.cpp -pthread -lstdc++ -o provaReliability*/

#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

sem_t mutex; // semaforo che va inizializzato a 1
sem_t semPush, semPop; // semafori che vanno inizializzato a 0
int pushBloccati = 0, popBloccati = 0;

int* head; //indica il primo elemento occupato
int* tail; //indica l'ultimo elemento occupato
int* numElementi; //indica il numero di elementi presenti nell'array per capire se è pieno o vuoto
int dim;
int* myArray;
bool empty = true, full = false;
 
void showArray();

void startPush () {
    sem_wait(&mutex);
    /*per garantire l'inserimento del dato all'interno della coda, il thread si blocca su un semaforo apposito se non ci sono le condizioni di inserimento (coda piena), altrimenti prosegue al suo inserimento. Questo garantisce che il dato verrà inserito nel momento in cui il thread verrà risvegliato perchè ci sono nuovamente condizioni favorevoli all'inserimento (coda non più piena).*/
    if (numElementi[0] != dim) {
        // ci sono le condizioni (coda NON piena)
        sem_post(&semPush);
    }
    else {
        pushBloccati++;        
    }
    sem_post(&mutex);

    sem_wait(&semPush);
}

void endPush () {
    sem_wait(&mutex);
    /* se ci sono estrazioni bloccate ne sblocco una per prima per evitare la starvation delle POP, se non ci sono e invece ci sono inserimenti bloccati sblocco uno di loro. */
    if (popBloccati > 0) {
        popBloccati--;
        sem_post(&semPop);
    } 
    else if (pushBloccati > 0) {
        pushBloccati--;
        sem_post(&semPush);
    }
    sem_post(&mutex);
}

void startPop () {
    sem_wait(&mutex);
    /*per garantire l'estrazione del dato all'interno della coda, il thread si blocca su un semaforo apposito se non ci sono le condizioni di inserimento (coda vuota), altrimenti prosegue alla sua estrazione. Questo garantisce che il dato verrà estratto nel momento in cui il thread verrà risvegliato perchè ci sono nuovamente condizioni favorevoli all'estrazione (coda non più vuota).*/
    if (numElementi[0] != 0) {
        // ci sono le condizioni (coda NON vuota)
        sem_post(&semPop);
    }
    else {
        popBloccati++;        
    }
    sem_post(&mutex);

    sem_wait(&semPop);
}

void endPop () {
    sem_wait(&mutex);
    /* se ci sono inserimenti bloccati ne sblocco uno per primo per evitare la starvation delle PUSH, se non ci sono e invece ci sono estrazioni bloccate sblocco una di loro. */
    if (pushBloccati > 0) {
        pushBloccati--;
        sem_post(&semPush);
    } 
    else if (popBloccati > 0) {
        popBloccati--;
        sem_post(&semPop);
    }
    sem_post(&mutex);
}


void push (int element) {
    startPush();

    sem_wait(&mutex);
    sleep(2);
    if ( numElementi[0] != dim ) { // se l'array non è pieno allora ...
        tail[0] = (tail[0] + 1) % dim;
        myArray[tail[0]] = element;
        ++numElementi[0];
        // controllo se aggiornare i valori delle variabili 'empty' e 'full'
        if (numElementi[0] == dim) 
            full = true;
        else if ((numElementi[0] != 0) && (empty == true)){
            empty = false;
        }
        showArray();
    }
    sem_post(&mutex);

    endPush();
}


int pop () {
    startPop();
    int result;
    sem_wait(&mutex);
    if (numElementi[0] != 0) {     // se la coda non è vuota allora ...
        result = myArray[head[0]];
        head[0] = (head[0] + 1) % dim;
        --numElementi[0];
        // controllo se aggiornare i valori delle variabili 'empty' e 'full'
        if (numElementi[0] == 0) 
            empty = true;
        else if ((numElementi[0] != dim) && (full == true)){
            full = false;
        }
        showArray();
    }
    sem_post(&mutex);

    endPop();
    return result;
}

void showArray () {
    cout << "myArray = { ";
    for (int i=0; i<dim; i++) {
        if ((i+1)!=dim) {
            cout << myArray[i] << ", ";
        }
        else {
            cout << myArray[i] << " } ; \nhead[0]=" << head[0] << " ; tail[0]=" << tail[0] << " ; numElementi[0]=" << numElementi[0] << " ; empty=" << empty << " ; full=" << full << "\n\n";
        }
    }
}

/*----------------------------------------------------------------------*/

void *routine (void *arg) {
	int *pi = (int *)arg;
	int j = 0;
    cout << "iniziato il thread\n";
    while (j < 4) {
        push(j+10);
        cout << "j=" << j << " --> ";
        //showArray();
        if ((j%3) == 0) {
            int r = pop();
            cout << "j=" << j << " --> la POP ha estratto il valore r=" << r << endl;
        }
        j++;
    }
    cout << "terminato il thread\n";

	pthread_exit((void *) pi);
}

int main(){
	pthread_t *threads;
    int *taskKids;
    int *p;
	int i;
    dim = 10;
    myArray = (int*)malloc(dim*sizeof(int));
    head = (int*)malloc(1*sizeof(int));
    tail = (int*)malloc(1*sizeof(int));
    numElementi = (int*)malloc(1*sizeof(int));
    head[0] = 0;
    tail[0] = -1;
    numElementi[0] = 0;
    showArray();
    int j = 0;

	threads = (pthread_t *) malloc(3* sizeof(pthread_t));
    taskKids = (int *) malloc(3 * sizeof(int));
    sem_init(&mutex, 0, 1);
    sem_init(&semPop, 0, 0);
    sem_init(&semPush, 0, 0);

	for(i=0;i<10;i++){
        taskKids[i] = i;
		pthread_create(&threads[i], NULL, routine, (void *) (&taskKids[i]));
	}

	for (i=0; i < 3; i++){
        int ris;
        pthread_join(threads[i], (void**) & p);
        ris= *p;
        printf("MAIN: Pthread %d-esimo restituisce %d\n", i, ris);
    }

    sleep(2);
    return 0;
}






