import cython

cdef class PyMyElement:
    cdef MyElement* thisptr

    def __cinit__(self, name=""):
        print ("cython constructor : %s" % name)
        if name == "":
            self.thisptr = new MyElement()
        else:
            self.thisptr = new MyElement(name)

    def get_name(self):
        return self.thisptr.Name()
