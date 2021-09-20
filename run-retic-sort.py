from benchmark import run, pypycompiler, pypyrunner
import sys
import os.path
import os

runs=os.getenv('RUNS')
if(runs is None):
  runs=1
else:
  runs=int(runs)
path = os.path.abspath(sys.argv[1])
path = os.path.join(path,"experiments/reticulated/sort/")
run(path, ["sort"], ["IICCCCC","IICCCCD","IICCCDC","IICCCDD","IITTTDC","KKCCCCC","KKCCCCD","KKCCCDC","KKCCCDD","KKTTTDC","KKTTTDD"], "py", "sort.py", "sort", runs, pypycompiler, pypyrunner, [])

