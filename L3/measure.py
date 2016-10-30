#! /usr/bin/env python3 -tt

import cyforksqrt
import forksqrt
import time
import math


# executes m_funtime with parameters as specific functions and
# default value 5.0 to calculate the square root and execution time
def main():
    var_value = 5.0
    m_funtime('cyforksqrt.sqrtm', cyforksqrt.sqrtm, var_value)
    m_funtime('cyforksqrt.sqrt2', cyforksqrt.sqrt2, var_value)
    m_funtime('forksqrt.sqrt2', forksqrt.sqrt2, var_value, False)
    m_funtime('Math.sqrt', math.sqrt, var_value)


# prints the execution time and the result for a function
def m_funtime(name, fun, *args):
    time_ms = 1000000
    s_time = time.time()
    result = fun(*args)
    e_time = time.time() - s_time
    print('---------{}(5.0)---------\nResult: {}\nExecution Time: {}ms'
          .format(name, result, e_time * time_ms))


if __name__ == "__main__":
    main()
