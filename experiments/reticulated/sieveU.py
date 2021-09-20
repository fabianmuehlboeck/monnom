from compat import xrange
import util
import sys

from math import sin, cos, sqrt
import optparse
import time

sys.setrecursionlimit(100000)

class Stream(object):
  def __init__(self,n,rest):
    self.N=n
    self.Rest=rest

def CountFrom(n):
  return Stream(n,lambda : CountFrom(n+1))

def Sift(n,s):
  f = s.N
  while(f%n==0):
    s=s.Rest()
    f=s.N
  return Stream(f,lambda : Sift(n, s.Rest()))

def Sieve(s):
  return Stream(s.N, lambda : Sieve(Sift(s.N, s.Rest())))

def GetPrimes():
  return Sieve(CountFrom(2))

def StreamGet(s, n):
  while(n>0):
    s=s.Rest()
    n=n-1
  return s.N

def main(arg, timer):
  # XXX warmup
  times=[]
  last=-1
  for i in xrange(arg):
    t0 = timer()
    last=StreamGet(GetPrimes(),9999)
    tk = timer()
    times.append(tk - t0)
  print(last)
  return times
    
if __name__ == "__main__":
  parser = optparse.OptionParser(
        usage="%prog [options]",
        description="Test the performance of the Sieve benchmark")
  util.add_standard_options_to(parser)
  options, args = parser.parse_args()

  util.run_benchmark(options, 1, main)

