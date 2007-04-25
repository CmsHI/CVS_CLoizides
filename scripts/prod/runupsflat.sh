#!/bin/bash

runno=123
events=1000
tstr=condor.sh
#tstr=echo
pstr=`pwd`

for eta in 0 0.5 1 1.5 2 2.5; do 

    for pt in 2.5 5 10 20 50; do 

        export PTMIN=$pt;
        export PTMAX=$pt;

        export ETAMIN=$eta;
        export ETAMAX=$eta;
        FILENAME=upsflat-p${eta}-${pt}.root

        $tstr gen_events.sh $pstr/UpsFlat_settings $FILENAME $runno $events

        export ETAMIN=-$eta;
        export ETAMAX=-$eta;
        FILENAME=upsflat-m${eta}-${pt}.root

        $tstr gen_events.sh $pstr/UpsFlat.settings $FILENAME $runno $events
    done
done
