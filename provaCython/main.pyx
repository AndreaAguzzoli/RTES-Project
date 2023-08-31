
cimport cqueue


cdef class MyOperazioni:
    cdef cqueue.operazioni* op

    # if you want some vars you need --> def __init__(self, int some_var, char* some_string):

    def somma (self, int a, int b) : 
        return self.op.somma(a,b)

    def __init__(self):
        self.op = new cqueue.operazioni()
        if self.op == NULL:
            raise MemoryError('Not enough memory.')

    def __del__(self):
        del self.op

#    property some_var:
#        def __get__(self):
#            return self.cobj.some_var
#        def __set__(self, int var):
#            self.cobj.some_var = var