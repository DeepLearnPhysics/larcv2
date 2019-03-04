#!/bin/bash

source /usr/nevis/adm/nevis-init.sh
module load root/06.04.00

process=$1
sw_dir=$2
output_dir=$3
config=$4
sleep $process
source $sw_dir/larlite/config/setup.sh
source $sw_dir/larcv/configure.sh

scp -r $sw_dir/larcv/production/v02 ./tmp
wait
cd tmp

scp $output_dir/flist_nu_$process.txt ./flist_nu.txt
scp $output_dir/flist_cosmic_$process.txt ./flist_cosmic.txt

python job_merger.py $output_dir $config

cd ..
rm -r tmp


