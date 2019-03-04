from larcv import larcv
from larcv.dataloader2 import larcv_threadio

def start():

    #proc=larcv_threadio()
    #cfg = {'filler_name' : 'ThreadProcessor',
    #       'filler_cfg'  : 'test_voxel3d.cfg',
    #       'verbosity'   : 0}
    #proc.configure(cfg)
    proc = larcv.ThreadProcessor("ThreadProcessor")
    proc.configure('test_voxel3d.cfg')
    proc.start_manager(5)
    return proc

