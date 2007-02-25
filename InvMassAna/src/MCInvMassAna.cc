// -*- C++ -*-
//
// Package:    InvMassAna
// Class:      MCInvMassAna
// 
/**\class MCInvMassAna MCInvMassAna.cc CLoizides/InvMassAna/src/MCInvMassAna.cc

 Description: This class takes GenParticleCandiates for a given pair
              of particle ids and calculates their invariant mass distribution.

 Implementation: Nothing special about it. Class uses my new THistFileService
                 for the histogramming.
     
*/
//
// Original Author:  Constantin Loizides
//         Created:  Tue Feb 13 12:50:51 EST 2007
// $Id: MCInvMassAna.cc,v 1.2 2007/02/22 19:21:00 loizides Exp $
//
//


// system include files
#include <memory>

// my include filee
#include "CLoizides/InvMassAna/interface/MCInvMassAna.h"

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
MCInvMassAna::MCInvMassAna(const edm::ParameterSet& iConfig) :
   src_(iConfig.getParameter<edm::InputTag>("src")),
   pdgId1_(iConfig.getParameter<int>("pdgId1")),
   pdgId2_(iConfig.getParameter<int>("pdgId2")),
   ptmin_(iConfig.getUntrackedParameter<double>("ptmin",0.)),
   etamax_(iConfig.getUntrackedParameter<double>("etamax",5.)),
   m_InvMass(0),
   m_PtInvMass(0),
   m_EtaInvMass(0),
   m_Pt1(0),
   m_Pt2(0),
   m_Eta1(0),
   m_Eta2(0),
   m_Entries1(0),
   m_Entries2(0)
{
   // now do what ever initialization is needed

   edm::Service<THistFileService> fs;
   if(!fs) {
      throw edm::Exception(edm::errors::NullPointerError, "MCInvMassAna::MCInvMassAna()\n")
	  << "Could not get pointer to THistFileService.\n";
   }

   m_InvMass    = fs->makeHist<TH1D>("hInvMass", iConfig, ";inv.mass [GeV];#", 100, 5, 15);
   m_PtInvMass  = fs->makeHist<TH2D>("hPtvsInvMass", iConfig, ";inv.mass [GeV];p_{T} [GeV]",
                                    100, 5, 15, 150, 0, 15);
   m_EtaInvMass = fs->makeHist<TH2D>("hEtavsInvMass", iConfig, ";inv.mass [GeV];#eta", 
                                     100, 5, 15, 100, -5, 5);
   m_Pt1        = fs->makeHist<TH1D>("hPt1", iConfig, ";p_{T} [GeV];#", 150, 0, 15);
   m_Pt2        = fs->makeHist<TH1D>("hPt2", iConfig, ";p_{T} [GeV];#", 150, 0, 15);
   m_Eta1       = fs->makeHist<TH1D>("hEta1", iConfig, ";#eta;#", 100, -5, 5);
   m_Eta2       = fs->makeHist<TH1D>("hEta2", iConfig, ";#eta;#", 100, -5, 5);
   m_Entries1   = fs->makeHist<TH1D>("hEntries1", iConfig, ";#per event;#", 10, -0.5, 9.5);
   m_Entries2   = fs->makeHist<TH1D>("hEntries2", iConfig, ";#per event;#", 10, -0.5, 9.5);
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
   using namespace reco;

   Handle<CandidateCollection> particles;
   iEvent.getByLabel(src_,particles);

   int c1=0,c2=0;
   for(CandidateCollection::const_iterator p = particles->begin();
       p != particles->end(); ++p) {

      if(p->pdgId()==pdgId1_) {
         m_Pt1->Fill(p->pt());
         m_Eta1->Fill(p->eta());
         ++c1;
      }
      if(p->pdgId()==pdgId2_) {
         m_Pt2->Fill(p->pt());
         m_Eta2->Fill(p->eta());
         ++c2;
      }
   }
   m_Entries1->Fill(c1);
   m_Entries2->Fill(c1);

   for(CandidateCollection::const_iterator p1 = particles->begin();
       p1 != particles->end(); ++p1) {

      if(p1->pdgId()!=pdgId1_) continue;
      if(TMath::Abs(p1->eta()) > etamax_)  continue;
      if(p1->pt() < ptmin_)    continue;

      //make sure we pair with the right set of particles
      CandidateCollection::const_iterator p2begin=particles->begin();
      if(pdgId1_==pdgId2_) p2begin=p1+1;

      for(CandidateCollection::const_iterator p2 = p2begin;
          p2 != particles->end(); ++p2) {

         if(p2==p1) continue;
         if(p2->pdgId()!=pdgId2_) continue;
         if(TMath::Abs(p2->eta()) > etamax_)  continue;
         if(p2->pt() < ptmin_)    continue;

         Double_t invmass = (p1->p4() + p2->p4()).mass();
         Double_t invpt = (p1->p4() + p2->p4()).pt();
         Double_t inveta = (p1->p4() + p2->p4()).eta();
         m_InvMass->Fill(invmass);
         m_PtInvMass->Fill(invpt,invmass);
         m_EtaInvMass->Fill(inveta,invmass);
      }
   }  
}

// ------------ method called once each job just before starting event loop  ------------
void 
MCInvMassAna::beginJob(const edm::EventSetup&) {

}

// ------------ method called once each job just after ending the event loop  ------------
void 
MCInvMassAna::endJob() {
}
