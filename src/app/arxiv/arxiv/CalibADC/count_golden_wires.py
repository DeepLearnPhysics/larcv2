import os,sys
import ROOT as rt
import numpy as np

chbych = open("chbych_scaling.txt",'r')

goodranges = {0:[0.8,1.4],
              1:[0.5,1.0],
              2:[0.8,1.2]}

ngood = {0:0,
         1:0,
         2:0}

scalehists = {}
peakmeans = {}
for p in range(0,3):
    scalehists[p] = rt.TH1D("hscale_p%d"%(p),"",100,0,2.0)
    peakmeans[("mc",p)]  = rt.TH1D("hpeak_mc_p%d"%(p),"", 220, 0, 220)
    peakmeans[("data",p)]  = rt.TH1D("hpeak_data_p%d"%(p),"", 220, 0, 220)

lchbych = chbych.readlines()
#['2', '863', '368.49', '181.10', '2.03', '1']

badchlist = open('nongolden_channels.txt','w')
planes = {0:"U",
          1:"V",
          2:"Y"}
maxwires = {0:2400,1:2400,2:3456}
for l in lchbych[1:]:
    info =  l.strip().split()
    datap = float( info[2] )
    mcp   = float( info[3] )
    scale = float( info[4] )
    badmask = int( info[5] )
    plane = int( info[0] )
    wid   = int( info[1] )

    

    if badmask==1:
        scalehists[plane].Fill( scale )
        peakmeans[("mc",plane)].Fill( mcp )
        peakmeans[("data",plane)].Fill( datap )
    else:
        scalehists[plane].Fill( 0.0 )

    if scale>=goodranges[plane][0] and scale<=goodranges[plane][1]:
        ngood[plane] += 1
    else:
        for wire in range( wid*4, (wid+1)*4 ):
            if wire < maxwires[plane]:
                print >>badchlist,wire,'\t',planes[plane]

badchlist.close()

print "NGOOD:"
print "U-plane: ",ngood[0],"(",ngood[0]*4.0/2400.0*100.0,"%%)"
print "V-plane: ",ngood[1],"(",ngood[1]*4.0/2400.0*100.0,"%%)"
print "Y-plane: ",ngood[2],"(",ngood[2]*4.0/3456.0*100.0,"%%)"

print "TOTAL: ",ngood[0]+ngood[1]+ngood[2]," (",4.0*(ngood[0]+ngood[1]+ngood[2])/(2400+2400+3456.0)*100.0,"%%)"

# Fit scale distributions

print "SCALE FACTOR PEAK"
for p in range(0,3):
    scalehists[p].Fit("gaus","R","",0.3, 1.5)

print "MC PEAKS"
for p in range(0,3):
    peakmeans[("mc",p)].Fit("gaus","R","",50, 200)

print "DATA PEAKS"
for p in range(0,3):
    peakmeans[("data",p)].Fit("gaus","R","",50, 200)



c = rt.TCanvas("c","c",800,600)
c.Draw()
c.SetLogy(1)
c.SetGridx(1)
c.SetGridy(1)

scalehists[0].Draw()
scalehists[1].Draw("same")
scalehists[2].Draw("same")

scalehists[0].SetLineColor(rt.kRed)
scalehists[1].SetLineColor(rt.kGreen)
scalehists[2].SetLineColor(rt.kBlue)

c.Update()

# cp = rt.TCanvas("cp","cp",800,600)
# cp.Draw()
# cp.SetLogy(1)
# cp.SetGridx(1)
# cp.SetGridy(1)

# scalehists[0].Draw()
# scalehists[1].Draw("same")
# scalehists[2].Draw("same")

# scalehists[0].SetLineColor(rt.kRed)
# scalehists[1].SetLineColor(rt.kGreen)
# scalehists[2].SetLineColor(rt.kBlue)

# c.Update()

c.SaveAs("ScaleFactorsDist.png")

raw_input()
    
