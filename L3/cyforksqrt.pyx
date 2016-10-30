# file: cyforksqrt.pyx

cdef extern from "math.h":
    double sqrt(double m)


# uses C function of sqrt()
def sqrtm(var):
    return sqrt(var)


# method of Heron to calculate the square root of any number
def sqrt2(double var):
    cdef double s = 1.0
    cdef int t = 14
    cdef int loops = 100
    for x in range(loops):
        s = 0.5 * (s + (var/s))
    erg = '{:.{}f}'.format(s, t)
    return erg
