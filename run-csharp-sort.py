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
path = os.path.join(path,"experiments/csharp/sort/")
run(path, ["List","ListImpl","Main","Sort"], ["II","CCC","C"], "cs", "Main.cs", "sort", runs, csharpcompiler, csharprunner, [])