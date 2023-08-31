#include "queue.h"

int operazioni::somma (int a, int b) {
    int somma = a+b+a;
    return somma;
}

int operazioni::differenza (int a, int b) {
    int diff = a-b;
    return diff;
}

int operazioni::moltiplicazione (int a, int b) {
    int prodotto = a*b;
    return prodotto;
}

double operazioni::divisione (int a, int b) {
    double quoziente = a/b;
    return quoziente;
}

int operazioni::modulo (int a, int b) {
    int mod = a%b;
    return mod;
}