#!/bin/bash
# $Id$

setrootsys=/opt/root/root_head
sethiroot=~/work/hiroot
setcmsarch=slc4_ia32_gcc345
setcmssoft=/opt/cmssoft/cmsset_default.sh
setcmsbase=~/work/cms/test/CMSSW_1_5_1/src
mycmsrun=$setcmsbase/CLoizides/scripts/myCmsRun
readhepmccfg=$setcmsbase/CLoizides/scripts/reco/readHepMC.cfg
readxhepmccfg=$setcmsbase/CLoizides/scripts/reco/readExtHepMC.cfg
mixcfg=$setcmsbase/CLoizides/scripts/reco/mixhepmc.cfg
nlines=100000000

nev=$1
if test -z "$nev"; then
    echo " *** Number of events set to 10 ***"
    nev=10
fi

nmix=1
typemix=HYDJET
if test -n "$2"; then
    nmix=$2
    typemix=PYTHIA_$nmix
    readxhepmccfg=$readhepmccfg
fi

runsig=112233
runback=445566

TMP=/tmp/${UID}_`date +%s`
mkdir -p $TMP
cd $TMP
echo " *** Switching to $TMP ***"
OFILE=$TMP/all-output.txt

#generate HIROOT trees
OLDLDPATH=$LD_LIBRARY_PATH
OLDPATH=$PATH
export ROOTSYS=$setrootsys
export HIROOT=$sethiroot
export HIROOT_BASEURL=$TMP
export PATH=$ROOTSYS/bin:$OLDPATH
export LD_LIBRARY_PATH=$HIROOT/lib:$ROOTSYS/lib:$OLDLDPATH

cp -a $HIROOT/macros/hictl/*.C .
cat > rootlogon.C <<EOF
{
   gROOT->Macro("$HIROOT/macros/setRootEnv.C+");
   gROOT->ProcessLine(".L createHRT.C+");
   gROOT->ProcessLine(".L createHRIT.C+");
   gROOT->ProcessLine(".L createHydjet.C+");
   gROOT->ProcessLine(".L createPythiaGamJet.C+");
   gROOT->ProcessLine(".L convertRootToHepMC.C+");
}
EOF

echo " *** Generating $nev PYTHIA events ***"
root -b -q createPythiaGamJet.C+\($nev,$runsig,250,new\ THIConstIndexGenerator\(1\)\) >> $OFILE 2>&1
echo " *** Generating $nev $typemix events ***"
if test "$typemix" = "HYDJET"; then
    root -b -q createHydjet.C+\($nev,$runback,2,0,5,0,kTRUE\) >> $OFILE 2>&1
else
    root -b -q createPythiaGamJet.C+\($nev,$runback,15,new\ THIConstIndexGenerator\($nmix\)\) >> $OFILE 2>&1
fi
echo " *** Converting $nev PYTHIA events to HEPMC ***"
root -b -q convertRootToHepMC.C+\($nlines,\"$runsig\",1,\"hrit${runsig}*.root\"\) >> $OFILE 2>&1
echo " *** Converting $nev $typemix events to HEPMC ***"
root -b -q convertRootToHepMC.C+\($nlines,\"$runback\",1,\"hrit${runback}*.root\"\) >> $OFILE 2>&1

#generate CMSSW trees
export LD_LIBRARY_PATH=$OLDLDPATH
export SCRAM_ARCH=$setcmsarch
source $setcmssoft
cd $setcmsbase
eval `scramv1 runtime -sh`
cd $TMP
echo " *** Importing $nev PYTHIA events to EDM ***"
MYINPUTFILE=file:${runsig}_r000001.hepmc MYOUTPUTFILE=edm${runsig}.root $mycmsrun $readhepmccfg >> $OFILE 2>&1
echo " *** Importing $nev $typemix events to EDM ***"
MYINPUTFILE=file:${runback}_r000001.hepmc MYOUTPUTFILE=edm${runback}.root $mycmsrun $readxhepmccfg >> $OFILE 2>&1
echo " *** MIXING $nev PYTHIA with $nev $typemix events to EDM ***"
MYINPUTFILE=file:edm${runsig}.root MYMIXINPUTFILE=file:edm${runback}.root MYOUTPUTFILE=edmmix.root $mycmsrun $mixcfg >> $OFILE 2>&1

#generate HID trees
cat > hidtest.cfg <<EOF
process TEST  = {
   source = PoolSource {
       untracked vstring fileNames = { "__MYINPUTFILE__" }
   }
   include "SimGeneral/HepPDTESSource/data/pythiapdt.cfi"
   include "Configuration/StandardSequences/data/FakeConditions.cff"
    module makeHID = HeavyIonDataTreeMaker {
        untracked bool doMC                    = true
        untracked bool doHepMCtracks           = true
        untracked bool doSimtracks             = false
        untracked bool doCheckCF               = false
        untracked bool doEcal                  = false
        untracked bool doRecHit                = false
        untracked bool doCaloJet               = false
        untracked bool doRecTrack              = false
    }
    path p1 = { makeHID }
}
EOF

echo " *** Converting $nev PYTHIA events from EDM to HID tree ***"
MYINPUTFILE=file:edm${runsig}.root $mycmsrun hidtest.cfg >> $OFILE 2>&1
mv hid000001s000.root hid${runsig}.root
echo " *** Converting $nev $typemix events from EDM to HID tree ***"
MYINPUTFILE=file:edm${runback}.root $mycmsrun hidtest.cfg >> $OFILE 2>&1
mv hid000001s000.root hid${runback}.root

cat > hidmtest.cfg <<EOF
process TEST  = {
   source = PoolSource {
       untracked vstring fileNames = { "__MYINPUTFILE__" }
   }
   include "SimGeneral/HepPDTESSource/data/pythiapdt.cfi"
   include "Configuration/StandardSequences/data/FakeConditions.cff"
    module makeHID = HeavyIonDataTreeMaker {
        untracked bool doMC                    = true
        untracked bool doHepMCtracks           = true
        untracked bool doSimtracks             = false
        untracked bool doCheckCF               = true
        untracked bool doEcal                  = false
        untracked bool doRecHit                = false
        untracked bool doCaloJet               = false
        untracked bool doRecTrack              = false
    }
    path p1 = { makeHID }
}
EOF

echo " *** Converting $nev mixed PYTHIA and $typemix events from EDM to HID tree ***"
MYINPUTFILE=file:edmmix.root $mycmsrun hidmtest.cfg >> $OFILE 2>&1
mv hid000001s000.root hidmix.root

#prepare and compare output
if ! test -e hrt${runsig}s000.root; then
    echo " !!! hrt${runsig}s000.root; not found !!!"
    exit 123;
fi
if ! test -e hrt${runsig}s000.root; then
    echo " !!! hrt${runsig}s000.root; not found !!!"
    exit 123;
fi
if ! test -e hrit${runsig}s000.root; then
    echo " !!! hrit${runsig}s000.root; not found !!!"
    exit 123;
fi
if ! test -e hrit${runsig}s000.root; then
    echo " !!! hrit${runsig}s000.root; not found !!!"
    exit 123;
fi
if ! test -e hid${runsig}.root; then
    echo " !!! hid${runsig}.root; not found !!!"
    exit 123;
fi

if ! test -e hid${runsig}.root; then
    echo " !!! hid${runsig}.root; not found !!!"
    exit 123;
fi
if ! test -e hid${runback}.root; then
    echo " !!! hid${runback}.root; not found !!!"
    exit 123;
fi
if ! test -e hidmix.root; then
    echo " !!! hidmix.root not found !!!"
    exit 123;
fi

unset CMSSW_VERSION
export ROOTSYS=$setrootsys
export HIROOT=$sethiroot
export HIROOT_BASEURL=$TMP
export LD_LIBRARY_PATH=$HIROOT/lib:$ROOTSYS/lib:$OLDLDPATH
export PATH=$ROOTSYS/bin:$OLDPATH

rm -f *.d *.so
cp -a $HIROOT/macros/ana/*.C .
ln -s printParticles.C printMixParticles.C

cat > rootlogon.C <<EOF
{
   gROOT->Macro("$HIROOT/macros/setRootEnv.C+");
}
EOF

root -b -q printMixParticles.C+\(1,\"hrit${runsig}s000.root\"\) 2>>$OFILE | grep eXt > hrt${runsig}.out
root -b -q printMixParticles.C+\(1,\"hrit${runback}s000.root\"\) 2>>$OFILE | grep eXt > hrt${runback}.out
root -b -q printMixParticles.C+\(2,\"hrit${runsig}s000.root\",\"hrit${runback}s000.root\"\) 2>>$OFILE | grep eXt > hritmixed.out
root -b -q printParticles.C+\(\"hid${runsig}.root\",\"HID\"\) 2>>$OFILE | grep eXt > hid${runsig}.out
root -b -q printParticles.C+\(\"hid${runback}.root\",\"HID\"\) 2>>$OFILE | grep eXt > hid${runback}.out
root -b -q printParticles.C+\(\"hidmix.root\",\"HID\"\) 2>>$OFILE | grep eXt > hidmixed.out

if test -n "`diff hrt${runsig}.out hid${runsig}.out`"; then
    echo "---------------------------------------------------------"
    echo "        *** Test FAILED -*- Test FAILED ***"
    echo " Error: Signal HRT and signal HID differ"
    echo " (diff $TMP/hrt${runsig}.out  $TMP/hid${runsig}.out)"
    echo "---------------------------------------------------------"
    echo
    exit 123;
fi
if test -n "`diff hrt${runback}.out hid${runback}.out`"; then
    echo "---------------------------------------------------------"
    echo "        *** Test FAILED -*- Test FAILED ***"
    echo " Error: Background HRT and background HID differ"
    echo " (diff $TMP/hrt${runback}.out $TMP/hid${runback}.out)"
    echo "---------------------------------------------------------"
    echo
    exit 123;
fi
if test -n "`diff hritmixed.out hidmixed.out`"; then
    echo "---------------------------------------------------------"
    echo "        *** Test FAILED -*- Test FAILED ***"
    echo " Error: Mixed HRIT and mixed HID differ"
    echo " (diff $TMP/hritmixed.out $TMP/hidmixed.out)"
    echo "---------------------------------------------------------"
    echo
    exit 123;
fi

echo
echo "*** Test for $nev events sucessfully completed! ***"
echo

cd $HIROOT
rm -rf $TMP
exit 0;
