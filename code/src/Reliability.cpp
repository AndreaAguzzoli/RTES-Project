#include <iostream>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
using namespace std;

#ifndef FIFO
#define FIFO 0
#define MULTIPLE 1
#define THREADS 20
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
sem_t** sem_empty; //Semaforo su cui si blocca chi vuole leggere ma la coda è vuota.
sem_t** sem_full; //matrice di semafori

void showArray();

int pop (int priority = -1) {

    bool flag = false;

    if(priority >= righe || priority < -1){
        cout << "Impossibile eseguire la POP. Priorità specificata non valida." << endl;
        return -1;
    }

    sem_wait(&mutex);

    if (type == FIFO) {
        if (priority != -1) {
            cout << "Impossibile specificare il livello di priorità in una coda FIFO." << endl;
            return -1;
        }
        ++priority; //Priorità 0.
    } 
    else if (priority == -1) {
        //Comportamento di default del metodo per le code multiple.
        flag = true;
        ++priority;
        while(priority < righe && empty[priority]) 
            ++priority;
        if(priority >= righe){
            //Ora è necessario bloccarsi in attesa che qualcuno inserica un valore all'interno della coda.
            do {
                priority = 0;
                cout << "thread(POP) si SOSPENDE per CODA VUOTA\n";
                ++pop_block[priority];
                // mi sospendo sul semaforo relativo al livello di priorità e correttamente in fila
                sem_post(&mutex);
                sem_wait(&sem_empty[priority][pop_block[priority]-1]);
                sem_wait(&mutex);  
                //Nel momento in cui si viene risvegliati si ha la certezza che nella coda si dispone di un elemento da poppare.
                while(priority < righe && empty[priority]) 
                    ++priority;
            } while (priority >= righe);
            cout << "thread(POP) si RISVEGLIA\n";
        }
    } 

    //se la coda è vuota devo sospendermi --> nel caso di default a code multiple è già garantito il fatto che la coda non è vuota quindi flag mi fa saltare il controllo
    if (empty[priority] && !flag) {
        //coda vuota
        do {
            cout << "thread(POP) si SOSPENDE per CODA VUOTA\n";
            ++pop_block[priority];
            // mi sospendo sul semaforo relativo al livello di priorità e correttamente in fila
            sem_post(&mutex);
            sem_wait(&sem_empty[priority][pop_block[priority]-1]);
            sem_wait(&mutex);        
        } while (empty[priority]); //il do/while è presente per evitare che tra il suo risveglio e l'acquisizione del mutex la coda torni vuota
        cout << "thread(POP) si RISVEGLIA\n";
    }


    int ret = myArray[priority][pop_next[priority]];
    myArray[priority][pop_next[priority]] = -1;
    pop_next[priority] = (pop_next[priority] + 1) % dim;
    --tot[priority];

    if (pop_next[priority] == push_next[priority]) {
        //Se dopo una pop i due puntatori coincidono, allora l'array è vuoto
        empty[priority] = true;
    }
    if (full[priority]) { 
        //Se dopo una pop full è true allora devo cambiarlo
        full[priority] = false;
    }

    cout << "POP --> ";
    showArray();        
    
    // per evitare la starvation delle PUSH cerco di risvegliare prima una PUSH se c'è, altrimenti sveglio una POP
    // controllo prima nei livelli più alti di priorità
    for (int i = 0; i < righe; i++) {
        if (push_block[i] > 0) {            
            --push_block[i];
            cout << "Sto per risvegliare una PUSH, sem_full[" << i << "][" << push_block[i] << "]" << endl;
            sem_post(&sem_full[i][push_block[i]]);
            //aggiorno la fila
            /*for (int j = 0; j < (push_block[i] - 1); j++) {
                if ((j+1) < THREADS) {  // caso limite da evitare per non leggere fuori dall'array
                    sem_full[i][j] = sem_full[i][j+1];
                }
            }*/
            break;
        }
        else if (pop_block[i] > 0) {
            --pop_block[i];
            sem_post(&sem_empty[i][pop_block[i]]);
            cout << "Sto per risvegliare una POP, sem_empty[" << i << "][" << pop_block[i] << "]" << endl;
            //aggiorno la fila
            /*for (int j = 0; j < (pop_block[i] - 1); j++) {
                if ((j+1) < THREADS) {  // caso limite da evitare per non leggere fuori dall'array
                    sem_empty[i][j] = sem_empty[i][j+1];
                }
            }*/
            break;
        }
    }
    
    sem_post(&mutex);
    return ret;
}


void push (int element, int priority = -1) {
    if(priority >= righe || priority < -1){
        cout << "Impossibile eseguire la PUSH. Priorità specificata non valida." << endl;
        return;
    }
    else if(element == NULL){
        cout << "Si sta tentando di inserire un elemento NULL." << endl;
        return;
    }

    sem_wait(&mutex);

    if(priority == -1){
        //Consideriamo la coda gestita come FIFO
        if(type != FIFO){
            cout << "Non è possibile inserire un elemento in una coda multipla senza specificare il livello di priorità." << endl;
            sem_post(&mutex);
            return;
        }
        ++priority; //Priorità 0.
    }

    //se la coda è piena devo sospendermi
    if(full[priority]){
        // coda piena
        do {
            ++push_block[priority];
            cout << "thread(PUSH) si SOSPENDE per CODA PIENA --> sem_full[" << priority << "][" << push_block[priority]-1 << "]" << endl;
            sem_post(&mutex);
            sem_wait(&sem_full[priority][push_block[priority]-1]);
            sem_wait(&mutex);            
        } while (full[priority]); //il do/while è presente per evitare che tra il suo risveglio e l'acquisizione del mutex la coda torni vuota
        cout << "thread(PUSH) si RISVEGLIA\n";
    }

    myArray[priority][push_next[priority]] = element;
    push_next[priority] = (push_next[priority] + 1) % dim;
    ++tot[priority];

    if (pop_next[priority] == push_next[priority]) {
        //Se dopo una push i due puntatori coincidono, allora l'array è pieno
        full[priority] = true;
    }
    if (empty[priority]) {
        //Se dopo una push empty è true lo devo cambiare
        empty[priority] = false;         
    }

    cout << "PUSH --> ";
    showArray();
    
    // per evitare la starvation delle POP cerco di risvegliare prima una POP se c'è, altrimenti sveglio una push
    // controllo prima nei livelli più alti di priorità
    for (int i = 0; i < righe; i++) {
        if (pop_block[i] > 0) {
            --pop_block[i];
            cout << "Sto per risvegliare una POP, sem_empty[" << i << "][" << pop_block[i] << "]" << endl;
            sem_post(&sem_empty[i][pop_block[i]]);
            //aggiorno la fila
            /*for (int j = 0; j < (pop_block[i] - 1); j++) {
                if ((j+1) < THREADS) {  // caso limite da evitare per non leggere fuori dall'array
                    sem_empty[i][j] = sem_empty[i][j+1];
                }
            }*/
            break;
        }
        else if (push_block[i] > 0) {
            --push_block[i];
            cout << "Sto per risvegliare una PUSH, sem_full[" << i << "][" << push_block[i] << "]" << endl;
            sem_post(&sem_full[i][push_block[i]]);
            //aggiorno la fila
            /*for (int j = 0; j < (push_block[i] - 1); j++) {
                if ((j+1) < THREADS) {  // caso limite da evitare per non leggere fuori dall'array
                    sem_full[i][j] = sem_full[i][j+1];
                }
            }*/
            break;
        }
    }

    sem_post(&mutex);
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
    while(1){
        cout << "iniziato il thread\n";
        sleep(1);
        int r = rand() % 10;

        //CODA FIFO
        
        if ((r%2)==0) 
            push(*pi+10);
        else {
            pop();
        }
        

        //CODE MULTIPLE
        //if ((r%2)!=0) 
        //    push(*pi+1,rand()%righe);
            
        //else
        //    pop(rand()%righe);

        //cout << "terminato il thread\n";
    }
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

    sem_empty = (sem_t**)malloc(righe*sizeof(sem_t*));
    sem_full = (sem_t**)malloc(righe*sizeof(sem_t*));
    sem_init(&mutex, 0, 1);
    threads = (pthread_t *) malloc(THREADS* sizeof(pthread_t));
    taskKids = (int *)malloc(THREADS * sizeof(int));

    for(int i=0; i<righe; i++){
        sem_full[i] = (sem_t*)malloc((THREADS)*sizeof(sem_t));
        sem_empty[i] = (sem_t*)malloc((THREADS)*sizeof(sem_t));
        pop_next[i] = 0;
        push_next[i] = 0;
        tot[i] = 0;
        empty[i] = true;
        full[i] = false;
        pop_block[i] = 0;
        push_block[i] = 0;
    }
    for(int i=0; i<righe; ++i){        
        for(int j=0; j<THREADS; ++j) {
            sem_init(&sem_empty[i][j], 0, 0);
            sem_init(&sem_full[i][j], 0, 0);
        }
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