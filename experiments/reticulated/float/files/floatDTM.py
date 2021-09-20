from retic.runtime import *
from retic.transient import *
from retic.typing import *

def check9(val):
    try:
        val.run_benchmark
        return val
    except:
        raise CheckError(val)

def check2(val):
    try:
        val.z
        return val
    except:
        raise CheckError(val)

def check8(val):
    try:
        val.parse_args
        return val
    except:
        raise CheckError(val)

def check0(val):
    try:
        val.x
        return val
    except:
        raise CheckError(val)

def check3(val):
    try:
        val.maximize
        return val
    except:
        raise CheckError(val)

def check5(val):
    try:
        val.normalize
        return val
    except:
        raise CheckError(val)

def check4(val):
    try:
        val.maximize
        val.__repr__
        val.normalize
        return val
    except:
        raise CheckError(val)

def check6(val):
    try:
        val.OptionParser
        return val
    except:
        raise CheckError(val)

def check1(val):
    try:
        val.y
        return val
    except:
        raise CheckError(val)

def check7(val):
    try:
        val.add_standard_options_to
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
        return ('<Point: x=%s, y=%s, z=%s>' % (check0(self).x, check1(self).y, check2(self).z))
    __repr__ = check_type_function(__repr__)

    def normalize(self):
        x = check0(self).x
        y = check1(self).y
        z = check2(self).z
        norm = check_type_function(sqrt)((((x * x) + (y * y)) + (z * z)))
        self.x = (check0(self).x / norm)
        self.y = (check1(self).y / norm)
        self.z = (check2(self).z / norm)
    normalize = check_type_function(normalize)

    def maximize(self, other):
        self.x = (check0(self).x if (check0(self).x > check0(other).x) else check0(other).x)
        self.y = (check1(self).y if (check1(self).y > check1(other).y) else check1(other).y)
        self.z = (check2(self).z if (check2(self).z > check2(other).z) else check2(other).z)
        return self
    maximize = check_type_function(maximize)
Point = check_type_class(Point, ['maximize', '__init__', '__repr__', 'normalize'])

def maximize(points):
    next = points[0]
    for p in points[1:]:
        next = check_type_function(check3(next).maximize)(p)
    return next
maximize = check_type_function(maximize)

def benchmark(n):
    points = check_type_list([check4(Point(i)) for i in check_type_function(xrange)(n)])
    for p in check_type_list(points):
        check_type_function(check5(p).normalize)()
    return maximize(points)
benchmark = check_type_function(benchmark)
POINTS = 100000

def main(arg, timer):
    times = []
    for i in check_type_function(xrange)(arg):
        t0 = check_type_function(timer)()
        o = benchmark(POINTS)
        tk = check_type_function(timer)()
        check_type_void(check_type_function(times.append)((tk - t0)))
    return times
main = check_type_function(main)
if (__name__ == '__main__'):
    parser = check_type_function(check6(optparse).OptionParser)(usage='%prog [options]', description='Test the performance of the Float benchmark')
    check_type_function(check7(util).add_standard_options_to)(parser)
    (options, args) = check_type_tuple(check_type_function(check8(parser).parse_args)(), 2)
    check_type_function(check9(util).run_benchmark)(options, 1, main)
