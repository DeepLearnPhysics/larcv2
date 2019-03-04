import os, sys
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

import numpy as np
import pandas as pd

from util.print_ import ana_stats

BASE_PATH = os.path.realpath(__file__)
BASE_PATH = os.path.dirname(BASE_PATH)

rse    = ['run','subrun','event']

name = str(sys.argv[1])
print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
print "~~~~~~~~~~~~~ Raw Output ~~~~~~~~~~~~~~~~"
print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"

all_df   = pd.read_pickle(os.path.join(BASE_PATH,"ll_bin","{}_all.pkl".format(name)))
event_df = all_df.groupby(rse).nth(0) 

scedr=5
LLCUT=None
ana_stats(all_df,event_df,scedr,name,LLCUT)

print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
print "~~~~~~~~~ Nue Assumption Output ~~~~~~~~~"
print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"

all_df   = pd.read_pickle(os.path.join(BASE_PATH,"ll_bin","{}_post_nue.pkl".format(name)))
scedr=5
LLCUT=None
ana_stats(all_df,event_df,scedr,name,LLCUT)

print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
print "~~~~~~~~~~~~~~ LL Output ~~~~~~~~~~~~~~~~"
print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"

all_df   = pd.read_pickle(os.path.join(BASE_PATH,"ll_bin","{}_post_LL.pkl".format(name)))
scedr=5
LLCUT=-14.625
ana_stats(all_df,event_df,scedr,name,LLCUT)

if name != 'nue':
    sys.exit(1)

#
# Kinematic Plots
#
from util.plot import eff_plot

ll_df = pd.read_pickle(os.path.join(BASE_PATH,"ll_bin","{}_post_LL.pkl".format(name)))

eff_plot(event_df.query("good_croi_ctr>0 & selected1L1P==1"),
         ll_df.query("good_croi_ctr>0 & LL>@LLCUT & scedr<@scedr & selected1L1P==1"),
         200,800,20,
         "energyInit >= @Xmin & energyInit <= @Xmax","energyInit",
         "energyInit [MeV]","Count / 20 [MeV]",
         "energyInit")

for df in [event_df,ll_df]:
    p0_px = df['daughterPx_v'].str[0].values
    p0_py = df['daughterPy_v'].str[0].values
    p0_pz = df['daughterPz_v'].str[0].values
    p0_cosYZ = p0_pz / np.sqrt( p0_pz*p0_pz + p0_py*p0_py )

    p1_px = df['daughterPx_v'].str[1].values
    p1_py = df['daughterPy_v'].str[1].values
    p1_pz = df['daughterPz_v'].str[1].values
    p1_cosYZ = p1_pz / np.sqrt( p1_pz*p1_pz + p1_py*p1_py )

    p01_dot  = p0_px * p1_px + p0_py * p1_py + p0_pz * p1_pz
    p01_dot /= np.sqrt(p0_px*p0_px + p0_py*p0_py + p0_pz*p0_pz)
    p01_dot /= np.sqrt(p1_px*p1_px + p1_py*p1_py + p1_pz*p1_pz)

    df['P0ThetaYZ'] = np.nan_to_num(np.arccos(p0_cosYZ))
    df['P1ThetaYZ'] = np.nan_to_num(np.arccos(p1_cosYZ))
    df['P01Theta']  = np.nan_to_num(np.arccos(p01_dot))

eff_plot(event_df.query("good_croi_ctr>0 & selected1L1P==1"),
         ll_df.query("good_croi_ctr>0 & LL>@LLCUT & scedr<@scedr & selected1L1P==1"),
         0,3.14,3.14/20.,
         "energyInit >= 200 & energyInit <= 800","P0ThetaYZ",
         "P0ThetaYZ [rad]","Count / pi/20 [rad]",
         "P0ThetaYZ")

eff_plot(event_df.query("good_croi_ctr>0 & selected1L1P==1"),
         ll_df.query("good_croi_ctr>0 & LL>@LLCUT & scedr<@scedr & selected1L1P==1"),
         0,3.14,3.14/20.,
         "energyInit >= 200 & energyInit <= 800","P1ThetaYZ",
         "P1ThetaYZ [rad]","Count / pi/20 [rad]",
         "P1ThetaYZ")

eff_plot(event_df.query("good_croi_ctr>0 & selected1L1P==1"),
         ll_df.query("good_croi_ctr>0 & LL>@LLCUT & scedr<@scedr & selected1L1P==1"),
         0,3.14,3.14/20.,
         "energyInit >= 200 & energyInit <= 800","P01Theta",
         "P01Theta [rad]","Count / pi/20 [rad]",
         "P01Theta")
         
#
# Likelihood Plots

from util.plot import histo1, histo2, eff_scan

histo1(ll_df,
       -50,0,1.0,
       None,"LL",
       "LL","Fraction of Events",
       "LL")

histo2(ll_df,
       -50,0,1.0,
       "scedr>5","scedr<=5","LL",
       "LL","Fraction of Events",
       "LL_scedr")

eff_scan(ll_df.query("scedr>@scedr"),
         ll_df.query("scedr<=@scedr"),
         -50,0,0.5,
         "LL",
         "LL","Efficiency",
         "LL",)

