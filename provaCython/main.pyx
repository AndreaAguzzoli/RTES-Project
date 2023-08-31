
cimport cqueue


cdef class MyInterface:
    cdef cqueue.Interface* op

    # if you want some vars you need --> def __init__(self, int some_var, char* some_string):




    #def stampaMatrice (self) :
    #    return self.op.stampaMatrice()
        
    def createMatrix (self, int righe, int colonne) :
        return self.op.createMatrix(righe,colonne)

    def MAIN (self, int r, int c) :
        return self.op.MAIN(r,c)

    #def push (self, int element, int priority) :
      #  return self.op.push(element, priority)




    def __init__(self):
        self.op = new cqueue.Interface()
        if self.op == NULL:
            raise MemoryError('Not enough memory.')

    def __del__(self):
        del self.op

#    property some_var:
#        def __get__(self):
#            return self.cobj.some_var
#        def __set__(self, int var):
#            self.cobj.some_var = var