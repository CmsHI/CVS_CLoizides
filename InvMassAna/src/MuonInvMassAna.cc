// -*- C++ -*-
//
// Package:    InvMassAna
// Class:      MuonInvMassAna
// 
/**\class MuonInvMassAna MuonInvMassAna.cc CLoizides/InvMassAna/src/MuonInvMassAna.cc

 Description: 

 Implementation: 
     
*/
//
// Original Author:  Constantin Loizides
//         Created:  Tue Feb 22 12:50:51 EST 2007
// $Id: MuonInvMassAna.cc,v 1.2 2007/02/22 19:21:00 loizides Exp $
//
//


// system include files
#include <memory>

// my include filee
#include "CLoizides/InvMassAna/interface/MuonInvMassAna.h"

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h" 

#include "DataFormats/HLTReco/interface/HLTFilterObject.h"
#include "DataFormats/Common/interface/RefToBase.h"
//#include "DataFormats/TrackReco/interface/Track.h"
//#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "CLoizides/Utils/interface/THistFileService.h"

#include "TH1D.h"
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
MuonInvMassAna::MuonInvMassAna(const edm::ParameterSet& iConfig) :
   src_( iConfig.getParameter<edm::InputTag>("src")),
   m_InvMass(0)
{
   // now do what ever initialization is needed

   edm::Service<THistFileService> fs;
   if(!fs) {
      throw edm::Exception(edm::errors::NullPointerError, "MuonInvMassAna::MuonInvMassAna()\n")
	  << "Could not get pointer to THistFileService.\n";
   }

   m_InvMass  = fs->makeHist<TH1D>("hInvMass", iConfig, 
                                   ";inv.mass [GeV];#", 100, 5, 15);

   if(!m_InvMass) {
      throw edm::Exception(edm::errors::NullPointerError, "MuonInvMassAna::MuonInvMassAna()\n")
         << "Could not get pointer to histogram.\n";
   }
}

MuonInvMassAna::~MuonInvMassAna()
{
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}


//
// member functions
//

// ------------ method called to for each event  ------------
void
MuonInvMassAna::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace reco;

   Handle<HLTFilterObjectWithRefs> filterproduct;
   iEvent.getByLabel(src_, filterproduct);

   std::cout<<"MuonsInvMassAna: " << filterproduct->size() << std::endl;
   for (unsigned int i=0; i<filterproduct->size(); i++) {
      RefToBase<Candidate> candref1 = filterproduct->getParticleRef(i);

      for (unsigned int j=i+1; j<filterproduct->size(); j++) {
         RefToBase<Candidate> candref2 = filterproduct->getParticleRef(j);

         Double_t invmass = (candref1->p4() + candref2->p4()).mass();
         m_InvMass->Fill(invmass);
      }
   }  

#if 0
      std::cout << "MuonInvMassAna"
                << " Track passing filter: pt= " 
                << candref->pt() 
                << ", eta: " 
                << candref->eta() 
                << std::endl;
#endif
}

// ------------ method called once each job just before starting event loop  ------------
void 
MuonInvMassAna::beginJob(const edm::EventSetup&) {

}

// ------------ method called once each job just after ending the event loop  ------------
void 
MuonInvMassAna::endJob() {
}
