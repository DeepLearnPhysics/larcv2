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

scp $output_dir/flist_$process.txt ./flist.txt

python job_hirescrop.py $output_dir $config

cd ..
rm -r tmp


