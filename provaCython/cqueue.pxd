cdef extern from "queue.cpp":
#    pass

#cdef extern from "queue.h":
    cdef cppclass operazioni:
        int somma(int a, int b)
        int differenza(int a, int b)
        int moltiplicazione(int a, int b)
        double divisione(int a, int b)
        int modulo(int a, int b)





