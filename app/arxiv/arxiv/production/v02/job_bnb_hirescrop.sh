#!/bin/bash

source /usr/nevis/adm/nevis-init.sh
module load root/06.04.00

process=$1
sw_dir=$2
input_dir=$3
output_dir=$4
config=$5
sleep $process
source $sw_dir/larlite/config/setup.sh
source $sw_dir/larcv/configure.sh

scp -r $sw_dir/larcv/production/v02 ./tmp
wait
cd tmp

scp $output_dir/procs.txt ./procs.txt

python job_bnb_hirescrop.py $process $input_dir $output_dir $config

cd ..
rm -r tmp


