from benchmark import run, higgscompiler, higgsrunner
import sys
import os.path
import os

runs=os.getenv('RUNS')
if(runs is None):
  runs=1
else:
  runs=int(runs)
path = os.path.abspath(sys.argv[1])
path = os.path.join(path,"experiments/higgs/sort/")
run(path, ["list","listimpl","main","sort"], ["II","KK","CCC","TTT","C","D"], "ts", "main.ts", "sort", runs, higgscompiler, higgsrunner, [])