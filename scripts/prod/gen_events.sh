#!/bin/bash

function usage() {
    echo "Usage: $0 filename cfg_template_path events run_number"
    exit 1
}

FILENAME=$1
if test -z "$FILENAME"; then
    echo "Missing filename (argument 1), exiting!"
    usage
fi

CFGTEMPPATH=$2
if test -z "$CFGTEMPPATH"; then
    echo "Missing cfg_template_path (argument 2), exiting!"
    usage
fi
if test "$CFGTEMPPATH" = "`basename $CFGTEMPPATH`"; then
    CFGTEMPPATH=`pwd`/$CFGTEMPPATH
fi

EVENTS=$3
if test -z "$EVENTS"; then
    echo "Missing number of events (argument 3), exiting!"
    usage
fi

FRUNNUM=$4
if test -z "$FRUNNUM"; then
    echo "Missing run number (argument 4), exiting!"
    usage
fi

#this is for running in condor
if test -z "$HOME"; then
    export HOME=/net/hisrv0001/home/loizides
fi

if test -z "$DATADIR"; then
    DATADIR=$HOME/data/cms/
fi

if test -z "${_CONDOR_SCRATCH_DIR}"; then
    export _CONDOR_SCRATCH_DIR=/tmp/loizides
fi

FILEBASE=`basename $FILENAME .root`
CFGFILE=`basename $CFGTEMPPATH | cut -d_ -f1`_${FILEBASE}.cfg
TMPDIR=${_CONDOR_SCRATCH_DIR}/`basename $CFGTEMPPATH | cut -d_ -f1`_`date "+%s"`-$RANDOM

if test -z "$CMSSW_VERSION"; then
    source $HOME/setup_cms.sh 1_2_3 prod
fi

mkdir -p $TMPDIR
cd $TMPDIR

GENCONFIG=`dirname $CFGTEMPPATH`/gen_config.sh 
$GENCONFIG $CFGTEMPPATH $FILENAME $TMPDIR/$CFGFILE $EVENTS 1 $FRUNNUM

time cmsRun $TMPDIR/$CFGFILE < /dev/null > ${FILEBASE}.out 2> ${FILEBASE}.err

cp -a $FILENAME $DATADIR
bzip2 -9 < ${FILEBASE}.out > ${DATADIR}/${FILEBASE}.out.bz2
bzip2 -9 < ${FILEBASE}.err > ${DATADIR}/${FILEBASE}.err.bz2
bzip2 -9 < ${CFGFILE} > ${DATADIR}/${CFGFILE}.bz2

cd $HOME
rm -rf $TMPDIR
