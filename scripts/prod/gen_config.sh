#!/bin/bash

CFGTEMPPATH=$1
if test -z "$CFGTEMPPATH"; then
    echo "Config template pathname missing, exiting!"
    exit 1;
fi

FILENAME=$2
if test -z "$FILENAME"; then
    FILENAME=test.root
    echo "File name missing, setting to $FILENAME!"
fi

CFGFILE=$3
if test -z "$CFGFILE"; then
    CFGFILE=`echo $CFGTEMPPATH | cut -d_ -f1`_`basename ${FILENAME} .root`.cfg
    echo "Config name missing, setting to $CFGFILE"
fi

EVENTS=$4
if test -z "$EVENTS"; then
    EVENTS=1
    echo "Number of events missing, setting to $EVENTS"
fi

FEVNUM=$5
if test -z "$FEVNUM"; then
    FEVNUM=1
    echo "Number of first event missing, setting to $FEVNUM"
fi

FRUNNUM=$6
if test -z "$FRUNNUM"; then
    FRUNNUM=1
    echo "Number of run missing, setting to $FRUNNUM"
fi

cat $CFGTEMPPATH | sed "s/__FILENAME__/$FILENAME/"    | sed "s/__EVENTS__/$EVENTS/" \
             | sed "s/__FIRSTEVENTNUM__/$FEVNUM/" | sed "s/__FIRSTRUNNUM__/$FRUNNUM/"  \
             | sed "s/__SEED1__/$RANDOM/"         | sed "s/__SEED2__/$RANDOM/"  \
             | sed "s/__SEED3__/$RANDOM/" > $CFGFILE
