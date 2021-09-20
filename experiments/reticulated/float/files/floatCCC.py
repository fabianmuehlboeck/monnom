from retic.runtime import *
from retic.transient import *
from retic.typing import *

def check4(val):
    try:
        val.add_standard_options_to
        return val
    except:
        raise CheckError(val)

def check1(val):
    try:
        val.maximize
        return val
    except:
        raise CheckError(val)

def check5(val):
    try:
        val.parse_args
        return val
    except:
        raise CheckError(val)

def check0(val):
    try:
        val.y
        val.maximize
        val.z
        val.x
        val.__repr__
        val.normalize
        return val
    except:
        raise CheckError(val)

def check3(val):
    try:
        val.OptionParser
        return val
    except:
        raise CheckError(val)

def check6(val):
    try:
        val.run_benchmark
        return val
    except:
        raise CheckError(val)

def check2(val):
    try:
        val.normalize
        return val
    except:
        raise CheckError(val)
from compat import xrange
import util
from math import sin, cos, sqrt
import optparse
import time


class Point(retic_actual(object)):

    def __init__(self, i):
        self.x = x = check_type_function(sin)(i)
        self.y = (check_type_function(cos)(i) * 3)
        self.z = ((x * x) / 2)
    __init__ = check_type_function(__init__)

    def __repr__(self):
        check0(self)
        return ('<Point: x=%s, y=%s, z=%s>' % (check_type_float(self.x), check_type_float(self.y), check_type_float(self.z)))
    __repr__ = check_type_function(__repr__)

    def normalize(self):
        check0(self)
        x = check_type_float(self.x)
        y = check_type_float(self.y)
        z = check_type_float(self.z)
        norm = check_type_function(sqrt)((((x * x) + (y * y)) + (z * z)))
        self.x = check_type_float((check_type_float(self.x) / norm))
        self.y = check_type_float((check_type_float(self.y) / norm))
        self.z = check_type_float((check_type_float(self.z) / norm))
    normalize = check_type_function(normalize)

    def maximize(self, other):
        check0(self)
        check0(other)
        self.x = (check_type_float(self.x) if (check_type_float(self.x) > check_type_float(other.x)) else check_type_float(other.x))
        self.y = (check_type_float(self.y) if (check_type_float(self.y) > check_type_float(other.y)) else check_type_float(other.y))
        self.z = (check_type_float(self.z) if (check_type_float(self.z) > check_type_float(other.z)) else check_type_float(other.z))
        return self
    maximize = check_type_function(maximize)
Point = check_type_class(Point, ['__init__', '__repr__', 'normalize', 'maximize'])

def maximize(points):
    check_type_list(points)
    next = check0(points[0])
    for p in check_type_list(points[1:]):
        check0(p)
        next = check_type_function(check1(next).maximize)(p)
    return check0(next)
maximize = check_type_function(maximize)

def benchmark(n):
    check_type_int(n)
    points = check_type_list([check0(Point(i)) for i in check_type_function(xrange)(n)])
    for p in check_type_list(points):
        check_type_function(check2(p).normalize)()
    return check0(maximize(points))
benchmark = check_type_function(benchmark)
POINTS = 100000

def main(arg, timer):
    check_type_int(arg)
    times = []
    for i in check_type_function(xrange)(arg):
        t0 = check_type_function(timer)()
        o = check0(benchmark(POINTS))
        tk = check_type_function(timer)()
        check_type_void(check_type_function(times.append)((tk - t0)))
    return times
main = check_type_function(main)
if (__name__ == '__main__'):
    parser = check_type_function(check3(optparse).OptionParser)(usage='%prog [options]', description='Test the performance of the Float benchmark')
    check_type_function(check4(util).add_standard_options_to)(parser)
    (options, args) = check_type_tuple(check_type_function(check5(parser).parse_args)(), 2)
    check_type_function(check6(util).run_benchmark)(options, 1, main)
