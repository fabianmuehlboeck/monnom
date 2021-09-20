from retic.runtime import *
from retic.transient import *
from retic.typing import *

def check1(val):
    try:
        val.N
        return val
    except:
        raise CheckError(val)

def check3(val):
    try:
        val.OptionParser
        return val
    except:
        raise CheckError(val)

def check0(val):
    try:
        val.setrecursionlimit
        return val
    except:
        raise CheckError(val)

def check2(val):
    try:
        val.Rest
        return val
    except:
        raise CheckError(val)

def check4(val):
    try:
        val.parse_args
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
    return Stream(n, (lambda : CountFrom((n + 1))))
CountFrom = check_type_function(CountFrom)

def Sift(n, s):
    check_type_int(n)
    f = check1(s).N
    while ((f % n) == 0):
        s = check_type_function(check2(s).Rest)()
        f = check1(s).N
    return Stream(f, (lambda : Sift(n, check_type_function(check2(s).Rest)())))
Sift = check_type_function(Sift)

def Sieve(s):
    return Stream(check1(s).N, (lambda : Sieve(Sift(check_type_int(check1(s).N), check_type_function(check2(s).Rest)()))))
Sieve = check_type_function(Sieve)

def GetPrimes():
    return Sieve(CountFrom(2))
GetPrimes = check_type_function(GetPrimes)

def StreamGet(s, n):
    check_type_int(n)
    while (n > 0):
        s = check_type_function(check2(s).Rest)()
        n = (n - 1)
    return check1(s).N
StreamGet = check_type_function(StreamGet)

def main(arg, timer):
    times = []
    last = (- 1)
    for i in check_type_function(xrange)(arg):
        t0 = check_type_function(timer)()
        last = StreamGet(GetPrimes(), 9999)
        tk = check_type_function(timer)()
        check_type_void(check_type_function(times.append)((tk - t0)))
    check_type_function(print)(last)
    return times
main = check_type_function(main)
if (__name__ == '__main__'):
    parser = check_type_function(check3(optparse).OptionParser)(usage='%prog [options]', description='Test the performance of the Sieve benchmark')
    check_type_function(util.add_standard_options_to)(parser)
    (options, args) = check_type_tuple(check_type_function(check4(parser).parse_args)(), 2)
    check_type_function(util.run_benchmark)(options, 1, main)
