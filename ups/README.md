# Making a UPS product from larcv2

To differentiate from [larcv1](https://github.com/LArbys/LArCV), this
UPS product is called **larcv2** (not just `larcv`).

## What is `larcv2.table` ?
The file `larcv2.table` shouldn't be edited by hand.
It is automatically updated by the script `create_ups_release.sh`.
It defines what happens when UPS sets up `larcv2` in the environment,
and what other UPS products it depends on (in our case, just ROOT).

## Updating larcv2 version available in UPS
Here are the steps to update the larcv2 version
available as a [UPS](https://github.com/brettviren/fnal-ups) product.

1. Make sure you built larcv2 properly.
```
$ cd $LARCV_BASEDIR
$ source configure.sh
$ make -j8
```

Note that you might need to make 2 separate builds: one with `e20:prof`
qualifier of LArsoft, one with `c7:prof`.

*Note : to build for `c7` (clang) instead of `e20` (gcc), do instead*
```
# Assuming you setup a c7:prof qualifier for LArsoft and environment
$ cd $LARCV_BASEDIR
$ source configure.sh
$ export LARCV_CXX=$(which clang++)
$ make -j8
```

2. Create a tag to keep track of what you used to make the UPS product.
```
$ git tag v2_X_x
$ git push origin v2_X_x
```
The script `create_ups_release.sh` will check that this tag exists.
If the tag exists but does not coincide with the current HEAD, a warning
will be printed but you will be allowed to move forward.

3. Create a local products folder somewhere (outside of larcv2 folder).
For the sake of demonstration I will call mine `/your/products/folder`.
```
$ mkdir /your/products/folder
```
We use this to build the UPS folder and test it first before we copy it
to the CVMFS file system, which is supposed to be somewhat definitive
(if you want to make changes, push a new "version" there).

4. Call the script that automates building the UPS folder:
```
$ sh $LARCV_BASEDIR/ups/create_ups_release.sh /your/products/folder v2_X_x
```
This should create folders and files under `/your/products/folder/larcv2/`.
It should also update the file `larcv2.table` using current values of
`ROOTSYS` and `ROOT_VERSION` (environment variables).

**If you are building for several qualifiers (say `c7:prof` and `e20:prof`),
you should run this command after each build (and in each environment).** It
will automatically update `larcv2.table` for the appropriate (current) qualifier.

5. You can test the product this way:
```
$ export PRODUCTS=$PRODUCTS:/your/products/folder
```
Make sure you have a file /your/products/folder/.upsfiles/dbconfig, to have it recognized as
a products repository. If it doesn't exist yet, you can create it and put this in it:
```
FILE = DBCONFIG
AUTHORIZED_NODES = *
VERSION_SUBDIR = 1
PROD_DIR_PREFIX = ${UPS_THIS_DB}}
```

Then test the ups setup:
```
$ ups list -aK+ larcv2
"larcv2" "v2_1_0" "Linux64bit+3.10-2.17" "e20:p392:prof" ""
"larcv2" "v2_1_0" "Linux64bit+3.10-2.17" "c7:p392:prof" ""
$ ups setup larcv2 v2_X_x
```
In this example, I built `larcv2` against both qualifiers and they show up on 2 different lines.
You can check that you can build software that depends on it, etc.

6. Now you are ready to copy these files over to the CVMFS file system.
You need to have proper kerberos permission for it. /wherever/your/products/folder is likely on a
different server.

```
$ ssh cvmfssbn@oasiscfs.fnal.gov
$ sbn_cvmfs_check
No transaction open
$ sbn_cvmfs_transaction
$ kinit your_username@FNAL.GOV
$ scp -r /wherever/your/products/folder/larcv2/v2_X_x* /cvmfs/sbn.opensciencegrid.org/products/sbn/larcv2/
$ sbn_cvmfs_publish
```

**Note** If `sbn_cvmfs_check` replies that a transaction exists, you must try `sbn_cvmfs_abort` to
kill it. If that does not work (it might say `Repository sbn.opensciencegrid.org is not in a transaction`),
then you have to remove the lock file manually:
```
$ rm .sbn_server_transaction
```
Then you can go back to creating the transaction.


