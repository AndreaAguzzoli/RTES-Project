#include <iostream>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
using namespace std;

#ifndef FIFO
#define FIFO 0
#define MULTIPLE 1
#define THREADS 200
#endif



int type;
int righe;
int colonne;
int* pop_next; //indica il primo elemento occupato
int* push_next; //indica l'ultimo elemento occupato
int* push_block;
int* pop_block;
int* tot; //indica il numero di elementi presenti nell'array per capire se è pieno o vuoto
int dim;
int** myArray;
bool* empty;
bool* full;
sem_t mutex; //Per accedere in maniera mutuamente esclusiva alla coda.
sem_t* sem_empty; //Semaforo su cui si blocca chi vuole leggere ma la coda è vuota.
sem_t* sem_full; //matrice di semafori

void showArray();


int pop (int priority) {

    sem_wait(&mutex);
    if(priority >= righe || priority < -1){
        cout << "Impossibile eseguire la POP. Priorità specificata non valida." << endl;
        sem_post(&mutex);
        return -1;
    }
    if (priority == -1) {
        if (type == FIFO) {
            ++priority;
        }
        else {
            //code multiple default (posiziona sulla prima coda non vuota se c'è)
            do {
                ++priority;
                if (priority > righe) {
                    //le code sono tutte vuote (si deve attendere)
                    priority = -1;
                    break;
                }
            } while (empty[priority]);
        }
    }
    else {
        if (type == FIFO) {
            cout << "Non è possibile specificare il grado di priorità in una coda FIFO";
            sem_post(&mutex);
            return -2;
        }
    }

    if (priority == -1) {
        // tutte le code sono vuote (PUSH non possibile)
        cout << "Non è stato possibile effettuare la POP perhè tutte le code sono vuote.\n";
        sem_post(&mutex);
        return -3;
    }
    else if (empty[priority]) {
        //coda FIFO vuota, POP impossibile
        cout << "Non è stato possibile effettuare la POP perhè la coda è vuota.\n";
        sem_post(&mutex);
        return -4;
    }

    int ret = myArray[priority][pop_next[priority]];
    myArray[priority][pop_next[priority]] = -1;
    pop_next[priority] = (pop_next[priority] + 1) % dim;
    --tot[priority];        
    if (pop_next[priority] == push_next[priority]) {
        //Se dopo una pop i due puntatori coincidono, allora l'array è vuoto
        empty[priority] = true;
    }
    else if(full[priority]){
        full[priority] = false;
    }

    showArray();
    
    sem_post(&mutex);
    
    return ret;
}


int push (int element, int priority) {

    sem_wait(&mutex);
    if(priority >= righe || priority < -1){
        cout << "Impossibile eseguire la PUSH. Priorità specificata non valida." << endl;
        sem_post(&mutex);
        return -1;
    }
    else if(element == NULL){
        cout << "Si sta tentando di inserire un elemento NULL." << endl;
        sem_post(&mutex);
        return -2;
    }
    if(priority == -1){
        //Consideriamo la coda gestita come FIFO
        if(type != FIFO){
            cout << "Non è possibile inserire un elemento in una coda multipla senza specificare il livello di priorità." << endl;
            sem_post(&mutex);
            return -3;
        }
        ++priority; //Priorità 0.
    }
    else {
        //valore di priorità specificato
        if(type == FIFO){
            cout << "Non è possibile inserire un elemento in una coda FIFO specificando il livello di priorità." << endl;
            sem_post(&mutex);
            return -4;
        }
    }

    if(full[priority]){
        //La coda è piena (PUSH non possibile)
        cout << "Non è possibile eseguire la PUSH perchè la coda è piena." << endl;
        sem_post(&mutex);
        return -5;
    }
    
    myArray[priority][push_next[priority]] = element;
    push_next[priority] = (push_next[priority] + 1) % dim;
    ++tot[priority];
    if (pop_next[priority] == push_next[priority]) {
        //Se dopo una push i due puntatori coincidono, allora l'array è pieno
        full[priority] = true;
    }
    if (empty[priority]) {
        empty[priority] = false;         
    }

    showArray();

    sem_post(&mutex);

    return 0;
}



void showArray () {
    cout << "myArray = { ";
    for (int i=0; i<righe; i++) {
        for (int j=0; j<colonne; j++) {
            if ((j+1)!=colonne) {
                cout << myArray[i][j] << ", ";
            }
            else {
                if ((i+1)!=righe){
                    cout << myArray[i][j] << " } ; --> pop_next=" << pop_next[i] << " ; push_next=" << push_next[i] << " ; tot=" << tot[i] << " ; empty=" << empty[i] << " ; full=" << full[i] << "\n          { ";
                }
                else {
                    cout << myArray[i][j] << " } ; --> pop_next=" << pop_next[i] << " ; push_next=" << push_next[i] << " ; tot=" << tot[i] << " ; empty=" << empty[i] << " ; full=" << full[i] << " } ;\n";
                }
            }
        }
    }
}


void *routine (void *arg) {
	int *pi = (int *)arg;
    
    int popOrPush = rand() % 2 + 1;
    if (popOrPush ==2) {
        //int r = rand() % righe;
        cout << "iniziato il thread " << *pi << "-esimo (PUSH)\n";
        sleep(1);
        //code MULTIPLE        
        //push(*pi+10, r);

        //coda FIFO
        push(*pi+1,-1);
    }
    else if (popOrPush == 1) {
        cout << "iniziato il thread " << *pi << "-esimo (POP)\n";
        sleep(1);
        //code MULTIPLE
        //int r = rand() % righe;
        //int res = pop(1);

        //coda FIFO
        pop(-1);
    }
    /*sem_wait(&mutex);
    showArray();
    sem_post(&mutex);*/
    cout << "terminato il thread\n";
	pthread_exit((void *) pi);
}

int main (){

    pthread_t *threads;
    int *taskKids;
    int *p;

    //input righe
    cout << "inserisci il numero di righe della matrice\n";
    cin >> righe;
    
    if (righe == 1) {
        type = FIFO;
    }
    else {
        type = MULTIPLE;
    }

    //input colonne
    cout << "inserisci il numero di colonne della matrice\n";
    cin >> colonne;
    dim = colonne;

    // allocazione di un puntatore per ogni riga
    myArray = new int*[righe];
    // allocazione dell'array che contiene ogni riga
    for (int i=0; i<righe; i++) {
        myArray[i] = new int[colonne];
    }

    // elementi preimpostati a -1
    for (int i=0; i<righe; i++) {
        for (int j=0; j<colonne; j++) {
            myArray[i][j] = -1;
        }
    }

    pop_next = (int*)malloc(righe*sizeof(int));
    push_next = (int*)malloc(righe*sizeof(int));
    tot = (int*)malloc(righe*sizeof(int));
    empty = (bool*)malloc(righe*sizeof(bool));
    full = (bool*)malloc(righe*sizeof(bool));
    push_block = (int*)malloc(righe*sizeof(int));
    pop_block = (int*)malloc(righe*sizeof(int));

    sem_empty = (sem_t*)malloc(righe*sizeof(sem_t));
    sem_full = (sem_t*)malloc(righe*sizeof(sem_t*));
    sem_init(&mutex, 0, 1);
    threads = (pthread_t *) malloc(THREADS* sizeof(pthread_t));
    taskKids = (int *)malloc(THREADS * sizeof(int));

    for(int i=0; i<righe; i++){
        sem_init(&sem_empty[i], 0, 0);
        sem_init(&sem_full[i], 0, 0);
        pop_next[i] = 0;
        push_next[i] = 0;
        tot[i] = 0; 
        empty[i] = true;
        full[i] = false;   
        push_block[i] = 0;
        pop_block[i] = 0;
    }

    showArray();
    
    for (int i=0; i < THREADS; i++) {
        taskKids[i] = i;
		pthread_create(&threads[i], NULL, routine, (void *) (&taskKids[i]));
	}

	for (int i=0; i < THREADS; i++) {
        int ris;
        pthread_join(threads[i], (void**) & p);
        ris= *p;
        printf("MAIN: Pthread %d-esimo restituisce %d\n", i, ris);
    }

    // deallocazione delle singole righe
    for (int i=0; i<righe; i++) {
        delete[] myArray[i];
    }
    // deallocazione dei puntatori alle righe
    delete[] myArray;   

    return 0;
}