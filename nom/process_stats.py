import sys
import os
import os.path
from os import listdir
from os.path import isfile
from os.path import join
import matplotlib.pyplot as plt
keys = []
datas = {}
configs = []

def process(benchmark, statsfile):
    for bmdir in [d for d in listdir(benchmark+"/benchmark") if not isfile(join(benchmark+"/benchmark/",d))]:
        fname = benchmark + "/benchmark/" + bmdir + "/"+statsfile
        if(isfile(fname)):
            datas[bmdir] = {}
            configs.append(bmdir)
            with open(fname, 'r') as file:
                for line in file:
                    if(len(line.strip())>0):
                        parts = line.split(':')
                        if parts[0] not in keys:
                            keys.append(parts[0])
                        datas[bmdir][parts[0]] = int(parts[1].strip())
    configs.sort()

def draw_bars(ax, key):
    ax.bar(range(len(configs)), [datas[config][key] for config in configs])
    plt.xticks(range(len(configs)), configs)
    ax.set_xlabel("Configuration (0 = untyped, 1 = typed)")
    ax.set_ylabel(key)

def draw_scatter(ax, key):
    ax.scatter([len(config.replace("0","")) for config in configs], [datas[config][key] for config in configs], marker = "x")
    plt.xticks(range(max([len(config) for config in configs])),range(max([len(config) for config in configs])))
    ax.set_xlabel("# of Typed Modules")
    ax.set_ylabel(key)

if(len(sys.argv)>4):
    process(sys.argv[2], sys.argv[3])
    fig, ax = plt.subplots()
    if(len(configs)<=16):
        draw_bars(ax, keys[int(sys.argv[4])])
    else:
        draw_scatter(ax, keys[int(sys.argv[4])])
    if(sys.argv[1]=="SHOW"):
        plt.show()
    else:
        plt.savefig(sys.argv[1])
