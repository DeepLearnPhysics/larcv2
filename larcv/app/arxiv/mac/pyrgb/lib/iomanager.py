from .. import larcv

# thin iomanager wrapper

class IOManager(object):

    def __init__(self,infiles=[],outfile=None,IOMode=0) :
        assert type(infiles) == list

        self.iom = larcv.IOManager(IOMode)

        for f in infiles :
            self.iom.add_in_file(f)
                
        if outfile is not None:
            self.iom.set_verbosity(0)
            self.iom.set_out_file(outfile)
            
        self.iom.initialize()

    def read_entry(self,entry):
        self.iom.read_entry(entry)

    def get_data(self,product,producer):
        return self.iom.get_data(product,producer)

    def set_verbosity(self,verb):
        assert type(verb) is int
        self.iom.set_verbosity(verb)

    def set_id(self,r,s,e):
        self.iom.set_id(r,s,e)

    def save_entry(self):
        self.iom.save_entry()

    def finalize(self):
        self.iom.finalize()
        
    def get_n_entries(self):
        return self.iom.get_n_entries()
