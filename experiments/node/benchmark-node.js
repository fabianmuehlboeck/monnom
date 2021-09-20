var main = require("./main.js");

(function() {
    // 'main' is an export set exporting a module 'main' with a function 'main'
    var bm = main.main.main;
    var setup = main.main.setup;
    var resetup = main.main.resetup;
    var runs = main.main.runs;
    if (typeof runs !== "number")
        runs = 1;
    var warmupRuns = main.warmupRuns;
    if (typeof warmupRuns !== "number")
        warmupRuns = 2;

    // Setup if necessary
    if (typeof setup === "function")
        setup();

    // Do the warmup runs
    for (var i = 0; i < warmupRuns; i++) {
        bm();
       if(typeof resetup === "function")
       {
          resetup();
       }
   }

    // Then the real run
    var a = new Date().getTime();
    for (var i = 0; i < runs; i++)
        bm();
    var b = new Date().getTime();
    console.log(String((b-a)/1000)+" Seconds");
})();
