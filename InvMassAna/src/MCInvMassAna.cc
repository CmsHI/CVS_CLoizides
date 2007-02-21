// -*- C++ -*-
//
// Package:    MCInvMassAna
// Class:      MCInvMassAna
// 
/**\class MCInvMassAna MCInvMassAna.cc MC/MCInvMassAna/src/MCInvMassAna.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Constantin Loizides
//         Created:  Tue Feb 13 12:50:51 EST 2007
// $Id$
//
//


// system include files
#include <memory>

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

#include "PhysicsTools/UtilAlgos/interface/TFileService.h"

#include "TH1D.h"
#include "TString.h"

//
// class declaration
//

class MCInvMassAna : public edm::EDAnalyzer {
   public:
      explicit MCInvMassAna(const edm::ParameterSet&);
      ~MCInvMassAna();

   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      // ----------member data ---------------------------
      edm::InputTag src_;
      int pdgId1_;
      int pdgId2_;
      TH1D *m_InvMass;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
MCInvMassAna::MCInvMassAna(const edm::ParameterSet& iConfig) :
   src_( iConfig.getParameter<edm::InputTag>( "src" ) ),
   pdgId1_( iConfig.getParameter<int>( "pdgId1") ),
   pdgId2_( iConfig.getParameter<int>( "pdgId2") )
{
   // now do what ever initialization is needed

}


MCInvMassAna::~MCInvMassAna()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
void
MCInvMassAna::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   //using namespace HepMC;
   using namespace reco;

   Handle<CandidateCollection> particles;
   iEvent.getByLabel( src_, particles );
   for( CandidateCollection::const_iterator p1 = particles->begin();
                                            p1 != particles->end(); ++p1 ) {

      if(p1->pdgId()!=pdgId1_) continue;

      //make sure we pair with the right set of particles
      CandidateCollection::const_iterator p2begin=particles->begin();
      if(pdgId1_==pdgId2_) p2begin=p1+1;

      for( CandidateCollection::const_iterator p2 = p2begin;
                                               p2 != particles->end(); ++p2 ) {

         if(p2==p1) continue;
         if(p2->pdgId()!=pdgId2_) continue;

         //std::cout << p1->pdgId() << " " << p1->px() << " " << p1->py() << " " << p1->pz() << std::endl;
         //std::cout << "Found: " << p2->pdgId() << " " << p2->px() << " " << p2->py() << " " << p2->pz() << std::endl;
         Double_t invmass = (p1->p4() + p2->p4()).mass();
         //std::cout << "Found: " << p1->pdgId() << " " << p2->pdgId() << " " << invmass << std::endl;
         m_InvMass->Fill(invmass);
      }
   }  
}

// ------------ method called once each job just before starting event loop  ------------
void 
MCInvMassAna::beginJob(const edm::EventSetup&) {

   edm::Service<TFileService> fs;
   m_InvMass  = fs->make<TH1D>(Form("InvariantMassPdg%dPdg%d",pdgId1_,pdgId2_), 
                               ";inv.mass [GeV];#",100,0,100);
}

// ------------ method called once each job just after ending the event loop  ------------
void 
MCInvMassAna::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(MCInvMassAna);
