from benchmark import run, griftcompiler, griftrunner, monogriftcompiler
import sys
import os.path
import os

runs=os.getenv('RUNS')
if(runs is None):
  runs=1
else:
  runs=int(runs)
path = os.path.abspath(sys.argv[1])
regpath = os.path.join(path,"experiments/grift/float/")
monopath = os.path.join(path,"experiments/grift/floatmono/")
run(regpath, ["Main","Point","PointMapFun","run"], ["C","D","S","T","L","M",""], "grift", "float.grift", "float", runs, griftcompiler, griftrunner, [])
run(monopath, ["Main","Point","PointMapFun","run"], ["C","D","S","T","L","M",""], "grift", "float.grift", "floatmono", runs, monogriftcompiler, griftrunner, [])