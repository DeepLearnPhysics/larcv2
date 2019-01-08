import sys
import os
import time
import copy
from collections import OrderedDict

import numpy
from mpi4py import MPI

from larcv.dataloader2 import larcv_threadio

class larcv_interface(object):

    def __init__(self, verbose=False, root=0, comm=MPI.COMM_WORLD, distribute_to_root=True):
        object.__init__(self)

        # MPI parameters:
        self._comm = comm
        self._size = comm.Get_size()
        self._rank = comm.Get_rank()
        self._root = root

        # This option controls whether or not to distrubute data to the root process
        self._distribute_to_root = distribute_to_root

        # Parameters based from the non MPI version:
        self._dims        = {}
        self._dataloaders = {}
        self._data_keys   = {}
        self._verbose     = verbose
        
        # These are used to correctly scatterv the data:
        self._counts      = {}
        self._displs      = {}
        self._dtypes      = {}

        # These ONLY get populated on the root node:
        self._raw_dims    = {}
        self._raw_dtypes  = {}

        self._datasize    = {}

        return


    def prepare_manager(self, mode, io_config, minibatch_size, data_keys):
        '''
        This has to be called on all ranks, not just the root rank
        '''

        # If we're on the root rank, we can prepare the interface and broadcast the dimensions.
        # Otherwise, just receive the dimensions:

        # Store the keys for accessing this datatype:
        # (This gets passed on every rank)

        if type(data_keys) is not OrderedDict:
            raise Exception("Please pass the data keys via OrderedDict, to ensure mpi messaging order is well defined.")


        self._data_keys[mode] = data_keys


        if self._rank == self._root:
    
            if mode in self._dataloaders:
                raise Exception("Can not prepare manager for mode {}, already exists".format(mode))

            # Check that the required keys are in the io config:
            for req in ['filler_name', 'verbosity', 'filler_cfg']:
                if req not in io_config:
                    raise Exception("io_config for mode {} is missing required key {}".format(mode, req))



            start = time.time()

            # Initialize and configure a manager:
            io = larcv_threadio()
            io.configure(io_config)
            io.start_manager(minibatch_size)

            # Save the manager
            self._dataloaders.update({mode : io})
            self._dataloaders[mode].next()


    
            # Read and save the dimensions of the data:
            # As the root processes (for IO), set the dims and then distribute to everyone:
            self._raw_dims[mode]   = {}
            self._raw_dtypes[mode] = {}
            for key in self._data_keys[mode]:
                self._raw_dims[mode][key]   = self._dataloaders[mode].fetch_data(self._data_keys[mode][key]).dim()
                self._raw_dtypes[mode][key] = self._dataloaders[mode].fetch_data(self._data_keys[mode][key]).data().dtype

            # Print out how long it took to start IO:
            if self._verbose:
                sys.stdout.write("Time to start {0} IO: {1:.2}s\n".format(mode, end - start))



        # call set dims to distribute the dimensions to everyone:
        self._set_dims(mode)


        return


    def _set_dims(self, mode):
        ''' 
        This function is used to broadcast the shape and dimensions 
        of the data used.  It is called automatically from prepare manager,
        do not call it yourself.
        '''

        if self._rank != self._root:
            dims = None
        else:
            # the number of events per worker needs to evenly divide the number
            # of batches for this to succeed.  So, check and adjust the dims:
            first_key = next(iter(self._data_keys[mode]))
            n_raw_batch_size = self._raw_dims[mode][first_key][0]

            # How many worker nodes depends on whether or not we're distributing to the root node:
            n_worker_nodes = self._size if self._distribute_to_root else self._size - 1

            if n_raw_batch_size % n_worker_nodes != 0:
                raise Exception("Requested to broadcast {} images to {} workers, please adjust to distribute evenly".format(
                    n_raw_batch_size, n_worker_nodes))
            
            # Make a copy of the raw dimensions so we can scale it down for broadcasting:
            dims = copy.deepcopy(self._raw_dims[mode])
            for key in self._data_keys[mode]:
                dims[key][0] /= n_worker_nodes

        # Actually broadcast the dimensions:
        dims = self._comm.bcast(dims, root=self._root)
        # Note: this is relying on pickling the dimensions, which is a dict of numpy arrays
        # It's slow, yes, but the data is so small that it shoul not be a big overhead.

        # And store them:
        self._dims[mode] = dims

        # Broadcast the total size of the dataset:
        if self._rank == self._root:
            datasize = self._dataloaders[mode].fetch_n_entries()
        else:
            datasize = 0

        self._datasize[mode] = self._comm.bcast(datasize, root = self._root)

        # Distribute the counts and displacements necessary for scatterv
        self._counts[mode] = {}
        self._displs[mode] = {}

        # Calculate the total size of each message recieved by each node, 
        # And it's displacement from the start of the total message

        for key in self._data_keys[mode]:
            size = int(numpy.prod(self._dims[mode][key]))
            # Size of the message is just the produce of the worker batch dims
            self._counts[mode][key] = numpy.ones((self._size), numpy.int)*size
            
            # If not distributing to root, the root node gets nothing:
            if not self._distribute_to_root:
                self._counts[mode][key][self._root] = 0

            # The displacements is just the sum of the counts, but started from 0
            self._displs[mode][key] = numpy.cumsum(self._counts[mode][key]) - size
        
        # Lastly, distribute the dtypes necessary for scatterv to work properly:
        if self._rank != self._root:
            dtypes = None
        else:
            dtypes = self._raw_dtypes[mode]

        self._dtypes[mode]= self._comm.bcast(dtypes, root=self._root)
        
        return 

    def read_and_distribute_data(self, mode, unscattered_data=None):
        '''
        Read the next batch of data from the interface (which, presumably, is 
        threaded so the data is ready to go when this is called) and distribute it
        to the MPI Comm specified (Default = MPI.COMM_WORLD)
        '''

        # We have the private data only on the root rank:
        private_data = {}

        for key in self._data_keys[mode]:

            if key not in unscattered_data:
                continue

            # Create a buffer for the data to scatter
            # Which is just a reference to the numpy array
            sendbuff = None
            if self._rank==self._root:
                sendbuff = unscattered_data[key]
                
            # The recvbuff must be properly sized:
            recvbuff = numpy.empty((self._counts[mode][key][self._rank]), dtype=self._dtypes[mode][key])

            # Scatterv will scatter numpy arrays, and note the automatic lookup of 
            # dtypes from numpy to MPI.  If you are getting crazy, undefined or NaN values,
            # the dtype is a good start for investigating.
            self._comm.Scatterv([ sendbuff,
                                  self._counts[mode][key], 
                                  self._displs[mode][key],
                                  MPI._typedict[self._dtypes[mode][key].char]],
                                recvbuff,
                                root=self._root)


            # Reshape the data (all ranks except root, unless distributing to root)
            if self._distribute_to_root or self._rank != self._root:                
                private_data[key] = numpy.reshape(recvbuff, self._dims[mode][key])
            else:
                private_data[key] = None

        return private_data

    def finish(self):
        if self._rank == self._root:
            for mode in self._dataloaders:
                self._dataloaders[mode].stop_manager()


    def fetch_minibatch_data(self, mode, fetch_meta_data=False):
        '''
        Read data from disk and distribute it to all other workers.
        '''

        # sys.stdout.write("Rank {} starting to fetch minibatch data\n".format(self._rank))

        # If this is the root node, read the data from disk:
        if self._rank == self._root:
            unscattered_data = {}
            for key in self._data_keys[mode]:
                unscattered_data[key] = self._dataloaders[mode].fetch_data(self._data_keys[mode][key]).data()
        else:
            unscattered_data = {}
            for key in self._data_keys[mode]:
                unscattered_data[key]  = None


        this_data = self.read_and_distribute_data(mode, unscattered_data)


        if self._rank == self._root:
            self._dataloaders[mode].next()

        return this_data

    def fetch_minibatch_dims(self, mode):
        # Return a dictionary object with keys 'image', 'label', and others as needed
        # self._dataloaders['train'].fetch_data(keyword_label).dim() as an example
        return self._dims[mode]

    def stop(self):
        if self._rank == self._root:
            for mode in self._dataloaders:
                while self._dataloaders[mode].is_reading():
                    time.sleep(0.01)
                self._dataloaders[mode].stop_manager()


    def size(self, mode):
        return self._datasize[mode]
