from compat import xrange
import util

from math import sin, cos, sqrt
import optparse
import time

@fields({'x':float, 'y':float, 'z':float})
class Point(object):

    def __init__(self, i:float):
        self.x = x = sin(i)
        self.y = cos(i) * 3
        self.z = (x * x) / 2

    def __repr__(self:Point)->str:
        return "<Point: x=%s, y=%s, z=%s>" % (self.x, self.y, self.z)

    def normalize(self:Point):
        x = self.x
        y = self.y
        z = self.z
        norm = sqrt(x * x + y * y + z * z)
        self.x /= norm
        self.y /= norm
        self.z /= norm

    def maximize(self:Point, other:Point)->Point:
        self.x = self.x if self.x > other.x else other.x
        self.y = self.y if self.y > other.y else other.y
        self.z = self.z if self.z > other.z else other.z
        return self


def maximize(points):
    next = points[0]
    for p in points[1:]:
        next = next.maximize(p)
    return next

def benchmark(n):
    points = [Point(i) for i in xrange(n)]
    for p in points:
        p.normalize()
    return maximize(points)

POINTS = 100000

def main(arg, timer):
    # XXX warmup
    
    times = []
    for i in xrange(arg):
        t0 = timer()
        o = benchmark(POINTS)
        tk = timer()
        times.append(tk - t0)
    return times
    
if __name__ == "__main__":
    parser = optparse.OptionParser(
        usage="%prog [options]",
        description="Test the performance of the Float benchmark")
    util.add_standard_options_to(parser)
    options, args = parser.parse_args()

    util.run_benchmark(options, 1, main)
