#!/usr/bin/env python
import sys,os,argparse

# This script is a simplification of run_processordb but without the proddb dependence.
# It incorporates command line arguments but runs stand-alone with just larcv.

parser = argparse.ArgumentParser(description='LArCV ProcessDriver execution script (use proddb)')

parser.add_argument('-c','--config',
                    type=str, dest='cfg',
                    help='string, Config file',required=True)

parser.add_argument('-il','--input-larcv',required=True,
                    dest='larcv_fin',nargs='+',
                    help='string or list, Input larcv file name[s] (Required)')

parser.add_argument('-nevents','--num-events',
                    type=int, dest='nevents', default=0,
                    help='integer, Number of events to process')

parser.add_argument('-nskip','--num-skip',
                    type=int, dest='nskip', default=0,
                    help='integer, Number of events to skip')

parser.add_argument('-ol','--output-larcv',default='',
                    type=str, dest='larcv_fout',
                    help='string,  Output larcv file name (optional)')

parser.add_argument('-oa','--output-ana',default='',
                    type=str, dest='ana_fout',
                    help='string,  Output analysis file name (optional)')

args = parser.parse_args()


if len(args.larcv_fin) == 0:
    print('No input files found')
    sys.exit(1)

from ROOT import std
from larcv import larcv

proc = larcv.ProcessDriver('ProcessDriver')

proc.configure(args.cfg)

if args.larcv_fout != '':
    proc.override_output_file(args.larcv_fout)

proc.override_ana_file(args.ana_fout)

flist = std.vector('string')()
for f in args.larcv_fin:
    flist.push_back(f)

proc.override_input_file(flist)

proc.initialize()

proc.batch_process(args.nskip,args.nevents)

print("Number of entries processed: " + str(proc.io().get_n_entries_out()))
print("Output file name: " + str(proc.io().get_file_out_name()))


proc.finalize()
