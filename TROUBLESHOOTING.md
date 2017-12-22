## Set-up

In order to `import larcv`, you need to have the following environment
variables defined:

```
export LARCV_BASEDIR="/your/path/to/DeepLearnPhysics/larcv2"
export LARCV_COREDIR=$LARCV_BASEDIR/larcv/core
export LARCV_APPDIR=$LARCV_BASEDIR/larcv/app

export LARCV_BUILDDIR=$LARCV_BASEDIR/build
export LARCV_LIBDIR=$LARCV_BUILDDIR/lib
export LARCV_INCDIR=$LARCV_BUILDDIR/include
export LARCV_BINDIR=$LARCV_BUILDDIR/bin

export PYTHONPATH=${LARCV_BASEDIR}/python:$PYTHONPATH
```

On Mac-OSX, you will need a line like:

```
export DYLD_LIBRARY_PATH=$LARCV_LIBDIR:$DYLD_LIBRARY_PATH
```

While on Linux you will need a line like:

```
export LD_LIBRARY_PATH=$LARCV_LIBDIR:$LD_LIBRARY_PATH
```

It is best to put these definitions into a script to configure your
environment.
