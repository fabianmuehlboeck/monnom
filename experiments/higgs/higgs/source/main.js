"use strict";
exports.__esModule = true;
var c151a835ceacdb11090d1960ae92771fe;
if (typeof c151a835ceacdb11090d1960ae92771fe === "undefined")
    c151a835ceacdb11090d1960ae92771fe = $ir_contract_for("number");
$ir_obj_def_const(this, "c151a835ceacdb11090d1960ae92771fec", c151a835ceacdb11090d1960ae92771fe, true);
function c9deb2b0cfd31e008a148d99a841154baf() {
    "Array";
    if (typeof c9deb2b0cfd31e008a148d99a841154ba !== "undefined")
        return c9deb2b0cfd31e008a148d99a841154ba;
    var co = $ir_contract_for("object");
    var c = co;
    c = $ir_contract_oblige_member(c, "length", co);
    c = $ir_contract_oblige_array(c, co);
    c9deb2b0cfd31e008a148d99a841154ba = c;
    $ir_contract_oblige_member(c, "length", c151a835ceacdb11090d1960ae92771fe);
    c = $ir_contract_oblige_array(c, c151a835ceacdb11090d1960ae92771fe);
    return c;
}
var c9deb2b0cfd31e008a148d99a841154ba;
if (typeof c9deb2b0cfd31e008a148d99a841154ba === "undefined")
    c9deb2b0cfd31e008a148d99a841154ba = c9deb2b0cfd31e008a148d99a841154baf();
$ir_obj_def_const(this, "c9deb2b0cfd31e008a148d99a841154bac", c9deb2b0cfd31e008a148d99a841154ba, true);
function c1dd0c346254ae5e69699998cc90c136df() {
    "InfiniteList";
    if (typeof c1dd0c346254ae5e69699998cc90c136d !== "undefined")
        return c1dd0c346254ae5e69699998cc90c136d;
    var co = $ir_contract_for("object");
    var c = co;
    c = $ir_contract_oblige_member(c, "tList", co);
    c1dd0c346254ae5e69699998cc90c136d = c;
    $ir_contract_oblige_member(c, "tList", c9deb2b0cfd31e008a148d99a841154baf());
    return c;
}
var c1dd0c346254ae5e69699998cc90c136d;
if (typeof c1dd0c346254ae5e69699998cc90c136d === "undefined")
    c1dd0c346254ae5e69699998cc90c136d = c1dd0c346254ae5e69699998cc90c136df();
$ir_obj_def_const(this, "c1dd0c346254ae5e69699998cc90c136dc", c1dd0c346254ae5e69699998cc90c136d, true);
// sieve of erastosthenes 
// based on the typed racket version
// sieve of erastosthenes 
var main;
(function (main_1) {
    var InfiniteList = (function () {
        function InfiniteList() {
            $rt_addContract(this, c1dd0c346254ae5e69699998cc90c136dc, "main.ts(6,29)");
            this.tList = [2]; // start off with 2 as the first prime
        }
        // get the nth prime
        // compute iteratively, kind of like the racket code
        InfiniteList.prototype.sieveGetPrime = function (n) {
            $rt_addContract(this, c1dd0c346254ae5e69699998cc90c136dc, "main.ts(9,14)");
            // check current length of list
            // if n < length, return list[ n - 1]
            // else, keep computing
            if (this.tList.length >= n) {
                return this.tList[n - 1];
            }
            var oldLen = this.tList.length;
            var diff = n - oldLen;
            for (var i = 0; i < diff; ++i) {
                this.computeNextPrime();
            }
            return this.tList[n - 1];
        };
        // compute the next prime in the list
        InfiniteList.prototype.computeNextPrime = function () {
            $rt_addContract(this, c1dd0c346254ae5e69699998cc90c136dc, "main.ts(25,14)");
            var curVal = this.tList[this.tList.length - 1];
            // check previous primes computed in the list; if not divisible by
            // any, then it also must be a prime
            // logic for being "faithful" to the sieve: it only checks if divisible by previous
            // primes, it doesn't check against all values between itself and 0
            while (true) {
                var isPrime = true;
                // go through each previous prime and check its mod
                for (var i = 0; i < this.tList.length; i++) {
                    if (curVal % this.tList[i] == 0) {
                        isPrime = false;
                        break;
                    }
                }
                if (isPrime) {
                    break;
                }
                curVal++;
            }
            this.tList.push(curVal);
        };
        return InfiniteList;
    }());
    ;
    function main() {
        var computeWith = new InfiniteList();
        var out = $rt_check(computeWith.sieveGetPrime(10000), "number");
    }
    main_1.main = main;
    main_1.runs = 4;
})(main = exports.main || (exports.main = {}));
