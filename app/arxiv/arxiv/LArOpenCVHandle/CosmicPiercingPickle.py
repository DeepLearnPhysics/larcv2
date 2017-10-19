import ROOT
from larcv import larcv
import pickle

inputFiles = []

ioman = larcv.IOManager(larcv.IOManager.kREAD)

for infil in inputFiles:
    ioman.add_in_file(infil)
ioman.initialize()

cosmicDict = {}
for evt in range(ioman.get_n_entries()):
    ioman.read_entry(evt)
    clus = ioman.get_data(larcv.kProductPixel2D,"topspacepts")
    numPierce = clus.Pixel2DArray()[0].size()
    cosmicDict[tuple((clus.run(),clus.subrun(),clus.event()))] = numPierce

with open("topPierceDict.pickle","wb") as handle: pickle.dump(cosmicDict,handle,protocol=pickle.HIGHEST_PROTOCOL)
    
