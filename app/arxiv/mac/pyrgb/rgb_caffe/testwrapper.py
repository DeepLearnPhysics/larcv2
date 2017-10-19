import os,sys
import numpy as np
import yaml
from ..lib.iomanager import IOManager
from .. import larcv

class TestWrapper(object):

    def __init__(self):
        self.name = "TestWrapper"

        # Configuration
        self.config = None

        # The caffe net -- should only get loaded once when
        # running forward for the first time
        self.net    = None

        # Already loaded caffe or not
        self.loaded  = False

        # pointer to the current image
        self.pimg    = None

        # iomanager instance
        self.iom     = None

        # did config change since last running?
        self.config_changed = None
        
        self.caffe = None

    def set_config(self,config):
        self.config_changed = True
        self.config = config


    def load_config(self):
        with open(self.config, 'r') as f:
            self.config = yaml.load(f)

        self.config_changed = False
        
    def load(self):
        self.load_config()
        print self.config['cafferoot']
        sys.path.insert(0,self.config['cafferoot'])
        import caffe
        self.caffe = caffe

        if self.config['usecpu'] :
            self.caffe.set_mode_cpu()
        else:
            self.caffe.set_mode_gpu()
            self.caffe.set_device(set.config['gpuid'])
        
        if "channels" in self.config:
            self.nchannels = self.config["channels"]
        else:
            self.nchannels = 3

        self.__generate_model__()
        self.__create_net__()
        
    def __create_net__(self):
        assert self.config is not None        
        self.net = self.caffe.Net( self.config['model'],
                                   self.config["pretrainedmodel"],
                                   self.caffe.TEST )
        
        
    def set_image(self,img):
        self.pimg = img

    def prep_image(self):

        assert self.pimg is not None

        im = self.pimg.astype(np.float32,copy=True)

        #load the mean_file:
        if self.iom is None:
            self.iom = IOManager([self.config['meanfile']])
            self.iom.set_verbosity(0)
            self.iom.read_entry(0)
            means  = self.iom.get_data(larcv.kProductImage2D,self.config['meanproducer'])
            self.mean_v = [ larcv.as_ndarray(img) for img in means.Image2DArray() ]
            print "Mean channels=",len(self.mean_v)," mean size=",self.mean_v[0].shape
            for ix,m in enumerate(self.mean_v): 
                print "means of mean {} : {}".format(ix,m.mean())


        for ix,mean in enumerate(self.mean_v):
            print "check mean shape againts: ",im[:,:,ix].shape
            assert mean.shape == im[:,:,ix].shape
            im[:,:,ix] -= mean
        
        #im[ im < self.config['imin'] ] = self.config['imin']
        #im[ im > self.config['imax'] ] = self.config['imax']
        
        return im
        
    def forward_result(self):

        if self.config_changed == True: self.load()
        self.config_changed = False

        blob = {'data' : None, 'label' : None}
        
        im = self.prep_image()
        
        print "FORWARD ON IMAGE: ",im.shape

        blob['data'] = np.zeros((1, im.shape[0], im.shape[1], im.shape[2]),dtype=np.float32)
        print blob['data'].shape

        blob['data'][0,:,:,:] = im

        channel_swap = (0, 3, 1, 2)
        blob['data'] = blob['data'].transpose(channel_swap)  

        print blob['data'].shape

        blob['label'] = np.zeros((1,),dtype=np.float32)
        
        self.net.blobs['data'].reshape(*(blob['data'].shape))
        self.net.blobs['label'].reshape(*(blob['label'].shape))
        
        forward_kwargs = {'data': blob['data'] ,'label': blob['label']}
        
        blobs_out = self.net.forward(**forward_kwargs)
        
        scores  =  self.net.blobs[ self.config['lastfc'] ].data

        self.scores = scores
        print "Scores:  {}".format(scores)

        if "save_datablob" in self.config and self.config["save_datablob"]==True:
            print "SAVING DATA BLOB TO FILE"
            # we dump the data array the network has in it's blob to a numpy binary file
            # this is useful for sanity-checks
            if not hasattr(self,'data_numpy_outfile'):
                self.data_numpy_outfile = file('saved_data_blobs.npz','w')
            data_blob = self.net.blobs["data"].data
            np.savez( self.data_numpy_outfile, data_blob )
        
        
    def __generate_model__(self):
        
        print "\t>> Got an image of shape: {}".format(self.pimg.shape)
        td = ""
        td += "input: \"data\"\n"
        td += "input_shape: { dim: 1 dim: %d dim: %s dim: %s } \n"%(self.nchannels,
                                                                    self.pimg.shape[0],
                                                                    self.pimg.shape[1])
        td += "input: \"label\"\n"
        td += "input_shape: { dim: 1 }"
        
        proto = None
        with open(self.config['modelfile'],'r') as f:
            proto = f.read()
        
        proto = td + proto
        fout = open(self.config['model'],'w+')
        fout.write(proto)
        fout.close()
