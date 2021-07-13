import csv
import sys
import matplotlib.pyplot as plt

colors = ["red", "blue", "green", "yellow", "purple", "brown"]
langs = []
langcolors = {}
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
                if(row['Folder'] not in self.configs):
                    self.configs.append(row['Folder'])
                    self.timingscounts[row['Folder']]=0
                    self.times[row['Folder']] = 0.0
                self.times[row['Folder']] += float(row['Seconds'])
                self.timingscounts[row['Folder']] += 1
        for config in self.configs:
            self.times[config] /= self.timingscounts[config]
        self.configs.sort()

    def drawBar(self, ax, startx=0):
        global langs, plotdatas, langcolors
        if self.lang not in langs:
            langcolors[self.lang] = colors[len(langs)]
            langs.append(self.lang)
        ax.bar([langs.index(self.lang)*defaultwidth + x*plotdatas*(defaultwidth+spacing) for x in range(len(self.configs))], [self.times[config] for config in self.configs], defaultwidth, color=langcolors[self.lang], label=self.lang)

if(len(sys.argv)>2):
    datas = []
    for i in range(2,len(sys.argv),2):
        datas.append(PlotData(sys.argv[i],sys.argv[i+1]))
    fig, ax = plt.subplots()
    for data in datas:
        data.drawBar(ax)
    plt.xticks([x*plotdatas*(defaultwidth+spacing) + (plotdatas*defaultwidth/2) for x in range(len(datas[0].configs))], datas[0].configs)
    ax.legend(loc='upper right', shadow=True, fontsize='large')
    ax.set_xlabel("Configuration (0 = untyped, 1 = typed)")
    ax.set_ylabel("Running time in seconds")
    if(sys.argv[1]=="SHOW"):
        plt.show()
    else:
        plt.savefig(sys.argv[1])


