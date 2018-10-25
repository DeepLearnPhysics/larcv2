#import ROOT,sys,time,os,signal
from larcv import larcv
import ROOT as rt
import sys,time,os,signal
import numpy as np
import threading

def threadio_func(storage, proc):
   storage._threaded = True
   while storage._threaded:
      time.sleep(0.000005)
      if storage._filled: continue
      storage._read_start_time = time.time()
      while 1:
         if proc.storage_status_array()[storage._storage_id] == 3:
            storage._read_end_time=time.time()
            break
         time.sleep(0.000005)
         continue
      storage.next()
      storage._event_ids     = proc.processed_entries(storage._storage_id)
      storage._ttree_entries = proc.processed_entries(storage._storage_id)
   return

class threadio_storage(object):

   def __init__(self,storage_id):
      self._storage_id = int(storage_id)
      self._storage_m  = {}
      self._filled     = False
      self._threaded   = False
      self._empty      = False
      self._read_start_time = self._read_end_time = -1
      self._copy_start_time = self._copy_end_time = -1
      self._event_ids       = None
      self._ttree_entries   = None
      assert self._storage_id >= 0

   def register(self, key, dtype, make_copy=False):
      assert (not key in self._storage_m.keys())
      self._storage_m[key] = threadio_pydata(key,dtype)
      self._storage_m[key]._storage_id = self._storage_id
      self._storage_m[key]._make_copy  = make_copy

   def fetch_data(self,key):
      try:
         return self._storage_m[key]
      except KeyError:
         sys.stderr.write('Cannot fetch data w/ key %s (unknown)\n' % key)
         return

   def next(self):
      self._filled=False
      self._copy_start_time = time.time()
      for name, storage in self._storage_m.items():
         dtype = storage.dtype()
         batch_data = larcv.BatchDataStorageFactory(dtype).get().get_storage(name).get_batch(self._storage_id)
         storage.set_data(self._storage_id, batch_data)
      self._copy_end_time = time.time()
      self._filled=True

   def release(self):
      self._filled = False
      return 

class threadio_pydata(object):
   _name       = None
   _storage_id = -1
   _dtype      = None
   _npy_data   = None
   _dim_data   = None
   _time_copy  = 0
   _time_reshape = 0
   _make_copy  = False

   def __init__(self,name,dtype):
      self._name = str(name)
      self._storage_id = -1
      self._dtype = dtype
      self._npy_data = None
      self._dim_data = None
      self._time_copy = None
      self._time_reshape = None
      self._make_copy = False
      
   def batch_data_size(self):
      dsize=1
      for v in self._dim_data: dsize *= v
      return dsize

   def dtype(self): return self._dtype
   def data(self): return self._npy_data
   def dim(self):  return self._dim_data
   def time_copy(self): return self._time_copy
   def time_reshape(self): return self._time_reshape

   def set_data(self,storage_id,larcv_batchdata):
      self._storage_id = storage_id
      dim = larcv_batchdata.dim()

      # set dimension
      if self._dim_data is None:
         self._dim_data = np.array([dim[i] for i in range(dim.size())]).astype(np.int32)

      else:
         if not len(self._dim_data) == dim.size():
            sys.stderr.write('Dimension array length changed (%d => %d)\n' % (len(self._dim_data),dim.size()))
            raise TypeError
         for i in range(len(self._dim_data)):
            if not self._dim_data[i] == dim[i]:
               sys.stderr.write('%d-th dimension changed (%d => %d)\n' % (i,self._dim_data[i],dim[i]))
               raise ValueError

      # copy data into numpy array
      ctime = time.time()
      if self._make_copy:
         if self._npy_data is None:
            self._npy_data = np.array(larcv_batchdata.data())
         else:
            self._npy_data = self._npy_data.reshape(self.batch_data_size())
            larcv.copy_array(self._npy_data,larcv_batchdata.data())
      else:
         self._npy_data = larcv.as_ndarray(larcv_batchdata.data())
      self._time_copy = time.time() - ctime

      ctime = time.time()
      self._npy_data = self._npy_data.reshape(self._dim_data[0], int(self.batch_data_size()/self._dim_data[0]))
      self.time_data_conv = time.time() - ctime
      
      return

class larcv_threadio (object):

   _instance_m={}

   @classmethod
   def exist(cls,name):
      name = str(name)
      return name in cls._instance_m

   @classmethod
   def instance_by_name(cls,name):
      return cls._instance_m[name]

   def __init__(self):
      self._proc = None
      self._name = ''
      self._verbose = False
      self._cfg_file = None
      self._target_storage_id = 0
      self._storage_v = []
      self._thread_v = []

   def reset(self):
      self.stop_manager()
      if self._proc: self._proc.reset()

   def __del__(self):
      try:
         self.reset()
      except AttrbuteError:
         pass

   def configure(self,cfg):
      # if "this" was configured before, reset it
      if self._name: self.reset()
         
      # get name
      if not cfg['filler_name']:
         sys.stderr.write('filler_name is empty!\n')
         raise ValueError

      # ensure unique name
      if self.__class__.exist(cfg['filler_name']) and not self.__class__.instance_by_name(cfg['filler_name']) == self:
         sys.stderr.write('filler_name %s already running!' % cfg['filler_name'])
         return
      self._name = cfg['filler_name']         

      # get ThreadProcessor config file
      self._cfg_file = cfg['filler_cfg']
      if not self._cfg_file or not os.path.isfile(self._cfg_file):
         sys.stderr.write('filler_cfg file does not exist: %s\n' % self._cfg_file)
         raise ValueError

      # set verbosity
      if 'verbosity' in cfg:
         self._verbose = bool(cfg['verbosity'])

      # configure thread processor
      self._proc = larcv.ThreadProcessor(self._name)

      self._proc.configure(self._cfg_file)
      self._storage_v = []
      for storage_id in range(self._proc.storage_status_array().size()):
         self._storage_v.append(threadio_storage(storage_id))
      # fetch batch filler info
      make_copy = bool('make_copy' in cfg and cfg['make_copy'])
      for i in range(self._proc.batch_fillers().size()):
         pid = self._proc.batch_fillers()[i]
         name = self._proc.storage_name(pid)
         dtype = larcv.BatchDataTypeName(self._proc.batch_types()[i])
         for storage_id in range(self._proc.storage_status_array().size()):
            self._storage_v[storage_id].register(name,dtype,make_copy=make_copy)

      # all success?
      # register *this* instance
      self.__class__._instance_m[self._name] = self

   def start_manager(self, batch_size):
      if not self._proc or not self._proc.configured():
         sys.stderr.write('must call configure(cfg) before start_manager()!\n')
         return
      try:
         batch_size=int(batch_size)
         if batch_size<1:
            sys.stderr.write('batch_size must be positive integer!\n')
            raise ValueError
      except TypeError:
         sys.stderr.write('batch_size value/type error. aborting...\n')
         return
      except ValueError:
         sys.stderr.write('batch_size value/type error. aborting...\n')
         return

      self._batch=batch_size
      self._proc.start_manager(batch_size)
      for storage in self._storage_v:
         self._thread_v.append(threading.Thread(target = threadio_func, args=[storage,self._proc]))
         self._thread_v[-1].daemon = True
         self._thread_v[-1].start()
      self._target_storage_id=0

   def stop_manager(self):
      if not self._proc or not self._proc.configured():
         sys.stderr.write('must call configure(cfg) before start_manager()!\n')
         return
      self._batch=None
      self._proc.stop_manager()
      for storage in self._storage_v:
         storage._threaded = False
      time.sleep(0.1)
      self._thread_v = []

   def purge_storage(self):
      if not self._proc or not self._proc.configured():
         sys.stderr.write('must call configure(cfg) before start_manager()!\n')
         return
      self.stop_manager()
      self._proc.release_data()
      self._target_storage_id=0

   def set_next_index(self,index):
      if not self._proc or not self._proc.configured():
         sys.stderr.write('must call configure(cfg) before start_manager()!\n')
         return
      self._proc.set_next_index(index)

   def is_reading(self,storage_id=None):
      if storage_id is None:
         storage_id = self._target_storage_id
      return not self._storage_v[storage_id]._filled

   def next(self):
      while self.is_reading():
         time.sleep(0.000002)
      self._proc.release_data(self._target_storage_id)
      self._storage_v[self._target_storage_id].release()
      self._target_storage_id += 1
      if self._target_storage_id == self._proc.num_batch_storage():
         self._target_storage_id = 0
      return 

   def fetch_data(self,key,storage_id=None):
      if storage_id is None:
         storage_id = self._target_storage_id
      try:
         return self._storage_v[storage_id].fetch_data(key)
      except IndexError:
         sys.stderr.write('Cannot fetch data w/ storage id {:d} (unknown)\n'.format(storage_id))
         return

   def fetch_event_ids(self,storage_id=None):
      if storage_id is None:
         storage_id = self._target_storage_id
      return self._storage_v[storage_id]._event_ids

   def fetch_entries(self,storage_id=None):
      if storage_id is None:
         storage_id = self._target_storage_id
      return self._storage_v[storage_id]._ttree_entries

   def fetch_n_entries(self):
      return self._proc.get_n_entries()

def sig_kill(signal,frame):
   print('\033[95mSIGINT detected.\033[00m Finishing the program gracefully.')
   for name,ptr in larcv_threadio._instance_m.items():
      print('Terminating filler: %s' % name)
      ptr.reset()

signal.signal(signal.SIGINT,  sig_kill)



