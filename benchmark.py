import os
import os.path
import subprocess
import datetime
import shutil
import time

class ConfigSearchObject(object):
  def __init__(self,index,flag,suffix):
    self.index=index
    self.flag=flag
    self.dirname=str(index)+flag
    self.filename=str(index)+flag+"."+suffix
    self.suffix=suffix
    self.file=None
    
  def __str__(self):
    return self.filename
  def __repr__(self):
    return self.filename

class Configuration(object):
  def __init__(self, entries):
    self.entries=[ConfigSearchObject(entry.index, entry.flag, entry.suffix) for entry in entries ]
    self.results=[]
    
  def add_result(self,result):
    self.results.append(result)
  
  def found_file(self, name, path):
    entry = next((x for x in self.entries if x.filename == name), None)
    if not entry == None:
      if not entry.file==None:
        print("Found duplicate matching file: "+str(entry)+" in "+str(self))
      entry.file = path
  
  def matches_dir(self, name):
    entry = next((x for x in self.entries if x.dirname == name), None)
    return not entry == None
    
  def is_valid(self):
    for entry in self.entries:
      if entry.file==None:
        return False
    return True
    
  def __str__(self):
    return self.get_name()
  def __repr__(self):
    return self.get_name()
    
  def get_name(self):
    return "".join([entry.flag for entry in self.entries])
    
  def print_file(self, path):
    wf = open(path, "w")
    for entry in self.entries:
      rf = open(entry.file, "r")
      wf.write(rf.read())
      rf.close()
    wf.close()

def assemble_configs(path, files, flags, suffix):
  cfgs = [Configuration([])]
  for i in files:
    cfgs = [Configuration(cfg.entries + [ConfigSearchObject(i,flag,suffix)]) for flag in flags for cfg in cfgs]
  assemble_configs_path(path, cfgs)
  return [cfg for cfg in cfgs if cfg.is_valid()]
  
def assemble_configs_path(path, cfgs):
  for entry in os.scandir(path):
    if(entry.is_file()):
      for cfg in cfgs:
        cfg.found_file(entry.name, entry.path)
    if(entry.is_dir()):
      matches = [cfg for cfg in cfgs if cfg.matches_dir(entry.name)]
      if(len(matches) > 0):
        assemble_configs_path(entry.path, matches)

def run(path, files, flags, suffix, codename, name, rounds, compiler, runner, runargs):
  curdir = os.path.abspath(os.curdir)
  filesroot = os.path.join(path, "files/")
  configsroot = os.path.join(path, "configurations/")
  if not os.path.exists(configsroot):
    os.mkdir(configsroot)
  starttime = datetime.datetime.now()
  resultpath = os.path.join(path, name+"-"+starttime.strftime("%Y%m%d-%H%M%S")+".csv")
  resultfile = open(resultpath, "w")
  resultfile.write("Run,Config,Time\n")
  cfgs = assemble_configs(filesroot, files, flags, suffix)
  for cfg in cfgs:
    os.chdir(curdir)
    cfgdir = os.path.join(configsroot, cfg.get_name()+"/")
    if not os.path.exists(cfgdir):
      os.mkdir(cfgdir)
    filepath = os.path.join(cfgdir, codename)
    cfg.print_file(filepath)
    compiler(cfgdir, codename, name)
  for round in range(1,rounds+1):
    cfgno=1
    for cfg in cfgs:
      print("Round "+str(round)+"/"+str(rounds)+": "+cfg.get_name()+" ("+str(cfgno)+"/"+str(len(cfgs))+")")
      os.chdir(curdir)
      cfgdir = os.path.join(configsroot, cfg.get_name()+"/")
      time = runner(cfgdir, codename, name, runargs)
      resultfile.write(str(round)+","+cfg.get_name()+","+str(time)+"\n")
      cfg.add_result(time)
      cfgno=cfgno+1
  resultfile.close()
  os.chdir(curdir)
  resultpath = os.path.join(path, name+"-"+starttime.strftime("%Y%m%d-%H%M%S")+"_finished.csv")
  resultfile = open(resultpath, "w")
  #resultfile.write("Config,"+",".join([str(i) for i in range(1,rounds+1)])+"\n")
  for cfg in cfgs:
    resultfile.write(cfg.get_name()+","+",".join([str(r) for r in cfg.results])+"\n")
  resultfile.close()
  os.chdir(curdir)

def javacompiler(path, codename, name):
  os.chdir(path)
  subprocess.call(["javac",codename])
  
def javarunner(path, codename, name,runargs):
  os.chdir(path)
  time.sleep(0.1)
  output = subprocess.check_output(["java","-cp",".","Main"]+runargs,universal_newlines=True)
  time.sleep(0.1)
  output=str(output).split("\n")
  lasttime = ""
  for line in output:
    if line.strip().lower().endswith("seconds"):
      lasttime=line
  lasttime = lasttime[:-7].strip()
  return float(lasttime)
  
def csharpcompiler(path, codename, name):
  os.chdir(path)
  subprocess.call(["dotnet","new","console","--language","C#","--name",name,"-o",".","--force"])
  os.unlink("Program.cs")
  
def csharprunner(path, codename, name,runargs):
  os.chdir(path)
  time.sleep(0.1)
  output = subprocess.check_output(["dotnet","run","--project",name+".csproj","--configuration","Release"]+runargs,universal_newlines=True)
  time.sleep(0.1)
  output=str(output).split("\n")
  lasttime = ""
  for line in output:
    if line.strip().lower().endswith("seconds"):
      lasttime=line
  lasttime = lasttime[:-7].strip()
  return float(lasttime)

def griftcompiler(path, codename, name):
  os.chdir(path)
  subprocess.call(["grift","--reference-semantics","Proxied","--coercions",codename])
  
def monogriftcompiler(path, codename, name):
  os.chdir(path)
  subprocess.call(["grift","--reference-semantics","Monotonic","--coercions",codename])
  
def griftrunner(path, codename, name,runargs):
  os.chdir(path)
  time.sleep(0.1)
  output = subprocess.check_output(["./a.out"]+runargs,universal_newlines=True)
  time.sleep(0.1)
  output=str(output).split("\n")
  lasttime = ""
  for line in output:
    if line.strip().lower().startswith("time (sec):"):
      lasttime=line
  lasttime = lasttime[11:].strip()
  return float(lasttime)
 
def pypycompiler(path, codename, name):
  basepath = os.path.join(path,"../../../")
  utilpath = os.path.join(basepath,"util.py")
  compatpath = os.path.join(basepath, "compat.py")
  reticpath = os.path.join(basepath,"retic/")
  utiltargetpath = os.path.join(path,"util.py")
  compattargetpath = os.path.join(path, "compat.py")
  retictargetpath = os.path.join(path,"retic/")
  shutil.copyfile(utilpath,utiltargetpath)
  shutil.copyfile(compatpath,compattargetpath)
  shutil.copytree(reticpath,retictargetpath,dirs_exist_ok=True)
  
def pypyrunner(path, codename, name,runargs):
  os.chdir(path)
  time.sleep(0.1)
  output = subprocess.check_output(["pypy3",codename]+runargs,universal_newlines=True)
  time.sleep(0.1)
  output=str(output).split("\n")
  lasttime = ""
  for line in output:
    if len(line)>0:
      lasttime=line
  return float(lasttime)
  
def higgscompiler(path,codename,name):
  basepath = os.path.join(path,"../../../")
  runtimepath = os.path.join(basepath,"higgs/source/runtime/")
  stdlibpath = os.path.join(basepath,"higgs/source/stdlib/")
  bmpath = os.path.join(basepath,"benchmark-higgs.js")
  runtimetargetpath = os.path.join(path,"runtime")
  stdlibtargetpath = os.path.join(path,"stdlib")
  bmtargetpath = os.path.join(path,"benchmark.js")
  if not (os.path.exists(runtimetargetpath)):
    os.symlink(runtimepath,runtimetargetpath,target_is_directory=True)
  if not (os.path.exists(stdlibtargetpath)):
    os.symlink(stdlibpath,stdlibtargetpath,target_is_directory=True)
  if not (os.path.exists(bmtargetpath)):
    shutil.copyfile(bmpath,bmtargetpath)
  os.chdir(path)
  subprocess.call(["tsc","--noImplicitAny","--generateContracts",codename])

def higgsrunner(path, codename, name, runargs):
  os.chdir(path)
  time.sleep(0.1)
  output = subprocess.check_output(["higgs","benchmark.js"]+runargs,universal_newlines=True)
  time.sleep(0.1)
  output=str(output).split("\n")
  lasttime = ""
  for line in output:
    if line.strip().lower().endswith("seconds"):
      lasttime=line
  lasttime = lasttime[:-7].strip()
  return float(lasttime)
 
def nodecompiler(path,codename,name):
  basepath = os.path.join(path,"../../../")
  bmpath = os.path.join(basepath,"benchmark-node.js")
  bmtargetpath = os.path.join(path,"benchmark.js")
  if not (os.path.exists(bmtargetpath)):
    shutil.copyfile(bmpath,bmtargetpath)
  os.chdir(path)
  subprocess.call(["tsc",codename])

def noderunner(path, codename, name, runargs):
  os.chdir(path)
  time.sleep(0.1)
  output = subprocess.check_output(["node","benchmark.js"]+runargs,universal_newlines=True)
  time.sleep(0.1)
  output=str(output).split("\n")
  lasttime = ""
  for line in output:
    if line.strip().lower().endswith("seconds"):
      lasttime=line
  lasttime = lasttime[:-7].strip()
  return float(lasttime)