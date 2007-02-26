#!/bin/bash
# $Id:$

function usage() {
    echo "Usage: $0 config_filename [output_filename run_number events cfg_template_path]"
    exit 1
}

CONFIGFILE=$1
if test -n "$CONFIGFILE" && test -e "$CONFIGFILE"; then
    source $1
else 
    echo "Missing config filename (argument 1), exiting!"
    usage
fi
if test "$CONFIGFILE" = "`basename $CONFIGFILE`"; then
    CONFIGFILE=`pwd`/$CONFIGFILE
fi

if test -z "$FILENAME"; then
    export FILENAME=$2
    if test -z "$FILENAME"; then
        echo "Missing filename (argument 2), exiting!"
        usage
    fi
fi

if test -z "$FRUNNUM"; then
    export FRUNNUM=$3
    if test -z "$FRUNNUM"; then
        echo "Missing run number (argument 3), exiting!"
        usage
    fi
fi

if test -z "$EVENTS"; then
    export EVENTS=$4
    if test -z "$EVENTS"; then
        echo "Missing number of events (argument 4), exiting!"
        usage
    fi
fi

if test -z "$CFGTEMPPATH"; then
    export CFGTEMPPATH=$5
    if test -z "$CFGTEMPPATH"; then
        echo "Missing cfg_template_path (argument 5), exiting!"
        usage
    fi
fi
if test "$CFGTEMPPATH" = "`basename $CFGTEMPPATH`"; then
    export CFGTEMPPATH=`pwd`/$CFGTEMPPATH
fi

#this is for running in condor
if test -z "$HOME"; then
    export HOME=/net/hisrv0001/home/loizides
fi

if test -z "$DATADIR"; then
    export DATADIR=$HOME/data/cms/
fi

if test -z "${_CONDOR_SCRATCH_DIR}"; then
    export _CONDOR_SCRATCH_DIR=/tmp/loizides
fi

if test -z "$GENCONFIG"; then
    GENCONFIG=`dirname $CFGTEMPPATH`/gen_config.sh 
fi

if test -z "$CMSSW_VERSION"; then
    KEEPPATH=`pwd`
    source $HOME/setup_cms.sh 1_2_3 prod
    cd $KEEPPATH
fi

FILEBASE=`basename $FILENAME .root`
CFGFILE=`basename $CFGTEMPPATH | cut -d_ -f1`_${FILEBASE}.cfg
TMPDIR=${_CONDOR_SCRATCH_DIR}/`basename $CFGTEMPPATH | cut -d_ -f1`_`date "+%s"`-$RANDOM

#---
echo "---Settings in gen_events.sh---"
echo CONFIGFILE=$CONFIGFILE
echo FILENAME=$FILENAME
echo CFGTEMPPATH=$CFGTEMPPATH
echo EVENTS=$EVENTS
echo FRUNNUM=$FRUNNUM
echo HOME=$HOME
echo DATADIR=$DATADIR
echo _CONDOR_SCRATCH_DIR=${_CONDOR_SCRATCH_DIR}
echo GENCONFIG=$GENCONFIG
echo CMSSW_VERSION=$CMSSW_VERSION
echo FILEBASE=$FILEBASE
echo CFGFILE=$CFGFILE
echo TMPDIR=$TMPDIR
echo "---gen_events.sh---"
#---

mkdir -p $TMPDIR
cd $TMPDIR

$GENCONFIG $CFGTEMPPATH $FILENAME $TMPDIR/$CFGFILE $EVENTS 1 $FRUNNUM

time cmsRun $TMPDIR/$CFGFILE < /dev/null > ${FILEBASE}.out 2> ${FILEBASE}.err

cp -a $FILENAME $DATADIR
bzip2 -9 < ${FILEBASE}.out > ${DATADIR}/${FILEBASE}.out.bz2
bzip2 -9 < ${FILEBASE}.err > ${DATADIR}/${FILEBASE}.err.bz2
bzip2 -9 < ${CFGFILE} > ${DATADIR}/${CFGFILE}.bz2

cd $HOME
rm -rf $TMPDIR
