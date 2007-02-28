// -*- C++ -*-
//
// Package:    InvMassAna
// Class:      SimParticleAna
// 
/**\class SimParticleAna SimParticleAna.cc CLoizides/InvMassAna/src/SimParticleAna.cc

 Description: This class loops over SimTracks searching for particles with
              a given pid to draw some kinematical distributions.

 Implementation: Nothing special about it. Class uses my new THistFileService
                 for the histogramming.
     
*/
//
// Original Author:  Constantin Loizides
//         Created:  Tue Feb 13 12:50:51 EST 2007
// $Id: SimParticleAna.cc,v 1.1 2007/02/25 23:38:52 loizides Exp $
//
//


// system include files
#include <memory>

// my include filee
#include "CLoizides/InvMassAna/interface/SimParticleAna.h"

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h" 

#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Track/interface/SimTrack.h"

#include "CLHEP/Vector/LorentzVector.h"

#include "CLoizides/Utils/interface/THistFileService.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TMath.h"
#include "TString.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
SimParticleAna::SimParticleAna(const edm::ParameterSet& iConfig) :
   src_(iConfig.getParameter<edm::InputTag>("src")),
   pdgId_(iConfig.getParameter<int>("pdgId")),
   m_Pt(0),
   m_Eta(0),
   m_Entries(0)
{
   // now do what ever initialization is needed

   edm::Service<THistFileService> fs;
   if(!fs) {
      throw edm::Exception(edm::errors::NullPointerError, "SimParticleAna::SimParticleAna()\n")
	  << "Could not get pointer to THistFileService.\n";
   }

   m_Pt        = fs->makeHist<TH1D>("hPt", iConfig, ";p_{T} [GeV];#", 150, 0, 15);
   m_Eta       = fs->makeHist<TH1D>("hEta", iConfig, ";#eta;#", 100, -5, 5);
   m_Entries   = fs->makeHist<TH1D>("hEntries", iConfig, ";#per event;#", 10, -0.5, 9.5);
}

SimParticleAna::~SimParticleAna()
{
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}


//
// member functions
//

// ------------ method called to for each event  ------------
void
SimParticleAna::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace CLHEP;

   Handle<SimTrackContainer> particles;
   iEvent.getByLabel(src_, particles );

   int c=0;
   for(SimTrackContainer::const_iterator p = particles->begin();
       p != particles->end(); ++p ) {

      if(p->type()!=pdgId_) continue;

      m_Pt->Fill(p->momentum().perp());
      m_Eta->Fill(p->momentum().pseudoRapidity());

      ++c;
   }
   m_Entries->Fill(c);
}

// ------------ method called once each job just before starting event loop  ------------
void 
SimParticleAna::beginJob(const edm::EventSetup&) {

}

// ------------ method called once each job just after ending the event loop  ------------
void 
SimParticleAna::endJob() {
}
