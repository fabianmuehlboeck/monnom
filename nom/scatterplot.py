import csv
import sys
import matplotlib.pyplot as plt

colors = ["red", "blue", "green", "yellow", "purple", "brown"]
markers = ['x', 'D', '*', '^', 'o', '+']
langs = []
langcolors = {}
langmarkers = {}
defaultwidth = 0.8
spacing=0.2
plotdatas = 0

class PlotData:
    def __init__(self, file, lang):
        global plotdatas
        plotdatas += 1
        self.file = file
        self.lang = lang
        self.configs = []
        self.times = {}
        self.timingscounts = {}
        with open(file, 'r') as csvfile:
            dreader = csv.DictReader(csvfile, delimiter = ',', quotechar="'")
            for row in dreader:
                if('Seconds' not in row):
                    continue
                if(row['Seconds']==None):
                    continue
                if(len(row['Seconds'])==0):
                    continue
                if(row['Folder'] not in self.configs):
                    self.configs.append(row['Folder'])
                    self.timingscounts[row['Folder']]=0
                    self.times[row['Folder']] = 0.0
                self.times[row['Folder']] += float(row['Seconds'])
                self.timingscounts[row['Folder']] += 1
        for config in self.configs:
            self.times[config] /= self.timingscounts[config]
        self.configs.sort()

    def drawScatter(self, ax, offset=0):
        global langs, plotdatas, langcolors
        if self.lang not in langs:
            langcolors[self.lang] = colors[len(langs)]
            langmarkers[self.lang] = markers[len(langs)]
            langs.append(self.lang)
        ax.scatter([len(config.replace("0",""))+offset for config in self.configs], [self.times[config] for config in self.configs], color=langcolors[self.lang], marker=langmarkers[self.lang], label=self.lang)

if(len(sys.argv)>2):
    datas = []
    for i in range(2,len(sys.argv),2):
        datas.append(PlotData(sys.argv[i],sys.argv[i+1]))
    fig, ax = plt.subplots()
    posns = [0]
    if len(datas) > 1:
        posns = [-.3+(i*0.6/(len(datas)-1)) for i in range(len(datas))]
    counter=0
    for data in datas:
        data.drawScatter(ax, posns[counter])
        counter+=1
    #plt.xticks([x*plotdatas*(defaultwidth+spacing) + (plotdatas*defaultwidth/2) for x in range(len(datas[0].configs))], datas[0].configs)
    ax.legend(loc='upper right', shadow=True, fontsize='large')
    ax.set_xlabel("# of Typed Configurations")
    ax.set_ylabel("Running time in seconds")
    plt.xlim([-.4,max([len(config) for config in datas[0].configs])+.4])
    plt.ylim([0,max([max(data.times.values()) for data in datas])*1.05])
    if(sys.argv[1]=="SHOW"):
        plt.show()
    else:
        plt.savefig(sys.argv[1])
