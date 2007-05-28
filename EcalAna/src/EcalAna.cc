// -*- C++ -*-
//
// Package:    EcalAna
// Class:      EcalAna
// 
/**\class EcalAna EcalAna.cc CLoizides/EcalAna/src/EcalAna.cc

 Description: Simple class to look at Ecal digis.

 Implementation: 
     
*/
//
// Original Author:  Constantin Loizides
//         Created:  Tue Feb 22 12:50:51 EST 2007
// $Id: EcalAna.cc,v 1.2 2007/03/01 04:15:53 loizides Exp $
//
//


// system include files
#include <memory>
#include <vector>
#include <map>

// my include filee
#include "CLoizides/EcalAna/interface/EcalAna.h"

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h" 
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Geometry/EcalBarrelAlgo/interface/EcalBarrelGeometry.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include "SimDataFormats/CaloHit/interface/PCaloHit.h"
#include "SimDataFormats/CaloHit/interface/PCaloHitContainer.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"

#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TMath.h"
#include "TString.h"

#include "CLoizides/Utils/interface/THistFileService.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
EcalAna::EcalAna(const edm::ParameterSet& iConfig) :
   ebdsrc_( iConfig.getParameter<edm::InputTag>("ebdsrc")),
   eedsrc_( iConfig.getParameter<edm::InputTag>("eedsrc")),
   ebhsrc_( iConfig.getParameter<edm::InputTag>("ebhsrc")),
   eehsrc_( iConfig.getParameter<edm::InputTag>("eehsrc")),
   simsrc_( iConfig.getParameter<edm::InputTag>("simsrc")),
   hmcsrc_( iConfig.getParameter<edm::InputTag>("hmcsrc")),
   verbose_(iConfig.getUntrackedParameter<int>("verbose",0)),
   evcounter_(0)
{
   // now do what ever initialization is needed

   edm::Service<THistFileService> fs;
   if(!fs) {
      throw edm::Exception(edm::errors::NullPointerError, "EcalAna::EcalAna()\n")
	  << "Could not get pointer to THistFileService.\n";
   }

}

EcalAna::~EcalAna()
{
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}


//
// member functions
//

// ------------ method called to for each event  ------------
void
EcalAna::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   ESHandle<CaloGeometry> pG; 
   iSetup.get<IdealGeometryRecord>().get(pG); 

   // get histo service
   edm::Service<THistFileService> fs;

   // count events
   evcounter_++;

   // create my output histo
   TString hname(Form("hPhiEta_digis_event%d",evcounter_));
   TH2D *mEtaPhi_d  = fs->make<TH2D>(hname.Data(), ";#phi;#eta",
                                     360, -TMath::Pi(), TMath::Pi(),
                                     345, -3, 3);

   hname=Form("hPhiEta_hits_event%d",evcounter_);
   TH2D *mEtaPhi_h  = fs->make<TH2D>(hname.Data(), ";#phi;#eta",
                                     360, -TMath::Pi(), TMath::Pi(),
                                     345, -3, 3);

   hname=Form("hPhiEta_parts_event%d",evcounter_);
   TH2D *mEtaPhi_s  = fs->make<TH2D>(hname.Data(), ";#phi;#eta",
                                     360, -TMath::Pi(), TMath::Pi(),
                                     345, -3, 3);

   // get hepmc event
   Handle<HepMCProduct> pHepMC;
   const HepMC::GenEvent *hmcevent = 0;
   try {
      iEvent.getByLabel(hmcsrc_, pHepMC);
      hmcevent = pHepMC->GetEvent();
      edm::LogInfo("EcalAna") << "total # HepMCTracks: " << hmcevent->particles_size() 
                              << "HepMCVertices: " <<  hmcevent->vertices_size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << hmcsrc_.encode();
   }

   // get simulated tracks
   Handle<SimTrackContainer> pParticles;
   const SimTrackContainer *Particles = 0;
   try {
      iEvent.getByLabel(simsrc_, pParticles);
      Particles = pParticles.product(); 
      edm::LogInfo("EcalAna") << "total # SimTracks: " << Particles->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << simsrc_.encode();
   }

   // get simulated vertices
   Handle<SimVertexContainer> pVertices;
   const SimVertexContainer *Vertices = 0;
   try {
      iEvent.getByLabel(simsrc_, pVertices);
      Vertices = pVertices.product(); 
      edm::LogInfo("EcalAna") << "total # SimTracks: " << Vertices->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << simsrc_.encode();
   }

   // create my simtrack map
   typedef std::map<unsigned int, const SimTrack*> SimTrackMap;
   SimTrackMap mysimtrackmap;
   for(SimTrackContainer::const_iterator itp = Particles->begin(); itp != Particles->end(); ++itp ) {
      if(verbose_) std::cout << "Simtest: " << *itp;
      int vind = itp->vertIndex();
      if(vind>=0) {
         const SimVertex &sv = (*Vertices)[vind];
         if(verbose_) std::cout << " vertex "<< sv;
      }
      int gind = itp->genpartIndex();
      if(gind>=0) {
         HepMC::GenParticle *gpart = hmcevent->barcode_to_particle(gind);
         const HepMC::GenVertex *gvert = gpart->production_vertex();
         if(verbose_) std::cout << " hepmc " << gpart->pdg_id() << " " << gvert->id() << std::endl;
         if(gpart->pdg_id()!=itp->type()) {
            Fatal("EcalAna","Particle pid do not match: %d %d", gpart->pdg_id(), itp->type());
         }
      }
      if(verbose_) std::cout << std::endl;
      mysimtrackmap.insert(std::make_pair(itp->trackId(),&(*itp)));
   }

   // create my multi map
   typedef std::multimap<unsigned int, const PCaloHit*> CaloHitMMap;
   CaloHitMMap myhitmap;

   // get EB hits
   Handle<PCaloHitContainer> pEBhits;
   const PCaloHitContainer *EBhits = 0;
   try {
      iEvent.getByLabel(ebhsrc_, pEBhits);
      EBhits = pEBhits.product(); 
      edm::LogInfo("EcalAna") << "total # EBdigis: " << EBhits->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << ebhsrc_.encode();
   }
   // loop over EB hits
   if (EBhits) {
      for(PCaloHitContainer::const_iterator itht = EBhits->begin(); itht != EBhits->end(); ++itht) {
         myhitmap.insert(std::make_pair(itht->id(),&(*itht)));
         EBDetId id(itht->id());
         const GlobalPoint& pos=pG->getPosition(id);
         mEtaPhi_h->Fill(pos.phi(),pos.eta(),itht->energyEM());
      }
   }

   // get EE hits
   Handle<PCaloHitContainer> pEEhits;
   const PCaloHitContainer *EEhits = 0;
   try {
      iEvent.getByLabel(eehsrc_, pEEhits);
      EEhits = pEEhits.product(); 
      edm::LogInfo("EcalAna") << "total # EEdigis: " << EEhits->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << ebhsrc_.encode();
   }
   // loop over EE hits
   if (EEhits) {
      for( PCaloHitContainer::const_iterator itht = EEhits->begin(); itht != EEhits->end(); ++itht) {
         myhitmap.insert(std::make_pair(itht->id(),&(*itht)));
         EEDetId id(itht->id());
         const GlobalPoint& pos=pG->getPosition(id);
         mEtaPhi_h->Fill(pos.phi(),pos.eta(),itht->energyEM());
      }
   }

   // get EB digis
   Handle<EBDigiCollection> pEBdigis;
   const EBDigiCollection* EBdigis = 0;
   try {
      iEvent.getByLabel(ebdsrc_, pEBdigis);
      EBdigis = pEBdigis.product(); 
      edm::LogInfo("EcalAna") << "total # EBdigis: " << EBdigis->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << ebdsrc_.encode();
   }
   // loop over EB digis
   if (EBdigis) {
      for(EBDigiCollection::const_iterator itdg = EBdigis->begin(); itdg != EBdigis->end(); ++itdg) {
         //search the maximum adc value
         int val=0;
         int size=itdg->size();
         for(int i=0;i<size;i++) {
            int adc = itdg->sample(i).adc();
            if (adc>val) val=adc;
         }
         const EBDetId &id=itdg->id();
         const GlobalPoint& pos=pG->getPosition(id);
         mEtaPhi_d->Fill(pos.phi(),pos.eta(),val);
         unsigned int idkey=id.rawId();  
         for (CaloHitMMap::const_iterator pos = myhitmap.lower_bound(idkey);
              pos != myhitmap.upper_bound(idkey); ++pos) {
            unsigned int gtid = pos->second->geantTrackId();
            int pid = 0;
            const SimTrack *tr = mysimtrackmap[gtid];
            if(tr) {
               mEtaPhi_s->Fill(tr->momentum().phi(),tr->momentum().eta(), tr->momentum().e());
            }
         }
      }
   }

   // get EE digis
   Handle<EEDigiCollection> pEEdigis;
   const EEDigiCollection* EEdigis = 0;
   try {
      iEvent.getByLabel(eedsrc_, pEEdigis);
      EEdigis = pEEdigis.product(); 
      edm::LogInfo("EcalAna") << "total # EEdigis: " << EEdigis->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << eedsrc_.encode();
   }

   // loop over EE digis
   if (EEdigis) {
      for(EEDigiCollection::const_iterator itdg = EEdigis->begin(); itdg != EEdigis->end(); ++itdg) {
         //search the maximum adc value
         int val=0;
         int size=itdg->size();
         for(int i=0;i<size;i++) {
            int adc = itdg->sample(i).adc();
            if (adc>val) val=adc;
         }
         const EEDetId &id=itdg->id();
         const GlobalPoint& pos=pG->getPosition(id);
         mEtaPhi_d->Fill(pos.phi(),pos.eta(),val);
      }
   }
}

// ------------ method called once each job just before starting event loop  ------------
void 
EcalAna::beginJob(const edm::EventSetup&) {

}

// ------------ method called once each job just after ending the event loop  ------------
void 
EcalAna::endJob() {
}
