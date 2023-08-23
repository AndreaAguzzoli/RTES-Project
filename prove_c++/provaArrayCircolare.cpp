#include <iostream>
#include <stdlib.h>
using namespace std;

int* head; //indica il primo elemento occupato
int* tail; //indica l'ultimo elemento occupato
int* numElementi; //indica il numero di elementi presenti nell'array per capire se è pieno o vuoto
int dim;
int* myArray;
bool empty = true, full = false;

int pop () {
    int result;
    try {
        if (numElementi != 0) {     // se la coda non è vuota allora ...
            result = myArray[head[0]];
            head[0] = (head[0] + 1) % dim;
            numElementi[0]--;
            if (numElementi[0] == 0) 
                empty = true;
            else if ((numElementi[0] != dim) && (full == true)){
                full = false;
            }
        }
        else {
            throw -1;
        }
    }
    catch (...) {
        cout << "ERRORE: La coda è vuota e non è possibile eseguire la POP.\n";
    }
    return result;
}


void push (int elemento) {
    try {            
        if ( numElementi[0] != dim ) { // se l'array non è pieno allora ...
            tail[0] = (tail[0] + 1) % dim;
            myArray[tail[0]] = elemento;            
            numElementi[0]++;
            if (numElementi[0] == dim) 
                full = true;
            else if ((numElementi[0] != 0) && (empty == true)){
                empty = false;
            }
        }
        else {
            throw -1;
        }
    }
    catch (...) {
        cout << "ERRORE: La coda è piena, quindi non è possibile eseguire la PUSH.\n";
    }
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

int main (){
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
    while (j < 20) {
        push(j+10);
        cout << "j=" << j << " --> ";
        showArray();
        if ((j%3) == 0) {
            int r = pop();
            cout << "j=" << j << " --> la POP ha estratto il valore r=" << r << endl;
        }
        j++;
    }

    return 0;
}