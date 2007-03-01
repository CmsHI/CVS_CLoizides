// -*- C++ -*-
//
// Package:    InvMassAna
// Class:      MuonHLTInvMassAna
// 
/**\class MuonHLTInvMassAna MuonHLTInvMassAna.cc CLoizides/InvMassAna/src/MuonHLTInvMassAna.cc

 Description: 

 Implementation: 
     
*/
//
// Original Author:  Constantin Loizides
//         Created:  Tue Feb 22 12:50:51 EST 2007
// $Id: MuonHLTInvMassAna.cc,v 1.1 2007/02/25 23:38:52 loizides Exp $
//
//


// system include files
#include <memory>

// my include filee
#include "CLoizides/InvMassAna/interface/MuonHLTInvMassAna.h"

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

#include "CLoizides/Utils/interface/THistFileService.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TMath.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
MuonHLTInvMassAna::MuonHLTInvMassAna(const edm::ParameterSet& iConfig) :
   src_( iConfig.getParameter<edm::InputTag>("src")),
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
      throw edm::Exception(edm::errors::NullPointerError, "MuonHLTInvMassAna::MuonHLTInvMassAna()\n")
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

MuonHLTInvMassAna::~MuonHLTInvMassAna()
{
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}


//
// member functions
//

// ------------ method called to for each event  ------------
void
MuonHLTInvMassAna::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace reco;

   Handle<HLTFilterObjectWithRefs> filterproduct;
   iEvent.getByLabel(src_, filterproduct);

   int c1=0,c2=0;
   for (unsigned int i=0; i<filterproduct->size(); i++) {
      RefToBase<Candidate> p = filterproduct->getParticleRef(i);

      if(p->charge()<0) {
         m_Pt1->Fill(p->pt());
         m_Eta1->Fill(p->eta());
         ++c1;
      }
      if(p->charge()>0) {
         m_Pt2->Fill(p->pt());
         m_Eta2->Fill(p->eta());
         ++c2;
      }
   }
   m_Entries1->Fill(c1);
   m_Entries2->Fill(c2);


   for (unsigned int i=0; i<filterproduct->size(); i++) {
      RefToBase<Candidate> p1 = filterproduct->getParticleRef(i);

      if(TMath::Abs(p1->eta()) > etamax_)  continue;
      if(p1->pt() < ptmin_)    continue;

      for (unsigned int j=i+1; j<filterproduct->size(); j++) {
         RefToBase<Candidate> p2 = filterproduct->getParticleRef(j);

         if(p2==p1) continue;
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
MuonHLTInvMassAna::beginJob(const edm::EventSetup&) {

}

// ------------ method called once each job just after ending the event loop  ------------
void 
MuonHLTInvMassAna::endJob() {
}
