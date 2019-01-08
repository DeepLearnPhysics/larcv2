import sys
import os
import time

import numpy
from ROOT import larcv

class larcv_writer(object):

    '''A utility class for writing data as larcv objects to file.
    
    This class doesn't reinvent any wheels, it's just meant to package everything
    into an easy to use interface for writing output files.
    '''

    def __init__(self, io_config):
        '''larcv_writer accesses an IOManager instance directly to manage writing
        
        In the init function, the IOManager is created and prepared for writing.
        
        Arguments:
             {} -- [description]
        '''

        print(io_config)

        self._io = larcv.IOManager(io_config)
        self._io.initialize()

        self._write_workers = {
            'meta' : self._write_meta,
        }

        pass

    def finalize(self):
        # The purpose of this function is to make sure the output file is persisted to disk:
        self._io.finalize()

    def _seek(self, entry):
        '''Go to an entry
        
        Saves the current entry and seeks to a new entry.
        
        Arguments:
            entry {int} -- Desired Entry
        '''
        self._io.save_entry()
        self._io.read_entry(entry)

        return

    def _write_meta(self, data, producer):
        '''Write data of type 'meta'
        
        '''

        # Get the data object from the file:
        _writable_data = self._io.get_data("meta", producer)

        _writable_array = _writable_data.writeable_darray("meta")
        _writable_array.resize(len(data))

        for i in data:
            _writable_array.push_back(i)


        return



    def write(self, data, datatype, producer, entries, event_ids):
        '''Write data to file
        
        This function writes data to the output file.  It will check the entries and event ids
        to ensure there is not a mismatch.  You can call this function multiple times per event,
        if there are multiple things to write.  When you call it for the next event, and it has
        a new entry/event_id, it will save the previous event, seek to the next event, and continue.
        
        Arguments:
            data {numpy} -- Data to write, must meet certain formatting depending on datatype
            datatype {str} -- larcv2 datatype descriptor
            producer {srt} -- producer key underwhich to store this data
            entries {ROOT.vector<int>} -- vector of file entries, size 1 for now
            event_ids {ROOT.vector<larcv::EventBase>} -- vector of larcv EventBase objects, size 1 for now
        '''

        # First, check if the entries recieved for the data match the current entry
        # for the output file:

        if self._io.current_entry() != entries[0]:
            self._seek(entries[0])

        if datatype not in self._write_workers:
            raise Exception("No current support to write datatypes of type " + datatype)

        else:
            self._write_workers[datatype](data, producer)


        pass

