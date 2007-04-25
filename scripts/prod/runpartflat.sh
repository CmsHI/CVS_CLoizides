#!/bin/bash

pid=$1
events=$2
pt=$3
eta=$4

tstr=condor.sh
#tstr=echo
runno=$pid
pstr=`pwd`

export EVENTS=$events
export PARTID=$pid
export PTMIN=$pt;
export PTMAX=$pt;
export ETAMIN=-$eta;
export ETAMAX=+$eta;

FILENAME=partflat-pid${pid}-eta${eta}-pt${pt}.root

$tstr gen_events.sh $pstr/PartFlat.settings $FILENAME $runno $events
