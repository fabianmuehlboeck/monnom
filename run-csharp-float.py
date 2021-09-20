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
path = os.path.join(path,"experiments/csharp/float/")
run(path, ["Enumerable","Float"], ["CCC",""], "cs", "Main.cs", "float", runs, csharpcompiler, csharprunner, [])