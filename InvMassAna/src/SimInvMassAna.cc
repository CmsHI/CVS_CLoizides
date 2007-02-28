// -*- C++ -*-
//
// Package:    InvMassAna
// Class:      SimInvMassAna
// 
/**\class SimInvMassAna SimInvMassAna.cc CLoizides/InvMassAna/src/SimInvMassAna.cc

 Description: This class takes SimTracks for a given pair
              of particle ids and calculates their invariant mass distribution.

 Implementation: Nothing special about it. Class uses my new THistFileService
                 for the histogramming.
     
*/
//
// Original Author:  Constantin Loizides
//         Created:  Tue Feb 13 12:50:51 EST 2007
// $Id: SimInvMassAna.cc,v 1.4 2007/02/26 17:05:56 loizides Exp $
//
//


// system include files
#include <memory>

// my include filee
#include "CLoizides/InvMassAna/interface/SimInvMassAna.h"

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
SimInvMassAna::SimInvMassAna(const edm::ParameterSet& iConfig) :
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
      throw edm::Exception(edm::errors::NullPointerError, "SimInvMassAna::SimInvMassAna()\n")
	  << "Could not get pointer to THistFileService.\n";
   }

   m_InvMass    = fs->makeHist<TH1D>("hInvMass", iConfig, ";inv.mass [GeV];#", 100, 5, 15);
   m_PtInvMass  = fs->makeHist<TH2D>("hPtvsInvMass", iConfig, ";p_{T} [GeV];inv.mass [GeV]",
                                     150, 0, 15, 100, 5, 15);
   m_EtaInvMass = fs->makeHist<TH2D>("hEtavsInvMass", iConfig, ";#eta;inv.mass [GeV]", 
                                     100, -5, 5, 100, 5, 15 );
   m_Pt1        = fs->makeHist<TH1D>("hPt1", iConfig, ";p_{T} [GeV];#", 150, 0, 15);
   m_Pt2        = fs->makeHist<TH1D>("hPt2", iConfig, ";p_{T} [GeV];#", 150, 0, 15);
   m_Eta1       = fs->makeHist<TH1D>("hEta1", iConfig, ";#eta;#", 100, -5, 5);
   m_Eta2       = fs->makeHist<TH1D>("hEta2", iConfig, ";#eta;#", 100, -5, 5);
   m_Entries1   = fs->makeHist<TH1D>("hEntries1", iConfig, ";#per event;#", 10, -0.5, 9.5);
   m_Entries2   = fs->makeHist<TH1D>("hEntries2", iConfig, ";#per event;#", 10, -0.5, 9.5);
}

SimInvMassAna::~SimInvMassAna()
{
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}


//
// member functions
//

// ------------ method called to for each event  ------------
void
SimInvMassAna::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace CLHEP;

   Handle<SimTrackContainer> particles;
   iEvent.getByLabel(src_, particles );

   int c1=0,c2=0;
   for(SimTrackContainer::const_iterator p = particles->begin();
       p != particles->end(); ++p ) {

      if(p->type()==pdgId1_) {

         m_Pt1->Fill(p->momentum().perp());
         m_Eta1->Fill(p->momentum().pseudoRapidity());
         ++c1;
      }
      if(p->type()==pdgId2_) {
         m_Pt2->Fill(p->momentum().perp());
         m_Eta2->Fill(p->momentum().pseudoRapidity());
         ++c2;
      }
   }

   m_Entries1->Fill(c1);
   m_Entries2->Fill(c2);

   for(SimTrackContainer::const_iterator p1 = particles->begin();
       p1 != particles->end(); ++p1 ) {

      if(p1->type()!=pdgId1_) continue;
      if(TMath::Abs(p1->momentum().pseudoRapidity()) > etamax_)  continue;
      if(p1->momentum().perp() < ptmin_)    continue;

      //make sure we pair with the right set of particles
      SimTrackContainer::const_iterator p2begin=particles->begin();
      if(pdgId1_==pdgId2_) p2begin=p1+1;

      for(SimTrackContainer::const_iterator p2 = p2begin;
          p2 != particles->end(); ++p2 ) {

         if(p2==p1) continue;
         if(p2->type()!=pdgId2_) continue;
         if(TMath::Abs(p2->momentum().pseudoRapidity()) > etamax_)  continue;
         if(p2->momentum().perp() < ptmin_)    continue;

         Double_t invmass = (p1->momentum() + p2->momentum()).mag();
         Double_t invpt = (p1->momentum() + p2->momentum()).perp();
         Double_t inveta = (p1->momentum() + p2->momentum()).pseudoRapidity();
         m_InvMass->Fill(invmass);
         m_PtInvMass->Fill(invpt,invmass);
         m_EtaInvMass->Fill(inveta,invmass);
      }
   }  
}

// ------------ method called once each job just before starting event loop  ------------
void 
SimInvMassAna::beginJob(const edm::EventSetup&) {

}

// ------------ method called once each job just after ending the event loop  ------------
void 
SimInvMassAna::endJob() {
}
