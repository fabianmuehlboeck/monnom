from benchmark import run, csharpcompiler, csharprunner
import sys
import os.path
import os

runs=os.getenv('RUNS')
if(runs is None):
  runs=1
else:
  runs=int(runs)
path = os.path.abspath(sys.argv[1])
path = os.path.join(path,"experiments/csharp/sieve/")
run(path, ["Main","Stream"], ["CICCC","C"], "cs", "Main.cs", "sieve", runs, csharpcompiler, csharprunner, [])