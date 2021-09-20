from retic.runtime import *
from retic.transient import *
from retic.typing import *

def check1(val):
    try:
        val.Rest
        val.N
        return val
    except:
        raise CheckError(val)

def check0(val):
    try:
        val.setrecursionlimit
        return val
    except:
        raise CheckError(val)

def check3(val):
    try:
        val.parse_args
        return val
    except:
        raise CheckError(val)

def check2(val):
    try:
        val.OptionParser
        return val
    except:
        raise CheckError(val)
from compat import xrange
import util
from math import sin, cos, sqrt
import optparse
import time
check_type_function(check0(sys).setrecursionlimit)(100000)


class Stream(retic_actual(object)):

    def __init__(self, n, rest):
        self.N = n
        self.Rest = rest
    __init__ = check_type_function(__init__)
Stream = check_type_class(Stream, ['__init__'])

def CountFrom(n):
    check_type_int(n)
    return check1(Stream(n, (lambda : check1(CountFrom((n + 1))))))
CountFrom = check_type_function(CountFrom)

def Sift(n, s):
    check_type_int(n)
    check1(s)
    f = check_type_int(s.N)
    while ((f % n) == 0):
        s = check1(check_type_function(s.Rest)())
        f = check_type_int(s.N)
    return check1(Stream(f, (lambda : check1(Sift(n, check1(check_type_function(s.Rest)()))))))
Sift = check_type_function(Sift)

def Sieve(s):
    check1(s)
    return check1(Stream(check_type_int(s.N), (lambda : check1(Sieve(check1(Sift(check_type_int(s.N), check1(check_type_function(s.Rest)()))))))))
Sieve = check_type_function(Sieve)

def GetPrimes():
    return check1(Sieve(check1(CountFrom(2))))
GetPrimes = check_type_function(GetPrimes)

def StreamGet(s, n):
    check1(s)
    check_type_int(n)
    while (n > 0):
        s = check1(check_type_function(s.Rest)())
        n = (n - 1)
    return check_type_int(s.N)
StreamGet = check_type_function(StreamGet)

def main(arg, timer):
    times = []
    last = (- 1)
    for i in check_type_function(xrange)(arg):
        t0 = check_type_function(timer)()
        last = StreamGet(check1(GetPrimes()), 9999)
        tk = check_type_function(timer)()
        check_type_void(check_type_function(times.append)((tk - t0)))
    check_type_function(print)(last)
    return times
main = check_type_function(main)
if (__name__ == '__main__'):
    parser = check_type_function(check2(optparse).OptionParser)(usage='%prog [options]', description='Test the performance of the Sieve benchmark')
    check_type_function(util.add_standard_options_to)(parser)
    (options, args) = check_type_tuple(check_type_function(check3(parser).parse_args)(), 2)
    check_type_function(util.run_benchmark)(options, 1, main)
