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
path = os.path.join(path,"experiments/reticulated/float/")
run(path, ["float"], ["CCC","DSM","DTM"], "py", "float.py", "float", runs, pypycompiler, pypyrunner, [])