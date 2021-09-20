from retic.runtime import *
from retic.transient import *
from retic.typing import *

def check4(val):
    try:
        val.currentNode
        return val
    except:
        raise CheckError(val)

def check7(val):
    try:
        val.value
        return val
    except:
        raise CheckError(val)

def check9(val):
    try:
        val.MoveNext
        val.MovePrev
        val.Current
        val.Clone
        val.SetValue
        return val
    except:
        raise CheckError(val)

def check2(val):
    try:
        val.size
        return val
    except:
        raise CheckError(val)

def check0(val):
    try:
        val.first
        return val
    except:
        raise CheckError(val)

def check6(val):
    try:
        val.next
        return val
    except:
        raise CheckError(val)

def check12(val):
    try:
        val.parse_args
        return val
    except:
        raise CheckError(val)

def check8(val):
    try:
        val.GetIterator
        val.Add
        return val
    except:
        raise CheckError(val)

def check1(val):
    try:
        val.prev
        return val
    except:
        raise CheckError(val)

def check5(val):
    try:
        val.parent
        return val
    except:
        raise CheckError(val)

def check10(val):
    try:
        val.Add
        return val
    except:
        raise CheckError(val)

def check3(val):
    try:
        val.MakeIterator
        return val
    except:
        raise CheckError(val)

def check11(val):
    try:
        val.OptionParser
        return val
    except:
        raise CheckError(val)
from compat import xrange
import time
import util
import optparse


class Lst(retic_actual(object)):

    def __init__(self, val):
        self.first = check_type_function(ListNode)(val)
        self.size = 1
    __init__ = check_type_function(__init__)

    def Add(self, val):
        check_type_int(val)
        newNode = check_type_function(ListNode)(val)
        newNode.prev = check1(check0(self).first).prev
        newNode.next = check0(self).first
        check0(self).first.prev = newNode
        check1(newNode).prev.next = newNode
        self.size = (check2(self).size + 1)
    Add = check_type_function(Add)

    def GetIterator(self):
        return check_type_function(check3(self).MakeIterator)(check0(self).first)
    GetIterator = check_type_function(GetIterator)

    def MakeIterator(self, node):
        return check_type_function(Iterator)(self, node)
    MakeIterator = check_type_function(MakeIterator)
Lst = Lst


class ListNode(retic_actual(object)):

    def __init__(self, v):
        self.value = v
        self.next = self
        self.prev = self
    __init__ = check_type_function(__init__)
ListNode = ListNode


class Iterator(retic_actual(object)):

    def __init__(self, l, node):
        self.currentNode = node
        self.parent = l
    __init__ = check_type_function(__init__)

    def MovePrev(self):
        if (check4(self).currentNode is check0(check5(self).parent).first):
            return False
        self.currentNode = check1(check4(self).currentNode).prev
        return True
    MovePrev = check_type_function(MovePrev)

    def MoveNext(self):
        if (check6(check4(self).currentNode).next is check0(check5(self).parent).first):
            return False
        self.currentNode = check6(check4(self).currentNode).next
        return True
    MoveNext = check_type_function(MoveNext)

    def Current(self):
        return check_type_int(check7(check4(self).currentNode).value)
    Current = check_type_function(Current)

    def SetValue(self, x):
        check_type_int(x)
        check4(self).currentNode.value = x
    SetValue = check_type_function(SetValue)

    def Clone(self):
        return check_type_function(check3(check5(self).parent).MakeIterator)(check4(self).currentNode)
    Clone = check_type_function(Clone)
Iterator = Iterator

def QuickSort(l):
    check8(l)
    loIter = check9(check9(check_type_function(l.GetIterator)()))
    hiIter = check9(check9(check_type_function(l.GetIterator)()))
    if check_type_function(loIter.MoveNext)():
        check_type_function(hiIter.MoveNext)()
        lo = 0
        hi = 0
        while check_type_function(hiIter.MoveNext)():
            hi = (hi + 1)
        check_type_void(QuickSortRec(check9(loIter), check9(hiIter), lo, check_type_int(hi)))
QuickSort = check_type_function(QuickSort)

def QuickSortRec(loIter, hiIter, lo, hi):
    check9(loIter)
    check9(hiIter)
    check_type_int(lo)
    check_type_int(hi)
    if (lo < hi):
        upper = check9(check9(check_type_function(hiIter.Clone)()))
        lower = check9(check9(check_type_function(loIter.Clone)()))
        losize = check_type_int(Partition(check9(lower), check9(upper), (hi - lo)))
        check_type_void(QuickSortRec(loIter, check9(upper), lo, ((lo + losize) - 1)))
        check_type_void(QuickSortRec(check9(lower), hiIter, (lo + losize), hi))
QuickSortRec = check_type_function(QuickSortRec)

def Partition(loIter, hiIter, distance):
    check9(loIter)
    check9(hiIter)
    check_type_int(distance)
    pivot = check_type_int(check_type_function(loIter.Current)())
    losize = 0
    while True:
        while (check_type_int(check_type_function(loIter.Current)()) < pivot):
            check_type_bool(check_type_function(loIter.MoveNext)())
            distance = (distance - 1)
            losize = (losize + 1)
        while (check_type_int(check_type_function(hiIter.Current)()) > pivot):
            check_type_bool(check_type_function(hiIter.MovePrev)())
            distance = (distance - 1)
        if (distance < 0):
            break
        buf = check_type_int(check_type_function(loIter.Current)())
        check_type_void(check_type_function(loIter.SetValue)(check_type_int(check_type_function(hiIter.Current)())))
        check_type_void(check_type_function(hiIter.SetValue)(buf))
        check_type_bool(check_type_function(loIter.MoveNext)())
        losize = (losize + 1)
        check_type_bool(check_type_function(hiIter.MovePrev)())
        distance = (distance - 2)
    return check_type_int(losize)
Partition = check_type_function(Partition)

def MakeIntList():
    lst = check_type_function(Lst)(5)
    i = 0
    while (i < 100000):
        num = (((i * 163841) + 176081) % 122251)
        check_type_function(check10(lst).Add)(num)
        i = (i + 1)
    return lst
MakeIntList = check_type_function(MakeIntList)

def main(arg, timer):
    times = []
    for i in check_type_function(xrange)(arg):
        mylist = MakeIntList()
        t0 = check_type_function(timer)()
        check_type_void(QuickSort(check8(mylist)))
        tk = check_type_function(timer)()
        check_type_void(check_type_function(times.append)((tk - t0)))
    return times
main = check_type_function(main)
if (__name__ == '__main__'):
    parser = check_type_function(check11(optparse).OptionParser)(usage='%prog [options]', description='Test the performance of the Sort benchmark')
    check_type_function(util.add_standard_options_to)(parser)
    (options, args) = check_type_tuple(check_type_function(check12(parser).parse_args)(), 2)
    check_type_function(util.run_benchmark)(options, 1, main)
