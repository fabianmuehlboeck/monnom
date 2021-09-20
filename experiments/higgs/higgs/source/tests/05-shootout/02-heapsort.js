normalPrint = print;
silentPrint = function () {};

print = silentPrint;

arguments = [4];
load('benchmarks/shootout/heapsort.js');
assert(Math.abs(ary[n] - 0.9999928555) < 0.001);

print = normalPrint;

