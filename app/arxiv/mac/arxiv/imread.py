from larcv import larcv

# instantiate IO manager
io = larcv.IOManager(larcv.IOManager.kWRITE)
# set output file name
io.set_out_file('imread.root')
# initialize IO manager
io.initialize()
# create & retrieve a Image2D array data product from IO manager
image2d_array = io.get_data(larcv.kProductImage2D,'imread')
# read-in image file to create one Image2D instance
image2d = larcv.imread('sakura.jpg')
# insert one Image2D instance into an array
image2d_array.Append(image2d)
# set "event id" (must be set to avoid exception)
io.set_id(0,0,0)
# save this event entry
io.save_entry()
# finalize = close output file
io.finalize()
