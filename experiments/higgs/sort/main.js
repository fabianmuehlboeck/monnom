"use strict";
exports.__esModule = true;
var main;
(function (main_1) {
    var ListNode = (function () {
        function ListNode(v) {
            this.value = v;
            this.next = this;
            this.prev = this;
        }
        return ListNode;
    }());
    var List = (function () {
        function List(val) {
            this.size = 1;
            this.first = new ListNode(val);
        }
        List.prototype.add = function (val) {
            var newNode = new ListNode(val);
            newNode.prev = this.first.prev;
            newNode.next = this.first;
            this.first.prev = newNode;
            newNode.prev.next = newNode;
            this.size = this.size + 1;
        };
        List.prototype.getIterator = function () {
            var iter = this.makeIterator(this.first);
            return iter;
        };
        List.prototype.getSize = function () {
            return this.size;
        };
        List.prototype.makeIterator = function (n) {
            var self = this;
            return new IteratorImpl(self, n);
        };
        return List;
    }());
    var IteratorImpl = (function () {
        function IteratorImpl(l, node) {
            this.currentNode = node;
            this.parent = l;
        }
        IteratorImpl.prototype.movePrev = function () {
            if (this.currentNode === this.parent.first) {
                return false;
            }
            this.currentNode = this.currentNode.prev;
            return true;
        };
        IteratorImpl.prototype.moveNext = function () {
            if (this.currentNode.next === this.parent.first) {
                return false;
            }
            this.currentNode = this.currentNode.next;
            return true;
        };
        IteratorImpl.prototype.current = function () {
            return this.currentNode.value;
        };
        IteratorImpl.prototype.setValue = function (x) {
            this.currentNode.value = x;
        };
        IteratorImpl.prototype.clone = function () {
            return this.parent.makeIterator(this.currentNode);
        };
        return IteratorImpl;
    }());
    function quicksort(l) {
        var loIter = l.getIterator();
        var hiIter = l.getIterator();
        if (loIter.moveNext()) {
            hiIter.moveNext();
            var lo = 0;
            var hi = 0;
            while (hiIter.moveNext()) {
                hi = hi + 1;
            }
            quicksortRec(loIter, hiIter, lo, hi);
        }
    }
    function quicksortRec(loIter, hiIter, lo, hi) {
        if (lo < hi) {
            var upper = hiIter.clone();
            var lower = loIter.clone();
            var losize = partition(lower, upper, hi - lo);
            quicksortRec(loIter, upper, lo, lo + losize - 1);
            quicksortRec(lower, hiIter, lo + losize, hi);
        }
    }
    function partition(loIter, hiIter, distance) {
        var pivot = loIter.current();
        var losize = 0;
        while (true) {
            while (loIter.current() < pivot) {
                loIter.moveNext();
                distance = distance - 1;
                losize = losize + 1;
            }
            while (hiIter.current() > pivot) {
                hiIter.movePrev();
                distance = distance - 1;
            }
            if (distance < 0) {
                break;
            }
            var buffer = loIter.current();
            loIter.setValue(hiIter.current());
            hiIter.setValue(buffer);
            loIter.moveNext();
            losize = losize + 1;
            hiIter.movePrev();
            distance = distance - 2;
        }
        return losize;
    }
    function main() {
        var intList = makeIntList();
        test(intList);
        var ilistIter = intList.getIterator();
        ilistIter.moveNext();
        var last = ilistIter.current();
        while (ilistIter.moveNext()) {
            if (ilistIter.current() < last) {
                console.error("sorting failed!");
            }
            last = ilistIter.current();
        }
    }
    main_1.main = main;
    function test(list) {
        quicksort(list);
    }
    function makeIntList() {
        var list = new List(5);
        var i = 0;
        while (i < 100000) {
            var num = (i * 163841 + 176081) % 122251;
            list.add(num);
            i = i + 1;
        }
        return list;
    }
})(main = exports.main || (exports.main = {}));
