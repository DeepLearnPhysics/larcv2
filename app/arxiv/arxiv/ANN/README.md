# Interace to ANN data structures

We use the external package ANN to provide us a quick, scalable kd-tree (or bd-tree) implementation. This allows one to do nearest-neighbor queries for a set of points quickly.

To use this, one needs to install and build ANN: https://www.cs.umd.edu/~mount/ANN/.

Note that the very readable ANN manual is here: https://www.cs.umd.edu/~mount/ANN/Files/1.1.2/ANNmanual_1.1.pdf

Installation is fairly easy. Untar the package, go into the folder and type
 
    make linux-g++

To tell LArCV to build this module, and the DBSCAN clustering module that depends on this, add the following environment variables before running configure:

    ANN_LIBDIR=[path to ANN top-level folder]/lib
    ANN_INCDIR=[path to ANN top-level folder/include

