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

markerstyles = {'MonNom':{'color':'#ff0000', 'symbol':'circle','size':markersize},
  'Nom':{'color':'#0000ff', 'symbol':'star','size':markersize},
  'Proxied Grift':{'color':'#00ff00', 'symbol':'cross','size':markersize},
  'Monotonic Grift':{'color':'#009900', 'symbol':'x','size':markersize},
  'Racket':{'color':'#ff00ff', 'symbol':'diamond','size':markersize}}
linestyles = {'MonNom':{'color':'#ff0000', 'width':linewidth},
  'Nom':{'color':'#0000ff', 'dash':'dash', 'width':linewidth},
  'Proxied Grift':{'color':'#00ff00', 'dash':'longdash', 'width':linewidth},
  'Monotonic Grift':{'color':'#009900', 'dash':'dashdot', 'width':linewidth},
  'Racket':{'color':'#ff00ff', 'dash':'dot', 'width':linewidth}}


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
    if(key.isdigit()):
      return data.loc[int(key)][0]
    else:
      return data.loc[key][0]

def load_converter(path):
    data=pd.read_csv(path,index_col=0)
    return lambda k: fetch_key(k,data)

def load_newbenchmark(path):
    config=json.load(open(path+"/plotconfig.json","rt"))
    converter=cut_dotbm
    if(config.get("mapping")!=None):
      converter=combine_funcs(cut_dotbm,load_converter(path+"/"+config["mapping"]))
    data=pd.read_csv(path+"/results.csv",header=None,converters={0:converter},index_col=0)
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

def load_oldbenchmark(path):
    config=json.load(open(path+"/plotconfig.json","rt"))
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
    bmd=load_newbenchmark(path)
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
    bmd=load_newbenchmark(path)
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

def load_racket(fig, path, multilang):
    bmd=load_oldbenchmark(path)
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
    bmd=load_oldbenchmark(path)
    fig.append_trace(go.Scatter(None, mode='markers', x=[x+0.3 for x in bmd['Distance to Fully Typed/Nominal']],y=bmd["Running Time in Seconds"],marker=markerstyles["Nom"],name="Nom",legendgroup="runningtimes"),row=1,col=1)
    maxdistancetime = bmd["Distance to Fully Typed/Nominal"].idxmax()
    maxdistentry=bmd.loc[maxdistancetime]
    maxdisttime=maxdistentry["Running Time in Seconds"]
    bmd["Compared to Untyped Baseline"] = (bmd["Running Time in Seconds"]/maxdisttime)-1.0
    bmd=bmd.sort_values(by=["Compared to Untyped Baseline"])
    untypedOverheads = bmd["Compared to Untyped Baseline"].drop_duplicates()
    untypedCumulatives = [bmd[bmd["Compared to Untyped Baseline"] <= x]["Compared to Untyped Baseline"].count()/(bmd["Compared to Untyped Baseline"].count()*1.0) for x in untypedOverheads]
    fig.append_trace(go.Scatter(None, x=untypedOverheads, y=untypedCumulatives,marker=markerstyles["Nom"],showlegend=True,line=linestyles["Nom"],legendgroup="cumulatives",name="Racket"),row=1,col=2)

langhandlers={}
langhandlers['monnom'] = load_monnom;
langhandlers['grift'] = load_grift;
langhandlers['racket'] = load_racket;
langhandlers['nom'] = load_nom;

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
    print(fig.layout)
    fig.layout.xaxis.autorange="reversed"
    fig.layout.yaxis.rangemode="tozero"
    fig.layout.yaxis.title="Running Time in Seconds"
    fig.layout.xaxis.title="Number of Steps to Fully Typed/Nominal"
    fig.layout.yaxis2.tickformat=",.0%"
    fig.layout.yaxis2.title="Configurations Below"
    if multilang:
      fig.layout.xaxis3.title="Number of Steps to Fully Typed/Nominal"
      fig.layout.xaxis3.autorange="reversed"
      fig.layout.yaxis3.title="Running Time in Seconds"
      fig.layout.yaxis3.rangemode="tozero"
      fig.layout.yaxis4.tickformat=",.0%"
      fig.layout.xaxis4.tickformat=",.0%"
      fig.layout.xaxis2.ticksuffix="x"
      fig.layout.yaxis4.title="Configurations Below"
    else:
      fig.layout.xaxis2.tickformat=",.0%"
    fig.layout.title.text=sys.argv[1]
    fig.layout.title.font.size=30
    fig.show()


