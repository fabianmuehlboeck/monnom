from benchmark import run, javacompiler, javarunner
import sys
import os.path
import os

runs=os.getenv('RUNS')
if(runs is None):
  runs=1
else:
  runs=int(runs)
path = os.path.abspath(sys.argv[1])
path = os.path.join(path,"experiments/java/sieve/")
run(path, ["Sieve"], ["CICCCC"], "java", "Main.java", "sieve", runs, javacompiler, javarunner, [])