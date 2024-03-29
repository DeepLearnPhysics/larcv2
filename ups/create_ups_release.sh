# This script creates a UPS-release-style folder from
# the binaries / include / lib of a larcv2 build.
# It will also automatically update the file `larcv2.table`.
#
# Usage
#
# create_ups_release.sh /path/to/local/products v2_X_x
#
# We require that a tag exists called v2_X_x.
# We check (but do not enforce) whether the HEAD of git
# is currently at that tag or not. If not, we print a
# warning.
#
# Note
# ----
# One way to check flavor and qualifiers:
#
# -bash-4.2$ which root
# /cvmfs/larsoft.opensciencegrid.org/products/root/v6_22_08d/Linux64bit+3.10-2.17-e20-p392-prof/bin/root

#
# Step 1 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# decide flavor/qualifiers from environment variables
# our only dependency is ROOT so that's what will determine it.
# we pick what is used to build larcv2.
PRODUCTS_PATH=$1
TAG=$2

# Extract flavor and qualifiers from environment variable ROOTSYS
FQ=$(basename $ROOTSYS)
IFS="-" # we are going to split the string using this delimiter
read -a arr <<< "$FQ" # store in arr(ay) the pieces after the split
if [[ ${#arr[@]} -lt 5 ]]; then
  echo "Could not determine flavor or qualifier from environment variable ROOTSYS: ${ROOTSYS}"
  exit 1
fi
FLAVOR="${arr[0]}-${arr[1]}" # for example Linux64bit+3.10-2.17
QUAL_E="${arr[2]}" # for example e20
QUAL_P="${arr[3]}" # for example p392 (Python version)
QUAL_Q="${arr[4]}" # for example prof / debug etc
QUALIFIERS="${QUAL_E}_${QUAL_P}_${QUAL_Q}"
echo "Found qualifiers: ${QUALIFIERS}"

# We could also get flavor directly from ups...?
#FLAVOR=$(ups flavor -4)

FLAVOR_FULL="${FLAVOR}_${QUALIFIERS}"
echo "Found flavor: ${FLAVOR}"

# Step 2 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Make sure we have the right larcv2 version built
# by comparing git commit hash
if [[ -z "$LARCV_BASEDIR" ]]; then # does the environment variable LARCV_BASEDIR exist?
  echo "You need to run larcv2/configure.sh first!"
  exit 1
fi
cd $LARCV_BASEDIR
CHECK_TAG=$(git tag -l $TAG | sed 's/ //g')
if [ $CHECK_TAG != $TAG ]; then # does the tag v2_X_x exist?
  echo "The tag ${TAG} does not exist yet, did you make a typo?"
  exit 1
fi
CURRENT_COMMIT=$(git rev-parse HEAD)
TAG_COMMIT=$(git rev-parse $TAG)
if [ $CURRENT_COMMIT != $TAG_COMMIT ]; then
  echo "WARNING - current HEAD is not at tag ${TAG}. Are you sure you built the right version?"
fi
# I don't think it makes sense for this script to run the build
# itself...

# However we can update the larcv2.table automatically
if [[ -f $LARCV_BASEDIR/ups/larcv2.table ]]; then # does the file larcv2.table exist?
  echo "File ups/larcv2.table exists already."
  # If yes, we just need to update the relevant entry.
  NEW_GROUP="
Flavor     = ANY
Qualifiers = \"${QUAL_E}:${QUAL_P}:${QUAL_Q}\"
  Action = DefineFQ
      envSet(LARCV2_FQ_DIR, \${UPS_PROD_DIR}/\${UPS_PROD_FLAVOR}_${QUAL_E}_${QUAL_P}_${QUAL_Q})
  Action = GetProducts
      setupRequired( root $ROOT_VERSION -q +${QUAL_E}:+${QUAL_P}:+${QUAL_Q} )
  "
  NEW_GROUP=${NEW_GROUP//$'\n'/\\$'\n'}
  NEW_GROUP=${NEW_GROUP//$'/'/\\$'/'}
  NEW_GROUP=${NEW_GROUP//$'+'/\\$'+'}
  # first find if there is already an entry for current flavor and delete it
  PATTERN="Qualifiers = \"${QUAL_E}:${QUAL_P}:${QUAL_Q}\""

  if grep -q $PATTERN $LARCV_BASEDIR/ups/larcv2.table; then
    echo "Replacing existing flavor group."
    # Breakdown for my own sanity.
    # /$FLAVOR_PATTERN/    first find $FLAVOR_PATTERN
    #   N                  append next line
    #   /$PATTERN/         find $PATTERN
    #   :a                 start loop (label a)
    #   N                  and accumulate lines
    #   /^$/M !ba          until the pattern /^$/ (new line) is found, in a multiline setting (option M in GNU sed)
    #                      !ba means for each line that does NOT fit the pattern /^$/ we go back (b for branch) to label a.
    #   d                  delete pattern space (i.e. whole paragraph we just found)
    # After this, we find the start of the list of flavors "Group:"
    # and write down $NEW_GROUP.
    FLAVOR_PATTERN="Flavor     = ANY"
    sed -e '/'"${FLAVOR_PATTERN}"'/{ N; /'"${PATTERN}"'/{:a; N; /^$/M !ba; d; }; }' -e '/Group:/a \\n'"${NEW_GROUP}"'' $LARCV_BASEDIR/ups/larcv2.table > $LARCV_BASEDIR/ups/larcv2.temp
    # Breakdown for my own sanity.
    mv $LARCV_BASEDIR/ups/larcv2.temp $LARCV_BASEDIR/ups/larcv2.table
  else
    echo "Adding new flavor group."
    # Find "Group:" line and insert the new group right after this
    sed "s/Group:/&\n${NEW_GROUP}/" $LARCV_BASEDIR/ups/larcv2.table > $LARCV_BASEDIR/ups/larcv2.temp
    mv $LARCV_BASEDIR/ups/larcv2.temp $LARCV_BASEDIR/ups/larcv2.table
  fi
else
  echo "Creating file ups/larcv2.table"
  cat > $LARCV_BASEDIR/ups/larcv2.table <<- EOM
File=Table
Product=larcv2

Group:

Flavor     = ANY
Qualifiers = "${QUAL_E}:${QUAL_P}:${QUAL_Q}"
  Action = DefineFQ
      envSet(LARCV2_FQ_DIR, \${UPS_PROD_DIR}/\${UPS_PROD_FLAVOR}_${QUAL_E}_${QUAL_P}_${QUAL_Q})

  Action = GetProducts
      setupRequired( root $ROOT_VERSION -q +${QUAL_E}:+${QUAL_P}:+${QUAL_Q} )


Common:
  Action = setup
    prodDir()
    setupEnv()
    envSet(\${UPS_PROD_NAME_UC}_VERSION, \${UPS_PROD_VERSION})

    # Set up required products, which is root
    exeActionRequired(GetProducts)
    exeActionRequired(DefineFQ)

    # FIXME which ones do we really need?
    envSet(\${UPS_PROD_NAME_UC}_BASEDIR,\${LARCV2_FQ_DIR})
    #envSet(\${UPS_PROD_NAME_UC}_COREDIR,\${UPS_PROD_DIR}/\${UPS_PROD_FLAVOR}/core)
    #envSet(\${UPS_PROD_NAME_UC}_APPDIR,\${UPS_PROD_DIR}/\${UPS_PROD_FLAVOR}/app)
    envSet(\${UPS_PROD_NAME_UC}_ROOT6,0)
    envSet(\${UPS_PROD_NAME_UC}_INCDIR,\${LARCV2_FQ_DIR}/include)
    envSet(\${UPS_PROD_NAME_UC}_LIBDIR,\${LARCV2_FQ_DIR}/lib)

    # most critical
    envSet(\${UPS_PROD_NAME_UC}_INC,\${LARCV2_FQ_DIR}/include)
    envSet(\${UPS_PROD_NAME_UC}_LIB,\${LARCV2_FQ_DIR}/lib)

    if ( test `uname` = "Darwin" )
      envSet(\${UPS_PROD_NAME_UC}_CXX,clang++)
      pathPrepend(DYLD_LIBRARY_PATH, \${LARCV2_LIBDIR})
    else()
      envSet(\${UPS_PROD_NAME_UC}_CXX,g++)
      pathPrepend(LD_LIBRARY_PATH, \${LARCV2_LIBDIR})
    endif ( test `uname` = "Darwin" )

    # add the bin directory to the path
    pathPrepend(PATH, \${LARCV2_FQ_DIR}/bin )
    # add the python area to the pythonpath
    pathPrepend(PYTHONPATH, \${LARCV2_FQ_DIR}/python )

End:
# End Group definition
#*************************************************
#
# ups declare command that works on gpvm:
# ups declare larcv2 $TAG -r larcv2/$TAG -f $FLAVOR -m larcv2.table -q $QUAL_E:$QUAL_P:$QUAL_Q -U ups/
#
#
EOM
fi
echo "Updated ups/larcv2.table."

# Step 4 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# format product folder correctly and copy bin / include / lib
cd $PRODUCTS_PATH
if [[ ! -d larcv2 ]]; then
  mkdir larcv2
fi
cd larcv2

# Create folders v2_X_x and v2_X_x.version
if [[ ! -d $TAG ]]; then
  mkdir $TAG
fi
if [[ ! -d "$TAG".version ]]; then
  mkdir "$TAG".version
fi

# Format $TAG folder
scp -r "$LARCV_BASEDIR"/ups "$TAG"/
if [[ ! -d "$TAG"/"$FLAVOR_FULL" ]]; then
  mkdir "$TAG"/"$FLAVOR_FULL"
fi
scp -r $LARCV_BUILDDIR/bin "$TAG"/"$FLAVOR_FULL"/
scp -r $LARCV_BUILDDIR/include "$TAG"/"$FLAVOR_FULL"/
scp -r $LARCV_BUILDDIR/lib "$TAG"/"$FLAVOR_FULL"/

# Format $TAG.version folder
FLAVOR_FILE="$TAG".version/"$FLAVOR_FULL"
DATE=$(date -u +'%Y-%m-%d %H.%M.%S GMT')
cat > $FLAVOR_FILE <<- EOM
FILE = version
PRODUCT = larcv2
VERSION = ${TAG}

#*************************************************
#
FLAVOR = ${FLAVOR}
QUALIFIERS = "${QUAL_E}:${QUAL_P}:${QUAL_Q}"
  DECLARER = jenkins
  DECLARED = ${DATE}
  MODIFIER = jenkins
  MODIFIED = ${DATE}
  PROD_DIR = larcv2/${TAG}
  UPS_DIR = ups
  TABLE_DIR = ups
  TABLE_FILE = larcv2.table
EOM
