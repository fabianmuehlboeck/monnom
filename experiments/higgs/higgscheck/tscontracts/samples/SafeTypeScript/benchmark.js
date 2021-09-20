/*
var Benchmark = require('benchmark');

var bench = new Benchmark("test",
			  benchmark_fn,
			  {"setup" : setup_fn, "teardown": teardown_fn });

bench.run();
console.log("nsamples: " + bench.stats.sample.length);
console.log("mean: " + bench.stats.mean);
console.log("moe: " + bench.stats.moe);
// console.log("rme: " + bench.stats.rme);
// console.log("std dev: " + bench.stats.deviation);
// console.log("sem: " + bench.stats.sem);
*/

// Warmup
for (var i = 0; i < 20; i++) {
    benchmark_fn();
}

// Run
var a = new Date().getTime();
for (var i = 0; i < 10; i++) {
    benchmark_fn();
}
var b = new Date().getTime();
print(b-a);
