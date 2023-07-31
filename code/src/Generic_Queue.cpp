#include<iostream>
using namespace std;

#ifndef DIM
#define DIM 100
#endif

template <typename smt> //Serve per gestire la coda con i generics

class Generic_Queue{
    private:
        int type;
        int levels;
        smt* queue;
        int dim;

    public:
        static const int FIFO=0;
        static const int FIXED_PRIORITY=0;
        static const int DYNAMIC_PRIORITY=0;
        //Costruttori
        Generic_Queue(int type);
        Generic_Queue(int type, size_t dim);
        Generic_Queue(int type, int levels, size_t dim);
        //Setters
        void setType(int type);
        void setLevels(int levels);
        void insertSpecificItem(smt item, int index);
        //Getters
        int getType();
        int getLevels();
        smt getSpecificItem(int index);
};

template <typename smt>
Generic_Queue<smt>::Generic_Queue(int type){
    this->type=type;
    
    if(type != this->FIFO)
        return;

    this->levels=0;
    this->dim=DIM;
    queue = new smt[this->dim];
}

template <typename smt>
smt Generic_Queue<smt>::getSpecificItem(int index){
    //Vanno aggiunti i controlli sul valore di index, per ora è solo per provare.
    return this->queue[index]; 
}

template <typename smt>
void Generic_Queue<smt>::insertSpecificItem(smt item, int index){
    //Vanno aggiunti i controlli sul valore di index, per ora è solo per provare.
    this->queue[index] = item;
}

int main(){
    cout << "started..." << endl;
    Generic_Queue<int> q(q.FIFO);
    q.insertSpecificItem((int) 10, 0);
    cout << "Valore: " << q.getSpecificItem(0) << endl;
}