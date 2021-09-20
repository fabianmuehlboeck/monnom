from compat import xrange
import time
import util
import optparse

class Lst(object):
  def __init__(self,val):
    self.first=ListNode(val)
    self.size=1

  def Add(self, val):
    newNode = ListNode(val)
    newNode.prev=self.first.prev
    newNode.next=self.first
    self.first.prev=newNode
    newNode.prev.next=newNode
    self.size=self.size+1

  def GetIterator(self):
    return self.MakeIterator(self.first)

  def MakeIterator(self, node):
    return Iterator(self, node)

class ListNode(object):
  def __init__(self,v):
    self.value=v
    self.next=self
    self.prev=self

class Iterator(object):
  def __init__(self,l,node):
    self.currentNode=node
    self.parent=l

  def MovePrev(self):
    if(self.currentNode is self.parent.first):
      return False
    self.currentNode=self.currentNode.prev
    return True

  def MoveNext(self):
    if(self.currentNode.next is self.parent.first):
      return False
    self.currentNode=self.currentNode.next
    return True

  def Current(self):
    return self.currentNode.value

  def SetValue(self,x):
    self.currentNode.value=x

  def Clone(self):
    return self.parent.MakeIterator(self.currentNode)

def QuickSort(l:{'Add':Function([String],Void), 'GetIterator':Function([],{'MoveNext':Function([],Bool),'MovePrev':Function([],Bool),'Current':Function([],int),'SetValue':Function([int],Void),'Clone':Function([],Self)})})->Void:
  loIter=l.GetIterator()
  hiIter=l.GetIterator()
  if(loIter.MoveNext()):
    hiIter.MoveNext()
    lo = 0
    hi = 0
    while(hiIter.MoveNext()):
      hi = hi + 1
    QuickSortRec(loIter, hiIter, lo, hi)

def QuickSortRec(loIter:{'MoveNext':Function([],Bool),'MovePrev':Function([],Bool),'Current':Function([],int),'SetValue':Function([int],Void),'Clone':Function([],Self)}, hiIter:{'MoveNext':Function([],Bool),'MovePrev':Function([],Bool),'Current':Function([],int),'SetValue':Function([int],Void),'Clone':Function([],Self)}, lo : int, hi : int)->Void:
  if(lo<hi):
    upper=hiIter.Clone()
    lower=loIter.Clone()
    losize=Partition(lower,upper, hi-lo)
    QuickSortRec(loIter, upper, lo, lo+losize-1)
    QuickSortRec(lower, hiIter, lo + losize, hi)

def Partition(loIter:{'MoveNext':Function([],Bool),'MovePrev':Function([],Bool),'Current':Function([],int),'SetValue':Function([int],Void),'Clone':Function([],Self)}, hiIter: {'MoveNext':Function([],Bool),'MovePrev':Function([],Bool),'Current':Function([],int),'SetValue':Function([int],Void),'Clone':Function([],Self)}, distance:int)->int:
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

def MakeIntList():
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

