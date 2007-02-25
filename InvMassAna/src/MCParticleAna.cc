// -*- C++ -*-
//
// Package:    InvMassAna
// Class:      MCParticleAna
// 
/**\class MCParticleAna MCParticleAna.cc CLoizides/InvMassAna/src/MCParticleAna.cc

 Description: This class loops over GenParticleCandiates searching for particles with
              a given pid to draw some kinematical distributions.

 Implementation: Nothing special about it. Class uses my new THistFileService
                 for the histogramming.
     
*/
//
// Original Author:  Constantin Loizides
//         Created:  Tue Feb 13 12:50:51 EST 2007
// $Id: MCParticleAna.cc,v 1.2 2007/02/22 19:21:00 loizides Exp $
//
//


// system include files
#include <memory>

// my include filee
#include "CLoizides/InvMassAna/interface/MCParticleAna.h"

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h" 

#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleCandidate.h"

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
MCParticleAna::MCParticleAna(const edm::ParameterSet& iConfig) :
   src_(iConfig.getParameter<edm::InputTag>("src")),
   pdgId_(iConfig.getParameter<int>("pdgId")),
   m_Pt(0),
   m_Eta(0),
   m_Entries(0)
{
   // now do what ever initialization is needed

   edm::Service<THistFileService> fs;
   if(!fs) {
      throw edm::Exception(edm::errors::NullPointerError, "MCParticleAna::MCParticleAna()\n")
	  << "Could not get pointer to THistFileService.\n";
   }

   m_Pt        = fs->makeHist<TH1D>("hPt", iConfig, ";p_{T} [GeV];#", 150, 0, 15);
   m_Eta       = fs->makeHist<TH1D>("hEta", iConfig, ";#eta;#", 100, -5, 5);
   m_Entries   = fs->makeHist<TH1D>("hEntries", iConfig, ";#per event;#", 10, -0.5, 9.5);
}

MCParticleAna::~MCParticleAna()
{
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}


//
// member functions
//

// ------------ method called to for each event  ------------
void
MCParticleAna::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace reco;

   Handle<CandidateCollection> particles;
   iEvent.getByLabel(src_,particles);

   int c=0;
   for(CandidateCollection::const_iterator p = particles->begin();
       p != particles->end(); ++p) {

      if(p->pdgId()!=pdgId_) continue;

      m_Pt->Fill(p->pt());
      m_Eta->Fill(p->eta());

      ++c;
   }
   m_Entries->Fill(c);
}

// ------------ method called once each job just before starting event loop  ------------
void 
MCParticleAna::beginJob(const edm::EventSetup&) {

}

// ------------ method called once each job just after ending the event loop  ------------
void 
MCParticleAna::endJob() {
}
