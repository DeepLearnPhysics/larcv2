import os,sys,copy
"""
This code is here to dump the start, end wire table store in the ROOT file format into 
a static, const c++ array the DividerAlgo can load on start.
"""

"""
******************************************************************************
*Tree    :wireInfo  : Wire Geo Info                                          *
*Entries :     8256 : Total =          267376 bytes  File  Size =      72869 *
*        :          : Tree compression factor =   3.68                       *
******************************************************************************
*Br    0 :plane     : plane/I                                                *
*Entries :     8256 : Total  Size=      33650 bytes  File Size  =        386 *
*Baskets :        2 : Basket Size=      32000 bytes  Compression=  85.95     *
*............................................................................*
*Br    1 :wireID    : wireID/I                                               *
*Entries :     8256 : Total  Size=      33656 bytes  File Size  =      11542 *
*Baskets :        2 : Basket Size=      32000 bytes  Compression=   2.87     *
*............................................................................*
*Br    2 :start     : start[3]/F                                             *
*Entries :     8256 : Total  Size=      99856 bytes  File Size  =      30375 *
*Baskets :        4 : Basket Size=      32000 bytes  Compression=   3.27     *
*............................................................................*
*Br    3 :end       : end[3]/F                                               *
*Entries :     8256 : Total  Size=      99840 bytes  File Size  =      29822 *
*Baskets :        4 : Basket Size=      32000 bytes  Compression=   3.33     *
*............................................................................*
"""

import ROOT as rt
import numpy as np

fin = rt.TFile("dat/imagedivider_y216.root")
wireInfo = fin.Get("imagedivider/wireInfo")

nentries =  wireInfo.GetEntries()

ustart = {}
vstart = {}
ystart = {}

uend = {}
vend = {}
yend = {}

for entry in range(nentries):
    wireInfo.GetEntry(entry)

    if wireInfo.plane==0:
        sdict = ustart
        edict = uend
    elif wireInfo.plane==1:
        sdict = vstart
        edict = vend
    elif wireInfo.plane==2:
        sdict = ystart
        edict = yend

    wID = copy.copy(wireInfo.wireID)
    arr = np.zeros( 3 )
    arr[0] = wireInfo.start[0]
    arr[1] = wireInfo.start[1]
    arr[2] = wireInfo.start[2]
    sdict[ wID ] = arr

    arr2 = np.zeros( 3 )
    arr2[0] = wireInfo.end[0]
    arr2[1] = wireInfo.end[1]
    arr2[2] = wireInfo.end[2]
    edict[ wID ] = arr2


print "entries: ",nentries

header1 = """
#ifndef __WIREINFODATA__
#define __WIREINFODATA__
// this file generated using dump_wire_header.py

namespace divalgo {

  static const int numWiresU = %d;
  static const int numWiresV = %d;
  static const int numWiresY = %d;
"""%(wireInfo.GetEntries("plane==0"),
     wireInfo.GetEntries("plane==1"),
     wireInfo.GetEntries("plane==2"))

startdata = ""
for plane,data in [('u',ustart),('v',vstart),('y',ystart)]:
    startdata += "  static const float %splaneStart[%d][3] = \n"%(plane,len(data))
    startdata += "  {"
    wids = data.keys()
    wids.sort()
    for wid in wids:
        arr = data[wid]
        startdata += "   {%.2f,%.2f,%.2f}, // %s plane,wire=%d\n"%(arr[0],arr[1],arr[2],plane,wid)
    startdata += "  };"
    startdata += "\n"

enddata = ""
for plane,data in [('u',uend),('v',vend),('y',yend)]:
    enddata += "  static const float %splaneEnd[%d][3] = \n"%(plane,len(data))
    enddata += "  {"
    wids = data.keys()
    wids.sort()
    for wid in wids:
        arr = data[wid]
        enddata += "   {%.2f,%.2f,%.2f}, // %s plane,wire=%d\n"%(arr[0],arr[1],arr[2],plane,wid)
    enddata += "  };"
    enddata += "\n"


#  static const wireInfoData[


header2 = """

  void getUstart( const int wireid, std::vector<float>& pos );
  void getUend( const int wireid, std::vector<float>& pos );
  void getVstart( const int wireid, std::vector<float>& pos );
  void getVend( const int wireid, std::vector<float>& pos );
  void getYstart( const int wireid, std::vector<float>& pos );
  void getYend( const int wireid, std::vector<float>& pos );

} //< end of namespace

#endif
"""

out = open("WireInfoData.h",'w')
header = header1 + '\n' + startdata + "\n" + enddata + '\n' + header2 + '\n'


print >>out,header

out.close()
