#!/usr/bin/env node
var fs = require("fs");
var cp = require("child_process");
var shell = require("shelljs");

if (process.argv.length < 6) {
    console.error("Use: mkbad.js <benchmark> <prefix> <# annotations> <process ct>");
    process.exit(1);
}

var benchmark = process.argv[2];
var prefix = process.argv[3];
var annCt = +process.argv[4];
var procCt = +process.argv[5];
var bmjs = fs.readFileSync("benchmark.js", "utf8");

function bitify(arr) {
    var oChars = [];
    for (var l = 0; l < arr.length; l += 4) {
        var ov = 0;
        for (var s = 0; s < 4; s++) {
            if (arr[l+s])
                ov |= (1<<s);
        }
        oChars.push(ov.toString(16));
    }
    return oChars.reverse().join("");
}

function mangle(anns, then) {
    var annsB = bitify(anns);

    // First mangle the annotations
    var ca = cp.spawn("node", [
        "../../built/local/tsc.js",
        "--annotationMangler", annsB,
        "--outFile", "tmp-" + annsB + ".ts",
        benchmark + "-typed.ts"]);
    ca.on("close", () => {

    // Compile the new version
    var cb = cp.spawn("node", [
        "../../built/local/tsc.js",
        "--generateContracts",
        "tmp-" + annsB + ".ts"]);
    cb.on("close", () => {

    // Put it together
    var bm = fs.readFileSync("tmp-" + annsB + ".js", "utf8");
    fs.writeFileSync("tmp-" + annsB + ".js", prefix + "\n" + bm + "\n" + bmjs);

    then();

    }); });
}

function run(anns, then) {
    var annsB = bitify(anns);

    // Run it
    var out = "";
    var ca = cp.spawn("higgs", ["tmp-" + annsB + ".js"]);
    ca.stdout.on("data", (chunk)=>{
        out += chunk.toString("utf8");
    });
    ca.on("close", ()=>{
        cp.spawn("rm", ["tmp-" + annsB + ".ts", "tmp-" + annsB + ".js"]).on("close", () => {
            then(+out);
        });
    });
}

// Start with a random set
var curWorst = new Array(annCt);
for (var i = 0; i < annCt; i++)
    curWorst[i] = (Math.random()>0.5);
var curWorstTime = 0;
var lastWorst = curWorst;

function reportWorst(step) {
    console.log(step + ",\"" + bitify(curWorst) + "\"," + curWorstTime);
}

mangle(curWorst, () => {
    run(curWorst, (time) => {
        curWorstTime = time;
        reportWorst(0);
        step(0);
    });
});

// Now walk
function step(stepNo) {
    var nextStepCt = 11;
    var toChange = ~~(annCt / 2 * (1-(Math.log(stepNo+1)/Math.log(100))));
    if (toChange <= 0)
        toChange = 1;
    console.error("Will change " + toChange);

    // Reset our current worst so we never fail to take a step due to a hiccup
    curWorstTime = 0;

    // Generate all our next steps
    var next = new Array(nextStepCt);
    for (var i = 0; i < nextStepCt-1; i++) {
        next[i] = curWorst.slice(0);
        for (var j = 0; j < toChange; j++) {
            var flip = ~~(Math.random()*annCt);
            next[i][flip] = !next[i][flip];
        }
    }
    next[nextStepCt-1] = curWorst.slice(0);

    // Mangle them
    var started = new Array(nextStepCt);
    var finished = new Array(nextStepCt);
    var done = 0;
    for (var i = 0; i < procCt && i < nextStepCt; i++) {
        started[i] = true;
        (function(i) {
            mangle(next[i], ()=>{postMangle(i);});
        })(i);
    }

    function postMangle(i) {
        finished[i] = true;
        done++;

        if (done < nextStepCt) {
            // Still more to do
            for (var j = 0; j < nextStepCt; j++) {
                if (!started[j]) {
                    // This one!
                    started[j] = true;
                    mangle(next[j], ()=>{postMangle(j);});
                    return;
                }
            }
        } else {
            // Last one, do the actual runs
            subStep(0);
        }
    }

    // And run them
    function subStep(subStepNo) {
        run(next[subStepNo], (time) => {
            console.error(bitify(next[subStepNo]) + ": " + time);
            if (time > curWorstTime) {
                curWorstTime = time;
                curWorst = next[subStepNo];
            }

            subStepNo++;
            if (subStepNo < nextStepCt) {
                subStep(subStepNo);
            } else {
                // Report our worst from this step, then move on
                stepNo++;
                reportWorst(stepNo);
                if (stepNo < 100)
                    step(stepNo);
            }
        });
    }
}
