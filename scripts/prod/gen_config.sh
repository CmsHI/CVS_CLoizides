#!/bin/bash
# $Id: gen_config.sh,v 1.2 2007/02/26 21:11:54 loizides Exp $

if test -z "$CFGTEMPPATH"; then
    CFGTEMPPATH=$1
    if test -z "$CFGTEMPPATH"; then
        echo "$0: Config template pathname missing, exiting!"
        exit 1;
    fi
fi

if test -z "$FILENAME"; then
    FILENAME=$2
    if test -z "$FILENAME"; then
        FILENAME=test.root
        echo "$0: File name missing, setting to $FILENAME!"
    fi
fi

if test -z "$CFGFILE"; then
    CFGFILE=$3
    if test -z "$CFGFILE"; then
        CFGFILE=`echo $CFGTEMPPATH | cut -d_ -f1`_`basename ${FILENAME} .root`.cfg
        echo "$0: Config name missing, setting to $CFGFILE"
    fi
fi

if test -z "$EVENTS"; then
    EVENTS=$4
    if test -z "$EVENTS"; then
        EVENTS=1
        echo "$0: Number of events missing, setting to $EVENTS"
    fi
fi

if test -z "$FEVNUM"; then
    FEVNUM=$5
    if test -z "$FEVNUM"; then
        FEVNUM=1
        echo "$0: Number of first event missing, setting to $FEVNUM"
    fi
fi

if test -z "$FRUNNUM"; then
    FRUNNUM=$6
    if test -z "$FRUNNUM"; then
        FRUNNUM=1
        echo "$0: Number of run missing, setting to $FRUNNUM"
    fi
fi

if test -z "$MAXEVENTS"; then
    MAXEVENTS=$7
    if test -z "$MAXEVENTS"; then
        MAXEVENTS=$EVENTS
        echo "$0: Number of max events missing, setting to $MAXEVENTS"
    fi
fi

if test -z "$MAXEVENTS"; then
    MAXEVENTS=$7
    if test -z "$MAXEVENTS"; then
        MAXEVENTS=$EVENTS
        echo "$0: Number of max events missing, setting to $MAXEVENTS"
    fi
fi

if test -z "$PTMIN"; then
    PTMIN=0
fi
if test -z "$PTMAX"; then
    PTMAX=-1
fi
if test -z "$ETAMIN"; then
    ETAMIN=-5
fi
if test -z "$ETAMAX"; then
    ETAMAX=+5
fi
if test -z "$PHIMIN"; then
    PHIMIN=0
fi
if test -z "$PHIMAX"; then
    PHIMAX=360
fi

cat $CFGTEMPPATH | sed "s/__FILENAME__/$FILENAME/"    | sed "s/__EVENTS__/$EVENTS/"        \
                 | sed "s/__FIRSTEVENTNUM__/$FEVNUM/" | sed "s/__FIRSTRUNNUM__/$FRUNNUM/"  \
                 | sed "s/__MAXEVENTS__/$MAXEVENTS/"  | sed "s/__PARTID__/$PARTID/"        \
                 | sed "s/__SEED1__/$RANDOM/"         | sed "s/__SEED2__/$RANDOM/"         \
                 | sed "s/__SEED3__/$RANDOM/"         | sed "s/__SEED4__/$RANDOM/"         \
                 | sed "s/__SEED5__/$RANDOM/"         | sed "s/__SEED6__/$RANDOM/"         \
                 | sed "s/__SEED7__/$RANDOM/"         | sed "s/__SEED8__/$RANDOM/"         \
                 | sed "s/__PTMIN__/$PTMIN/"          | sed "s/__PTMAX__/$PTMAX/"          \
                 | sed "s/__ETAMIN__/$ETAMIN/"        | sed "s/__ETAMAX__/$ETAMAX/"        \
                 | sed "s/__PHIMIN__/$PHIMIN/"        | sed "s/__PHIMAX__/$PHIMAX/"        \
    > $CFGFILE
