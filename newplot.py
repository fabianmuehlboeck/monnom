import csv
import sys
import plotly.express as px
import plotly.graph_objects as go
from plotly.subplots import make_subplots
import pandas as pd
import json
from os import listdir
from os.path import isfile, join
import re

monnomdistances={'C':0,'I':0,'D':1,'J':1,'K':2,'L':1,'M':2,'S':1,'T':2}

markersize=8
linewidth=3

markerstyles = {'MonNom':{'color':'#000000', 'symbol':'x','size':markersize+2},
  'Nom':{'color':'#00B050', 'symbol':'cross','size':markersize},
  'Proxied Grift':{'color':'#ED7D31', 'symbol':'arrow-up','size':markersize},
  'Monotonic Grift':{'color':'#ED7D31', 'symbol':'diamond-open','size':markersize},
  'Racket':{'color':'#4472C4', 'symbol':'circle-open','size':markersize},
  'C#':{'color':'#264478', 'symbol':'diamond','size':markersize},
  'Java':{'color':'#7030A0', 'symbol':'diamond-wide','size':markersize+3},
  'NodeJS':{'color':'#9E480E', 'symbol':'circle','size':markersize},
  'HiggsCheck':{'color':'#C00000', 'symbol':'arrow-up','size':markersize},
  'Reticulated':{'color':'#B21E6F', 'symbol':'circle-open','size':markersize}}
linestyles = {'MonNom':{'color':'#000000', 'width':linewidth},
  'Nom':{'color':'#00aa00', 'dash':'dash', 'width':linewidth},
  'Proxied Grift':{'color':'#ED7D31', 'dash':'longdash', 'width':linewidth},
  'Monotonic Grift':{'color':'#ED7D31', 'dash':'dashdot', 'width':linewidth},
  'Racket':{'color':'#4472C4', 'dash':'dot', 'width':linewidth},
  'C#':{'color':'#264478', 'dash':'dot', 'width':linewidth},
  'Java':{'color':'#7030A0', 'dash':'dot', 'width':linewidth},
  'NodeJS':{'color':'#9E480E', 'dash':'dot', 'width':linewidth},
  'HiggsCheck':{'color':'#C00000', 'dash':'dot', 'width':linewidth},
  'Reticulated':{'color':'#B21E6F', 'dash':'dot', 'width':linewidth}}


def distance_to_fully_typed(config):
  ret=0
  for c in config:
    ret+=monnomdistances[c]
  return ret

def combine_funcs(f1,f2):
    return lambda x: f2(f1(x))

def cut_dotbm(str):
    return str[4:]

def fetch_key(key,data):
  try:
    if(key.isdigit()):
      return data.loc[int(key)][0]
    else:
      return data.loc[key][0]
  except KeyError:
    return "REMOVE"

def load_converter(path):
    data=pd.read_csv(path,index_col=0)
    return lambda k: fetch_key(str(k),data)

def check_key(key,data):
  try:
    if(key.isdigit()):
      return data.loc[int(key)][0] is None
    else:
      return data.loc[key][0] is None
  except KeyError:
    return True

def load_skipper(path,results):
    data=pd.read_csv(path,index_col=0)
    actualdata=pd.read_csv(results,header=None)
    return lambda i: check_key(cut_dotbm(actualdata.iat[i,0]),data)


def load_benchmark(path):
  config=json.load(open(path+"/plotconfig.json","rt"))
  if(config.get("version")!=None):
    if(config.get("version")=="v2"):
      return load_newbenchmark(path,config)
    if(config.get("version")=="v3"):
      return load_benchmarkv3(path,config)
    if(config.get("version")=="v1"):
      return load_oldbenchmark(path,config)
  return load_newbenchmark(path,config)

def load_newbenchmark(path,config):
    converter=cut_dotbm
    skipper=lambda x : False
    if(config.get("mapping")!=None):
      converter=combine_funcs(cut_dotbm,load_converter(path+"/"+config["mapping"]))
      skipper=load_skipper(path+"/"+config["mapping"],path+"/results.csv")
    data=pd.read_csv(path+"/results.csv",header=None,converters={0:converter},skiprows=skipper,index_col=0)
    datacolumns=len(data.columns)
    linesperprog=config["lines"]
    resultcolumns=[[] for i in range(0,linesperprog-1)]
    timescolumns=[]
    if(datacolumns%linesperprog!=0):
       raise Exception("Invalid number of columns: "+path)
    rightvalues=[]
    for i in range(0,linesperprog):
       if i!=config["time"]:
         rightvalues.append(data.iat[0,i])
    for i in range(0,datacolumns):
       if i%linesperprog==config["time"]:
         timescolumns.append(i)
       else:
        if i%linesperprog<config["time"]:
         resultcolumns[i%linesperprog].append(i)
        else:
         resultcolumns[(i-1)%linesperprog].append(i)
    for i in range(0,linesperprog-1):
      if not data.take(resultcolumns[i],axis=1).applymap(lambda x : x==rightvalues[i]).all(axis=None):
        print(data.take(resultcolumns[i],axis=1))
        raise Exception("not all result values match!")
    times=data.take(timescolumns,axis=1)
    times=times.rename(columns={0:'Configuration'})
    dists=pd.Series(times.index.map(distance_to_fully_typed), name='Distance to Fully Typed/Nominal')
    means=times.mean(axis=1,numeric_only=True).rename("Running Time in Seconds")
    stdevs=times.std(axis=1,numeric_only=True).rename("Running Time Standard Deviation")
    extended=pd.concat([pd.Series(times.index),dists],join="inner",axis=1)
    extended=extended.set_index([0])
    dtable=pd.DataFrame(extended).join(means).join(stdevs)
    return dtable
	
def load_benchmarkv3(path,config):
    fullresultsfiles=[(x.string,x.group()[(x.string.find("-")+1):-9]) for x in [re.search("[a-zA-Z]+\-([0-9\-]+)_finished.csv",f) for f in listdir(path) if re.search("[a-zA-Z]+\-([0-9\-]+)_finished.csv",f)!=None]]
    fullresultsfiles.sort(key=lambda x:x[1],reverse=True)
    data=pd.read_csv(path+"/"+fullresultsfiles[0][0],header=None,index_col=0)
    datacolumns=len(data.columns)
    timescolumns=[]
    for i in range(0,datacolumns):
       timescolumns.append(i)
    times=data.take(timescolumns,axis=1)
    times=times.rename(columns={0:'Configuration'})
    dists=pd.Series(times.index.map(distance_to_fully_typed), name='Distance to Fully Typed/Nominal')
    means=times.mean(axis=1,numeric_only=True).rename("Running Time in Seconds")
    stdevs=times.std(axis=1,numeric_only=True).rename("Running Time Standard Deviation")
    extended=pd.concat([pd.Series(times.index),dists],join="inner",axis=1)
    extended=extended.set_index([0])
    dtable=pd.DataFrame(extended).join(means).join(stdevs)
    return dtable

def load_oldbenchmark(path, config):
    fullresultsfiles=[(x.string,x.group()[8:-9]) for x in [re.search("results_([0-9\-]+)_full.csv",f) for f in listdir(path+"/benchmark") if re.search("results_([0-9\-]+)_full.csv",f)!=None]]
    fullresultsfiles.sort(key=lambda x:x[1],reverse=True)
    converter=load_converter(path+"/"+config["mapping"])
    data=pd.read_csv(path+"/benchmark/"+fullresultsfiles[0][0],header=0,converters={"Folder":converter}).pivot(index="Folder",columns="Run",values="Seconds")
    means=data.mean(axis=1,numeric_only=True).rename("Running Time in Seconds")
    stdevs=data.std(axis=1,numeric_only=True).rename("Running Time Standard Deviation")
    dists=pd.Series(data.index.map(distance_to_fully_typed), name='Distance to Fully Typed/Nominal')
    extended=pd.concat([pd.Series(data.index),dists],join="inner",axis=1)
    extended=extended.set_index(["Folder"])
    dtable=pd.DataFrame(extended).join(means).join(stdevs)
    return dtable

def load_monnom(fig, path, multilang):
    bmd=load_benchmark(path)
    baserow=1
    if multilang:
      baserow=2
      fig.append_trace(go.Scatter(None, mode='markers', x=bmd['Distance to Fully Typed/Nominal'],y=bmd["Running Time in Seconds"],marker=markerstyles["MonNom"],name="MonNom",legendgroup="runningtimes"),row=1,col=1)
    fig.append_trace(go.Scatter(None, mode='markers', x=bmd['Distance to Fully Typed/Nominal'],y=bmd["Running Time in Seconds"],marker=markerstyles["MonNom"],name="MonNom",showlegend=not multilang),row=baserow,col=1)
    mindistancetime = bmd["Distance to Fully Typed/Nominal"].idxmin()
    maxdistancetime = bmd["Distance to Fully Typed/Nominal"].idxmax()
    mindistentry=bmd.loc[mindistancetime]
    maxdistentry=bmd.loc[maxdistancetime]
    mindisttime=mindistentry["Running Time in Seconds"]
    maxdisttime=maxdistentry["Running Time in Seconds"]
    mindist=int(mindistentry['Distance to Fully Typed/Nominal'])
    maxdist=int(maxdistentry['Distance to Fully Typed/Nominal'])
    fig.add_shape(type="line", x0=maxdist,y0=maxdisttime,x1=mindist,y1=mindisttime, row=baserow,col=1,line={'color':"Green",'width':linewidth,'dash':'dot'})
    fig.add_shape(type="line", x0=maxdist,y0=maxdisttime,x1=mindist,y1=maxdisttime, row=baserow,col=1,line={'color':"RoyalBlue",'width':linewidth,'dash':'dash'})
    fig.add_shape(type="line", x0=0,y0=0,x1=0,y1=1, row=baserow,col=2,line={'color':"Black",'width':linewidth,'dash':'dash'})
    expectedTimes=[mindisttime+(((maxdisttime-mindisttime)/(maxdist-mindist))*i) for i in range(0,maxdist-mindist+1)]
    bmd["Compared to Untyped Baseline"] = (bmd["Running Time in Seconds"]/maxdisttime)-1.0
    bmd["Compared to Linear Baseline"] = (bmd["Running Time in Seconds"]/bmd["Distance to Fully Typed/Nominal"].map(lambda x:expectedTimes[x]))-1.0
    bmd=bmd.sort_values(by=["Compared to Linear Baseline"])
    linearOverheads = bmd["Compared to Linear Baseline"].drop_duplicates()
    linearCumulatives = [bmd[bmd["Compared to Linear Baseline"] <= x]["Compared to Linear Baseline"].count()/(bmd["Compared to Linear Baseline"].count()*1.0) for x in linearOverheads]
    bmd=bmd.sort_values(by=["Compared to Untyped Baseline"])
    untypedOverheads = bmd["Compared to Untyped Baseline"].drop_duplicates()
    untypedCumulatives = [bmd[bmd["Compared to Untyped Baseline"] <= x]["Compared to Untyped Baseline"].count()/(bmd["Compared to Untyped Baseline"].count()*1.0) for x in untypedOverheads]
    fig.append_trace(go.Scatter(None, x=linearOverheads, y=linearCumulatives,marker={'color':'Green', 'symbol':'circle','size':markersize},name="Compared to Linear Baseline",line={'color':"Green",'width':linewidth,'dash':'dot'},legendgroup="baselines"),row=baserow,col=2)
    fig.append_trace(go.Scatter(None, x=untypedOverheads, y=untypedCumulatives,marker={'color':'RoyalBlue', 'symbol':'circle','size':markersize},name="Compared to Untyped Baseline",line={'color':"RoyalBlue",'width':linewidth,'dash':'dash'},legendgroup="baselines"),row=baserow,col=2)
    if multilang:
      fig.append_trace(go.Scatter(None, x=untypedOverheads, y=untypedCumulatives,marker=markerstyles["MonNom"],showlegend=True,line=linestyles["MonNom"],legendgroup="cumulatives",name="MonNom"),row=1,col=2)

def load_grift_version(fig, path, versionname, offset, multilang):
    bmd=load_benchmark(path)
    fig.append_trace(go.Scatter(None, mode='markers', x=[x+offset for x in bmd['Distance to Fully Typed/Nominal']],y=bmd["Running Time in Seconds"],marker=markerstyles[versionname],name=versionname,legendgroup="runningtimes"),row=1,col=1)
    maxdistancetime = bmd["Distance to Fully Typed/Nominal"].idxmax()
    maxdistentry=bmd.loc[maxdistancetime]
    maxdisttime=maxdistentry["Running Time in Seconds"]
    bmd["Compared to Untyped Baseline"] = (bmd["Running Time in Seconds"]/maxdisttime)-1.0
    bmd=bmd.sort_values(by=["Compared to Untyped Baseline"])
    untypedOverheads = bmd["Compared to Untyped Baseline"].drop_duplicates()
    untypedCumulatives = [bmd[bmd["Compared to Untyped Baseline"] <= x]["Compared to Untyped Baseline"].count()/(bmd["Compared to Untyped Baseline"].count()*1.0) for x in untypedOverheads]
    fig.append_trace(go.Scatter(None, x=untypedOverheads, y=untypedCumulatives,marker=markerstyles[versionname],showlegend=True,line=linestyles[versionname],legendgroup="cumulatives",name=versionname),row=1,col=2)

def load_grift(fig, path, multilang):
    load_grift_version(fig, path+"/proxies", "Proxied Grift", 0.15, multilang)
    load_grift_version(fig, path+"/monotonic", "Monotonic Grift", -0.15, multilang)

def load_griftnew(fig, path, multilang):
    load_grift_version(fig, path, "Proxied Grift", 0.15, multilang)
    load_grift_version(fig, path+"mono", "Monotonic Grift", -0.15, multilang)
	
def load_racket(fig, path, multilang):
    bmd=load_benchmark(path)
    fig.append_trace(go.Scatter(None, mode='markers', x=[x-0.3 for x in bmd['Distance to Fully Typed/Nominal']],y=bmd["Running Time in Seconds"],marker=markerstyles["Racket"],name="Racket",legendgroup="runningtimes"),row=1,col=1)
    maxdistancetime = bmd["Distance to Fully Typed/Nominal"].idxmax()
    maxdistentry=bmd.loc[maxdistancetime]
    maxdisttime=maxdistentry["Running Time in Seconds"]
    bmd["Compared to Untyped Baseline"] = (bmd["Running Time in Seconds"]/maxdisttime)-1.0
    bmd=bmd.sort_values(by=["Compared to Untyped Baseline"])
    untypedOverheads = bmd["Compared to Untyped Baseline"].drop_duplicates()
    untypedCumulatives = [bmd[bmd["Compared to Untyped Baseline"] <= x]["Compared to Untyped Baseline"].count()/(bmd["Compared to Untyped Baseline"].count()*1.0) for x in untypedOverheads]
    fig.append_trace(go.Scatter(None, x=untypedOverheads, y=untypedCumulatives,marker=markerstyles["Racket"],showlegend=True,line=linestyles["Racket"],legendgroup="cumulatives",name="Racket"),row=1,col=2)

def load_nom(fig, path, multilang):
    bmd=load_benchmark(path)
    fig.append_trace(go.Scatter(None, mode='markers', x=[x+0.3 for x in bmd['Distance to Fully Typed/Nominal']],y=bmd["Running Time in Seconds"],marker=markerstyles["Nom"],name="Nom",legendgroup="runningtimes"),row=1,col=1)
    maxdistancetime = bmd["Distance to Fully Typed/Nominal"].idxmax()
    maxdistentry=bmd.loc[maxdistancetime]
    maxdisttime=maxdistentry["Running Time in Seconds"]
    bmd["Compared to Untyped Baseline"] = (bmd["Running Time in Seconds"]/maxdisttime)-1.0
    bmd=bmd.sort_values(by=["Compared to Untyped Baseline"])
    untypedOverheads = bmd["Compared to Untyped Baseline"].drop_duplicates()
    untypedCumulatives = [bmd[bmd["Compared to Untyped Baseline"] <= x]["Compared to Untyped Baseline"].count()/(bmd["Compared to Untyped Baseline"].count()*1.0) for x in untypedOverheads]
    fig.append_trace(go.Scatter(None, x=untypedOverheads, y=untypedCumulatives,marker=markerstyles["Nom"],showlegend=True,line=linestyles["Nom"],legendgroup="cumulatives",name="Nom"),row=1,col=2)

def load_java(fig, path, multilang):
    bmd=load_benchmark(path)
    fig.append_trace(go.Scatter(None, mode='markers', x=[x for x in bmd['Distance to Fully Typed/Nominal']],y=bmd["Running Time in Seconds"],marker=markerstyles["Java"],name="Java",legendgroup="runningtimes"),row=1,col=1)
    #maxdistancetime = bmd["Distance to Fully Typed/Nominal"].idxmax()
    #maxdistentry=bmd.loc[maxdistancetime]
    #maxdisttime=maxdistentry["Running Time in Seconds"]
    #bmd["Compared to Untyped Baseline"] = (bmd["Running Time in Seconds"]/maxdisttime)-1.0
    #bmd=bmd.sort_values(by=["Compared to Untyped Baseline"])
    #untypedOverheads = bmd["Compared to Untyped Baseline"].drop_duplicates()
    #untypedCumulatives = [bmd[bmd["Compared to Untyped Baseline"] <= x]["Compared to Untyped Baseline"].count()/(bmd["Compared to Untyped Baseline"].count()*1.0) for x in untypedOverheads]
    #fig.append_trace(go.Scatter(None, x=untypedOverheads, y=untypedCumulatives,marker=markerstyles["Java"],showlegend=True,line=linestyles["Java"],legendgroup="cumulatives",name="Java"),row=1,col=2)

def load_csharp(fig, path, multilang):
    bmd=load_benchmark(path)
    fig.append_trace(go.Scatter(None, mode='markers', x=[x for x in bmd['Distance to Fully Typed/Nominal']],y=bmd["Running Time in Seconds"],marker=markerstyles["C#"],name="C#",legendgroup="runningtimes"),row=1,col=1)
    #maxdistancetime = bmd["Distance to Fully Typed/Nominal"].idxmax()
    #maxdistentry=bmd.loc[maxdistancetime]
    #maxdisttime=maxdistentry["Running Time in Seconds"]
    #bmd["Compared to Untyped Baseline"] = (bmd["Running Time in Seconds"]/maxdisttime)-1.0
    #bmd=bmd.sort_values(by=["Compared to Untyped Baseline"])
    #untypedOverheads = bmd["Compared to Untyped Baseline"].drop_duplicates()
    #untypedCumulatives = [bmd[bmd["Compared to Untyped Baseline"] <= x]["Compared to Untyped Baseline"].count()/(bmd["Compared to Untyped Baseline"].count()*1.0) for x in untypedOverheads]
    #fig.append_trace(go.Scatter(None, x=untypedOverheads, y=untypedCumulatives,marker=markerstyles["C#"],showlegend=True,line=linestyles["C#"],legendgroup="cumulatives",name="C#"),row=1,col=2)

def load_node(fig, path, multilang):
    bmd=load_benchmark(path)
    fig.append_trace(go.Scatter(None, mode='markers', x=[x for x in bmd['Distance to Fully Typed/Nominal']],y=bmd["Running Time in Seconds"],marker=markerstyles["NodeJS"],name="NodeJS",legendgroup="runningtimes"),row=1,col=1)
    #maxdistancetime = bmd["Distance to Fully Typed/Nominal"].idxmax()
    #maxdistentry=bmd.loc[maxdistancetime]
    #maxdisttime=maxdistentry["Running Time in Seconds"]
    #bmd["Compared to Untyped Baseline"] = (bmd["Running Time in Seconds"]/maxdisttime)-1.0
    #bmd=bmd.sort_values(by=["Compared to Untyped Baseline"])
    #untypedOverheads = bmd["Compared to Untyped Baseline"].drop_duplicates()
    #untypedCumulatives = [bmd[bmd["Compared to Untyped Baseline"] <= x]["Compared to Untyped Baseline"].count()/(bmd["Compared to Untyped Baseline"].count()*1.0) for x in untypedOverheads]
    #fig.append_trace(go.Scatter(None, x=untypedOverheads, y=untypedCumulatives,marker=markerstyles["NodeJS"],showlegend=True,line=linestyles["NodeJS"],legendgroup="cumulatives",name="NodeJS"),row=1,col=2)

def load_higgs(fig, path, multilang):
    bmd=load_benchmark(path)
    fig.append_trace(go.Scatter(None, mode='markers', x=[x for x in bmd['Distance to Fully Typed/Nominal']],y=bmd["Running Time in Seconds"],marker=markerstyles["HiggsCheck"],name="HiggsCheck",legendgroup="runningtimes"),row=1,col=1)
    maxdistancetime = bmd["Distance to Fully Typed/Nominal"].idxmax()
    maxdistentry=bmd.loc[maxdistancetime]
    maxdisttime=maxdistentry["Running Time in Seconds"]
    bmd["Compared to Untyped Baseline"] = (bmd["Running Time in Seconds"]/maxdisttime)-1.0
    bmd=bmd.sort_values(by=["Compared to Untyped Baseline"])
    untypedOverheads = bmd["Compared to Untyped Baseline"].drop_duplicates()
    untypedCumulatives = [bmd[bmd["Compared to Untyped Baseline"] <= x]["Compared to Untyped Baseline"].count()/(bmd["Compared to Untyped Baseline"].count()*1.0) for x in untypedOverheads]
    fig.append_trace(go.Scatter(None, x=untypedOverheads, y=untypedCumulatives,marker=markerstyles["HiggsCheck"],showlegend=True,line=linestyles["HiggsCheck"],legendgroup="cumulatives",name="HiggsCheck"),row=1,col=2)

def load_reticulated(fig, path, multilang):
    bmd=load_benchmark(path)
    fig.append_trace(go.Scatter(None, mode='markers', x=[x for x in bmd['Distance to Fully Typed/Nominal']],y=bmd["Running Time in Seconds"],marker=markerstyles["Reticulated"],name="Reticulated",legendgroup="runningtimes"),row=1,col=1)
    maxdistancetime = bmd["Distance to Fully Typed/Nominal"].idxmax()
    maxdistentry=bmd.loc[maxdistancetime]
    maxdisttime=maxdistentry["Running Time in Seconds"]
    bmd["Compared to Untyped Baseline"] = (bmd["Running Time in Seconds"]/maxdisttime)-1.0
    bmd=bmd.sort_values(by=["Compared to Untyped Baseline"])
    untypedOverheads = bmd["Compared to Untyped Baseline"].drop_duplicates()
    untypedCumulatives = [bmd[bmd["Compared to Untyped Baseline"] <= x]["Compared to Untyped Baseline"].count()/(bmd["Compared to Untyped Baseline"].count()*1.0) for x in untypedOverheads]
    fig.append_trace(go.Scatter(None, x=untypedOverheads, y=untypedCumulatives,marker=markerstyles["Reticulated"],showlegend=True,line=linestyles["Reticulated"],legendgroup="cumulatives",name="Reticulated"),row=1,col=2)



langhandlers={}
langhandlers['monnom'] = load_monnom
langhandlers['grift'] = load_grift
langhandlers['griftnew'] = load_griftnew
langhandlers['racket'] = load_racket
langhandlers['nom'] = load_nom
langhandlers['csharp'] = load_csharp
langhandlers['java'] = load_java
langhandlers['node'] = load_node
langhandlers['higgs'] = load_higgs
langhandlers['retic'] = load_reticulated

if(len(sys.argv)>2):
    folders=[]
    hasMonnom=False
    for i in range(2,len(sys.argv),2):
        folders.append({'path':sys.argv[i],'kind':sys.argv[i+1]})
        hasMonnom=hasMonnom or (sys.argv[i+1]=="monnom")
    multilang = hasMonnom and len(folders)>1
    figrows=1
    if multilang:
      figrows=2
    fig = make_subplots(rows=figrows, cols=2)
    for f in folders:
        langhandlers[f['kind']](fig, f['path'],multilang)
    fig.layout.xaxis.autorange="reversed"
    fig.layout.yaxis.rangemode="tozero"
    fig.layout.yaxis.title="Running Time in Seconds"
    fig.layout.xaxis.title="Number of Steps to Fully Typed"
    fig.layout.yaxis2.tickformat=",.0%"
    fig.layout.yaxis2.title="Configurations Below"
    if multilang:
      fig.layout.xaxis3.title="Number of Steps to Fully Typed"
      fig.layout.xaxis3.autorange="reversed"
      fig.layout.yaxis3.title="Running Time in Seconds"
      fig.layout.yaxis3.rangemode="tozero"
      fig.layout.yaxis4.tickformat=",.0%"
      fig.layout.xaxis4.tickformat=",.0%"
      fig.layout.xaxis2.ticksuffix="x"
      fig.layout.yaxis4.title="Configurations Below"
    else:
      fig.layout.xaxis2.tickformat=",.0%"
    figtitle=sys.argv[1]
    renderimg=False
    if(sys.argv[1][-4:]==".png"):
       renderimg=True
       figtitle=figtitle[0:-4]
    fig.layout.title.text=sys.argv[1]
    fig.layout.title.font.size=30
    if(renderimg):
       fig.write_image(sys.argv[1])
    else:
       fig.show()


