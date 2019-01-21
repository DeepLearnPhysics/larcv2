import sys
import os
import time

import numpy

from larcv.dataloader2  import larcv_threadio
from larcv.larcv_writer import larcv_writer


class larcv_interface(object):
    '''
    This class is just a wrapper around the dataloaders and threadio.  You should not feel
    pressured to use it, the threadio python classes work perfectly well.  However, 
    this class implements the same interface as the distributed_larcv_interface class
    which wraps larcv threadio in mpi4py to allow distributed training. See that class for
    more details.

    The bottom line is, if you want to use distributed_larcv_interface, you can use this class
    for a nearly drop-in replacement in prototyping in environments without  MPI, just on a single
    node.
    '''

    def __init__(self,verbose=False):
        '''init function
        
        Not much to store here, just a dict of dataloaders and the keys to access their data.
        '''
        object.__init__(self)
        self._dataloaders = {}
        self._data_keys   = {}
        self._dims        = {}
        self._verbose     = verbose
        self._writer      = None


    def prepare_writer(self, io_config, output_file=None):

        if self._writer is not None:
            raise Exception("larcv_interface doesn't yet support multiple writers.")

        # This only supports batch datasize of 1.  We can check that the reading instance
        # Has only size 1 by looking at the dims.

        key =list(self._data_keys['primary'].items())[0][0]
        if self._dims['primary'][key][0] != 1:
            raise Exception("To use the writing interface, please set batch size to 1.")

        # The writer is not an instance of threadIO but rather an instance of larcv_writer.

        # It configures a process to copy input to output and add more information as well.

        self._writer = larcv_writer(io_config, output_file)

        pass


    def prepare_manager(self, mode, io_config, minibatch_size, data_keys):
        '''Prepare a manager for io
        
        Creates an instance of larcv_threadio for a particular file to read.
        
        Arguments:
            mode {str} -- The mode of training to store this threadio under (typically "train" or "TEST" or similar)
            io_config {dict} -- the io config dictionary.  Required keys are: 'filler_name', 'verbosity', and 'filler_cfg'
            data_keys_override {dict} -- If desired, you can override the keys for dataacces,
        
        Raises:
            Exception -- [description]
        '''


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
        self._dataloaders.update({mode : io})
        # Force storing here, since it's not configurable on the first read.
        self.next(mode)

        while io.is_reading():
            time.sleep(0.01)

        # Save the manager

        # Store the keys for accessing this datatype:
        self._data_keys[mode] = data_keys
    
        # Read and save the dimensions of the data:
        self._dims[mode] = {}
        for key in self._data_keys[mode]:
            self._dims[mode][key] = self._dataloaders[mode].fetch_data(self._data_keys[mode][key]).dim()

        end = time.time()

        # Print out how long it took to start IO:
        if self._verbose:
            sys.stdout.write("Time to start {0} IO: {1:.2}s\n".format(mode, end - start))


        return


    def next(self, mode):
        self._dataloaders[mode].next(store_event_ids=True, store_entries=True)


    def fetch_minibatch_data(self, mode, fetch_meta_data=False):
        # Return a dictionary object with keys 'image', 'label', and others as needed
        # self._dataloaders['train'].fetch_data(keyword_label).dim() as an example
        
        
        while self._dataloaders[mode].is_reading():
            time.sleep(0.01)

        this_data = {}
        for key in self._data_keys[mode]:
            this_data[key] = self._dataloaders[mode].fetch_data(self._data_keys[mode][key]).data()
            this_data[key] = numpy.reshape(this_data[key], self._dims[mode][key])

        if fetch_meta_data:
            this_data['entries'] = self._dataloaders[mode].fetch_entries()
            this_data['event_ids'] = self._dataloaders[mode].fetch_event_ids()
        

        return this_data

    def fetch_minibatch_dims(self, mode):
        # Return a dictionary object with keys 'image', 'label', and others as needed
        # self._dataloaders['train'].fetch_data(keyword_label).dim() as an example
        return self._dims[mode]

    def stop(self):

        for mode in self._dataloaders:
            while self._dataloaders[mode].is_reading():
                time.sleep(0.01)
            self._dataloaders[mode].stop_manager()

        if self._writer is not None:
            self._writer.finalize()

    def size(self, mode):
        # return the number of images in the specified mode:
        return self._dataloaders[mode].fetch_n_entries()


    def write_output(self, data, datatype, producer, entries, event_ids):
        if self._writer is None:
            raise Exception("Trying to write data with no writer configured.  Abort!")


        self._writer.write(data=data, datatype=datatype, producer=producer, entries=entries, event_ids=event_ids)

        return
