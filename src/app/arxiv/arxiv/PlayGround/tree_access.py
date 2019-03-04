from larcv import larcv
from ROOT import TChain

treename = "image2d_tpc_tree"
filename = "supera_mc_muminus.root"

# Create TChain
ch=TChain(treename)

# Load file
ch.AddFile(filename)

# Get # entries
print "How many entries?", ch.GetEntries()

# Get entry 0
ch.GetEntry(0)

# EventImage2D object
br = None
exec('br = ch.%s' % treename.replace('_tree','_branch'))
print "EventImage2D object pointer:", br

# Let's get actual array of Image2D
img_arr = br.Image2DArray()

# Loop over individual Image2D
for img in img_arr:

    print img
    # Convert to Python ndarray
    print larcv.as_ndarray(img).shape

