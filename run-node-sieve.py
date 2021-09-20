from benchmark import run, nodecompiler, noderunner
import sys
import os.path
import os

runs=os.getenv('RUNS')
if(runs is None):
  runs=1
else:
  runs=int(runs)
path = os.path.abspath(sys.argv[1])
path = os.path.join(path,"experiments/node/sieve/")
run(path, ["count-from","ifun","main","sieve","sift","streams"], ["K","D","T","M"], "ts", "main.ts", "sieve", runs, nodecompiler, noderunner, [])