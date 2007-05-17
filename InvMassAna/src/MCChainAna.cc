// -*- C++ -*-
//
// Package:    InvMassAna
// Class:      MCChainAna
// 
/**\class MCChainAna MCChainAna.cc CLoizides/InvMassAna/src/MCChainAna.cc

 Description: This class prints GenParticleCandiates
     
*/
//
// Original Author:  Constantin Loizides
//         Created:  Tue Feb 13 12:50:51 EST 2007
// $Id: MCChainAna.cc,v 1.6 2007/03/01 04:15:53 loizides Exp $
//
//


// system include files
#include <memory>
#include <map>

// my include filee
#include "CLoizides/InvMassAna/interface/MCChainAna.h"

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h" 

#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/Particle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleCandidate.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
MCChainAna::MCChainAna(const edm::ParameterSet& iConfig) :
   src_(iConfig.getParameter<edm::InputTag>("src"))
{
   // now do what ever initialization is needed

}

MCChainAna::~MCChainAna()
{
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}


//
// member functions
//

// ------------ method called to for each event  ------------
void
MCChainAna::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace reco;

   Handle<CandidateCollection> particles;
   iEvent.getByLabel(src_,particles);

   // make index map
   std::map<const Candidate *, int> cmap;

   std::cout << "mcchainana" << std::endl; 
   int i=0;int off=1;
   for(CandidateCollection::const_iterator p = particles->begin();
       p != particles->end(); ++p) {
      cmap[&(*p)]=i++;
   }

#if 0
   printf("  Ind    KF   Moth  Dau1 -  Dau2 :    KS      px      py      pz\n");
#endif

   i=0;
   for(CandidateCollection::const_iterator p = particles->begin();
       p != particles->end(); ++p) {

      const Candidate *m=p->mother();
      const Candidate *dfirst=0;
      const Candidate *dlast=0;
      if(p->numberOfDaughters()>0) {
         dfirst=p->daughter(0);
         dlast=p->daughter(p->numberOfDaughters()-1);
      }
      const math::XYZTLorentzVector &mom = p->p4();
      int mindex=-1;
      if(m)
         mindex=cmap[m]+off;
      int dfirsti=-1;
      if(dfirst)
         dfirsti=cmap[dfirst]+off;
      int dlasti=-1;
      if(dlast)
         dlasti=cmap[dlast]+off;

#if 0 
      printf("%5d: %5d %5d %5d - %5d : %5d   % 4.3f  % 4.3f  % 4.3f\n", 
	     i+off, p->pdgId(), mindex, 
	     dfirsti, dlasti, 
	     p->status(), mom.X(), mom.Y(), mom.Z());
#else
      printf("eXt %6d: %6d% 10.3f% 10.3f% 10.3f%6d%10.3f% 10.3f% 10.3f "
                      "%6d% 10.3f% 10.3f% 10.3f%6d%10.3f% 10.3f% 10.3f\n", 
             i+off, p->pdgId(), mom.X(), mom.Y(), mom.Z(),
             m!=0 ? m->pdgId():0, m!=0 ? m->px():0, m!=0 ? m->py():0, m!=0 ? m->pz():0,
             dfirst!=0 ? dfirst->pdgId():0, dfirst!=0 ? dfirst->px():0, dfirst!=0 ? dfirst->py():0, dfirst!=0 ? dfirst->pz():0,
             dlast!=0 ? dlast->pdgId():0, dlast!=0 ? dlast->px():0, dlast!=0 ? dlast->py():0, dlast!=0 ? dlast->pz():0);
#endif
      const Candidate *m2=p->mother(1);
      if(p->numberOfMothers()>1) {
         printf("sec moth: %6d% 10.3f% 10.3f% 10.3f\n",
            m2!=0 ? m2->pdgId():0, m2!=0 ? m2->px():0, m2!=0 ? m2->py():0, m2!=0 ? m2->pz():0);
      }

      i++;
   }
}

// ------------ method called once each job just before starting event loop  ------------
void 
MCChainAna::beginJob(const edm::EventSetup&) {

}

// ------------ method called once each job just after ending the event loop  ------------
void 
MCChainAna::endJob() {
}
