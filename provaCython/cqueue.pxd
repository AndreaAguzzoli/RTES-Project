cdef extern from "queue.cpp":
#    pass

#cdef extern from "queue.h":
    cdef cppclass Interface:
        int createMatrix (int righe, int colonne)
        #int stampaMatrice ()
        int MAIN (int r, int c)
        #int push (int element, int priority)




