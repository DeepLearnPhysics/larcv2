#!/usr/bin/python3
from larcv import larcv
from ROOT import TChain
import numpy as np
import argparse, gdown, os, sys

def main():
    parser = argparse.ArgumentParser(
            description="Check two larcv files have the same contents")

    parser.add_argument('--ref_file', '-r',
                        help='Path to the reference file',
                        type=str, required=True)
    parser.add_argument('--test_file', '-t',
                        help='Path to the target file to be tested',
                        type=str, required=True)
    parser.add_argument('--key_tensor3d',
                        help='Name key string to retrieve a Tensor3D object',
                        type=str, required=False, default="pcluster")
    parser.add_argument('--key_cluster3d',
                        help='Name key string to retrieve a Cluster3D object',
                        type=str, required=False, default="pcluster")
    parser.add_argument('--key_particle',
                        help='Name key string to retrieve a Particle object',
                        type=str, required=False, default="pcluster")

    args = parser.parse_args()

    ref_file=args.ref_file
    test_file=args.test_file
    
    if not os.path.isfile(ref_file):
        FileNotFoundError(f'Reference file not found: {ref_file}')

    if not os.path.isfile(test_file):
        FileNotFoundError(f'Test target file not found: {test_file}')

    error_code=0
    try:
        test_tensor3d  (ref_file, test_file, args.key_tensor3d  )
    except AssertionError:
        print('Found incompatible tensor3d')
        error_code+=1

    try:
        test_cluster3d (ref_file, test_file, args.key_cluster3d )
    except AssertionError:
        print('Found incompatible cluster3d')
        error_code+=2

    try:
        test_particles (ref_file, test_file, args.key_particle  )
    except AssertionError:
        print('Found incompatible particle')
        error_code+=4

    sys.exit(error_code)
        
def compare_voxel_sets(vs0,vs1,meta=None):

    assert vs0.size() == vs1.size()
    
    pts0=np.zeros(shape=(vs0.size(),4),dtype=np.float32)
    pts1=np.zeros(shape=(vs1.size(),4),dtype=np.float32)

    if meta is None:
        larcv.fill_3d_pcloud(vs0,pts0)
        larcv.fill_3d_pcloud(vs1,pts1)
    else:
        larcv.fill_3d_pcloud(vs0,meta,pts0)
        larcv.fill_3d_pcloud(vs1,meta,pts1)
    
    assert pts0.sum()==pts1.sum()
    assert (pts1-pts0).sum() == np.float32(0.)
    assert (pts0 == pts1).sum() == np.prod(pts0.shape)
    

def test_tensor3d(ref_file,target_file,name_tensor3d):

    tree_name = 'sparse3d_'+name_tensor3d+'_tree'
    branch_name = 'sparse3d_'+name_tensor3d+'_branch'
    ch0=TChain(tree_name)
    ch1=TChain(tree_name)

    ch0.AddFile(ref_file)
    ch1.AddFile(target_file)

    assert ch0.GetEntries() == ch1.GetEntries()

    for entry in range(ch0.GetEntries()):

        ch0.GetEntry(entry)
        ch1.GetEntry(entry)

        br0=getattr(ch0,branch_name)
        br1=getattr(ch1,branch_name)

        compare_voxel_sets(br0,br1)

def test_cluster3d(ref_file,target_file,name_cluster3d):
    
    tree_name = 'cluster3d_'+name_cluster3d+'_tree'
    branch_name = 'cluster3d_'+name_cluster3d+'_branch'
    ch0=TChain(tree_name)
    ch1=TChain(tree_name)

    ch0.AddFile(ref_file)
    ch1.AddFile(target_file)

    assert ch0.GetEntries() == ch1.GetEntries()

    for entry in range(ch0.GetEntries()):

        ch0.GetEntry(entry)
        ch1.GetEntry(entry)

        br0=getattr(ch0,branch_name)
        br1=getattr(ch1,branch_name)

        assert br0.size() == br1.size()
        assert br0.meta() == br1.meta()
        
        for i in range(br0.size()):

            vs0=br0.as_vector()[i]
            vs1=br1.as_vector()[i]

            compare_voxel_sets(vs0,vs1,br0.meta())

def test_particles(ref_file,target_file,name_particle):

    tree_name = 'particle_'+name_particle+'_tree'
    branch_name = 'particle_'+name_particle+'_branch'
    ch0=TChain(tree_name)
    ch1=TChain(tree_name)

    ch0.AddFile(ref_file)
    ch1.AddFile(target_file)

    assert ch0.GetEntries() == ch1.GetEntries()

    for entry in range(ch0.GetEntries()):

        ch0.GetEntry(entry)
        ch1.GetEntry(entry)

        br0=getattr(ch0,branch_name)
        br1=getattr(ch1,branch_name)

        assert br0.size() == br1.size()

        for i in range(br0.size()):

            p0 = br0.as_vector()[i]
            p1 = br1.as_vector()[i]

            assert p0.position() == p1.position()
            assert p0.end_position() == p1.end_position()
            assert p0.first_step() == p1.first_step()
            assert p0.last_step() == p1.last_step()
            assert p0.pdg_code() == p1.pdg_code()
            assert p0.track_id() == p1.track_id()
            assert p0.parent_track_id() == p1.parent_track_id()
            assert p0.id() == p1.id()
            assert p0.parent_id() == p1.parent_id()
            assert p0.ancestor_track_id() == p1.ancestor_track_id()
            assert p0.shape() == p1.shape()

if __name__ == '__main__':

    main()
