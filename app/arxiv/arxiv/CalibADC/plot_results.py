import os,sys
import ROOT as rt
import numpy as np

mc_scale = [1.0,1.0,1.0]
#global_scales = [1.235,0.703,0.95] # (data/mc)
global_scales = [1.0,1.0,1.0]
mc_scale = global_scales # if want to plot aligned
sigmas = [40,30,20]
nentries = 800

#fdata = rt.TFile("adc_scales_data_cosmics.root")
#fdata = rt.TFile("test_out_data_analysis.root")
#fmc   = rt.TFile("adc_scales_mc_cosmics.root")
fdata = rt.TFile("extbnb_v05_fitted.root")
fmc   = rt.TFile("mc_v05_fitted.root")

cch = rt.TCanvas("cch","cch", 1200,1200)
cch.Divide(1,3)

fout = open("chbych_scaling.txt",'w')
print>>fout,"plane\tchannel\tdata\tmc\tscale\tbadmask"

graphs = []
planenames = {0:"U",
              1:"V",
              2:"Y"}
chscalings = {} # key is (p,ch)
badmask    = {} # key is (p,ch)
lowerbounds = [ 50, 50, 50 ]
midbounds   = [ 70, 70, 70 ]
for p in range(0,3):
    cch.cd(p+1).SetGridx(1)
    cch.cd(p+1).SetGridy(1)
    gdata = fdata.Get("gmean_plane%d"%(p))
    gmc   = fmc.Get("gmean_plane%d"%(p))
    gmc.SetLineColor(rt.kRed)
    gdata.Draw("AL")
    gmc.Draw("L")
    gdata.SetTitle("Plane %d (%s);wire pixel number;peak ADC centroid"%(p,planenames[p]))
    for ch in range(0,900):
        mc = gmc.GetY()[ch]
        data = gdata.GetY()[ch]

        hdata = fdata.Get("hadc_p%d_ch%d"%(p,ch))
        mcbin = hdata.GetXaxis().FindBin( mc )
        lowbin = hdata.GetXaxis().FindBin( lowerbounds[p] )
        midbin = hdata.GetXaxis().FindBin( midbounds[p] )
        hdata.GetXaxis().SetRange( lowbin, midbin )
        dmax = hdata.GetMaximum()
        dmaxbin = hdata.GetMaximumBin()
        dminbin = hdata.GetMinimumBin()
        hdata.GetXaxis().SetRange( dminbin, 100 )
        dmaxbin2 = hdata.GetMaximumBin()
        hdata.GetXaxis().SetRange( dmaxbin2, 100 )
        dminbin2 = hdata.GetMinimumBin()
        dmaxf = hdata.GetXaxis().GetBinCenter( dmaxbin2 )
        #print "[",(p,ch),"]: ",dminbin,dmaxbin2,dminbin2
        if dminbin < dmaxbin2 and dmaxbin2 < dminbin2:
            badmask[(p,ch)] = 1
        else:
            badmask[(p,ch)] = 0

        # check for refit
        hdata.GetXaxis().SetRange( dminbin, 100 )
        if (p==0 and (data>190 or data<120)) or (p==1 and (data>200 or data<90)):
            print "refit plane ",p,", ch=",ch,
            if hdata.Integral()<500:
                gdata.SetPoint( ch, gdata.GetX()[ch], 0.0 )
                data = 0.0
                print
            else:
                f1 = rt.TF1("refit_p%d_ch%d"%(p,ch),"gaus")
                f1.SetParameter(1,dmaxf)
                f1.SetParameter(2,40)
                f1.SetParameter(0,hdata.GetBinContent(dmaxbin2))
                fitmin = dmaxf-100
                fitmax = dmaxf+100
                if fitmin<hdata.GetXaxis().GetBinCenter(dminbin):
                    fitmin = hdata.GetXaxis().GetBinCenter(dminbin)
                hdata.Fit(f1,"RQ0","",fitmin,fitmax)
                mean = f1.GetParameter(1)
                print ": data=",data," fit=",mean," mcmean=",mc," dmax=",dmax," dmaxbin=",dmaxbin," dmaxf=",dmaxf
                gdata.SetPoint( ch, gdata.GetX()[ch], mean )
                data = mean
                if data<0:
                    data = 0.0

        if mc==0:
            chscalings[(p,ch)] = 0.0
        else:
            #if (p==1 and data<100) or (p in [0,2] and data<150):
            #    global_scales[p]
            #chscalings[(p,ch)] = data/mc
            chscalings[(p,ch)] = data/(mc*mc_scale[p])
        print>>fout,"%d\t%d\t%.2f\t%.2f\t%.2f\t%d"%(p,ch,data,mc,chscalings[(p,ch)],badmask[(p,ch)])
        hdata.GetXaxis().SetRange(1,500)
    gdata.GetYaxis().SetRangeUser(0,220)

cch.Draw()
cch.Update()

fout.close()

# Make total histogram from summing wire histograms

mchists = {} # key by plane
datahists = {} # key by plane

# find first existant histogram
for p in range(0,3):
    # MC
    for x in range(0,10):
        h = fmc.Get( "hadc_p%d_ch%d"%(p,x) )
        try:
            mchists[p] = h.Clone( "htot_plane%d"%(p) )
            mchists[p].Reset()
            break
        except:
            pass
    # DATA
    for x in range(0,10):
        h = fdata.Get( "hadc_p%d_ch%d"%(p,x) )
        try:
            datahists[p] = h.Clone( "htot_plane%d"%(p) )
            datahists[p].Reset()
            break
        except:
            pass

for p in range(0,3):
    for x in range(0,900):
        h = fmc.Get( "hadc_p%d_ch%d"%(p,x) )
        hd = fdata.Get( "hadc_p%d_ch%d"%(p,x) )
        try:
            mchists[p].Add( h )
            datahists[p].Add( hd )
        except:
            pass
    norm = 1.0/mchists[p].Integral()
    mchists[p].Scale(norm)
    norm = 1.0/datahists[p].Integral()
    datahists[p].Scale(norm)

ctot = rt.TCanvas("ctot","ctot",800,900)
ctot.Divide(1,3)
for p in range(0,3):
    ctot.cd(p+1)
    mchists[p].Draw()
    #mchists[p].GetYaxis().SetRangeUser(0,220)
    datahists[p].Draw("same")
    datahists[p].SetLineColor(rt.kBlack)
    mchists[p].SetLineColor(rt.kRed)

ctot.Draw()
ctot.Update()

raw_input()


