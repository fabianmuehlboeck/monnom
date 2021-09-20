"use strict";
exports.__esModule = true;
var main;
(function (main_1) {
    function CountFrom(n) {
        return new Stream(n, function () { return CountFrom(n + 1); });
    }
    function getPrimes() {
        return Sieve(CountFrom(2));
    }
    function main() {
        var nminusone = 9999;
        var result = stream_get(getPrimes(), nminusone);
    }
    main_1.main = main;
    main_1.runs = 1;
    function Sieve(s) {
        var fst = s.first;
        return new Stream(fst, function () { return Sieve(Sift(fst, s.rest())); });
    }
    function Sift(n, s) {
        var fst = s.first;
        if (fst % n == 0) {
            return Sift(n, s.rest());
        }
        else {
            return new Stream(fst, function () { return Sift(n, s.rest()); });
        }
    }
    var Stream = /** @class */ (function () {
        function Stream(f, r) {
            this.first = f;
            this.rest = r;
        }
        return Stream;
    }());
    ;
    function stream_get(s, n) {
        while (n > 0) {
            n = n - 1;
            s = s.rest();
        }
        return s.first;
    }
})(main = exports.main || (exports.main = {}));
