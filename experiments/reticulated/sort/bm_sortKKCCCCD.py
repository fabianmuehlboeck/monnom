from compat import xrange
import time
import util
import optparse

@fields({first:ListNode,size:int})
class Lst(object):
  def __init__(self:Lst,val:int):
    self.first=ListNode(val)
    self.size=1

  def Add(self:Lst, val:int):
    newNode = ListNode(val)
    newNode.prev=self.first.prev
    newNode.next=self.first
    self.first.prev=newNode
    newNode.prev.next=newNode
    self.size=self.size+1

  def GetIterator(self:Lst)->Dyn:
    return self.MakeIterator(self.first)

  def MakeIterator(self:Lst, node:ListNode)->Dyn:
    return Iterator(self, node)

@fields({value:int,next:ListNode,prev:ListNode})
class ListNode(object):
  def __init__(self:ListNode,v:int):
    self.value=v
    self.next=self
    self.prev=self

@fields({currentNode:ListNode,parent:Lst})
class Iterator(object):
  def __init__(self:Iterator,l:Lst,node:ListNode):
    self.currentNode=node
    self.parent=l

  def MovePrev(self:Iterator)->Bool:
    if(self.currentNode is self.parent.first):
      return False
    self.currentNode=self.currentNode.prev
    return True

  def MoveNext(self:Iterator)->Bool:
    if(self.currentNode.next is self.parent.first):
      return False
    self.currentNode=self.currentNode.next
    return True

  def Current(self:Iterator)->int:
    return self.currentNode.value

  def SetValue(self:Iterator,x:int):
    self.currentNode.value=x

  def Clone(self:Iterator)->Dyn:
    return self.parent.MakeIterator(self.currentNode)

def QuickSort(l):
  loIter=l.GetIterator()
  hiIter=l.GetIterator()
  if(loIter.MoveNext()):
    hiIter.MoveNext()
    lo = 0
    hi = 0
    while(hiIter.MoveNext()):
      hi = hi + 1
    QuickSortRec(loIter, hiIter, lo, hi)

def QuickSortRec(loIter, hiIter, lo, hi):
  if(lo<hi):
    upper=hiIter.Clone()
    lower=loIter.Clone()
    losize=Partition(lower,upper, hi-lo)
    QuickSortRec(loIter, upper, lo, lo+losize-1)
    QuickSortRec(lower, hiIter, lo + losize, hi)

def Partition(loIter, hiIter, distance):
  pivot=loIter.Current()
  losize=0
  while(True):
    while(loIter.Current() < pivot):
      loIter.MoveNext()
      distance=distance-1
      losize=losize+1
    while(hiIter.Current() > pivot):
      hiIter.MovePrev()
      distance=distance-1
    if(distance<0):
      break
    buf=loIter.Current()
    loIter.SetValue(hiIter.Current())
    hiIter.SetValue(buf)
    loIter.MoveNext()
    losize=losize+1
    hiIter.MovePrev()
    distance=distance-2
  return losize

def MakeIntList()->Dyn:
  lst=Lst(5)
  i=0
  while(i<100000):
    num=(i*163841 + 176081) % 122251
    lst.Add(num)
    i=i+1
  return lst

def main(arg, timer):
  # XXX warmup
  times=[]
  for i in xrange(arg):
    mylist=MakeIntList()
    t0 = timer()
    QuickSort(mylist)
    tk = timer()
    times.append(tk - t0)
  return times
    
if __name__ == "__main__":
  parser = optparse.OptionParser(
        usage="%prog [options]",
        description="Test the performance of the Sort benchmark")
  util.add_standard_options_to(parser)
  options, args = parser.parse_args()

  util.run_benchmark(options, 1, main)

