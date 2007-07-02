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
// $Id: EcalAna.cc,v 1.2 2007/06/05 15:20:42 loizides Exp $
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

#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
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

#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/ClusterShape.h"

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
   ebdsrc_(iConfig.getParameter<edm::InputTag>("ebdsrc")),
   eedsrc_(iConfig.getParameter<edm::InputTag>("eedsrc")),
   ebhsrc_(iConfig.getParameter<edm::InputTag>("ebhsrc")),
   eehsrc_(iConfig.getParameter<edm::InputTag>("eehsrc")),
   ebrsrc_(iConfig.getParameter<edm::InputTag>("ebrsrc")),
   eersrc_(iConfig.getParameter<edm::InputTag>("eersrc")),
   ebcsrc_(iConfig.getParameter<edm::InputTag>("ebcsrc")),
   eecsrc_(iConfig.getParameter<edm::InputTag>("eecsrc")),
   ebssrc_(iConfig.getParameter<edm::InputTag>("ebssrc")),
   eessrc_(iConfig.getParameter<edm::InputTag>("eessrc")),
   ebscsrc_(iConfig.getParameter<edm::InputTag>("ebscsrc")),
   eescsrc_(iConfig.getParameter<edm::InputTag>("eescsrc")),
   hmcsrc_(iConfig.getParameter<edm::InputTag>("hmcsrc")),
   simsrc_(iConfig.getParameter<edm::InputTag>("simsrc")),
   minet_(iConfig.getUntrackedParameter<double>("minet",5)),
   minetpart_(iConfig.getUntrackedParameter<double>("minetpart",5)),
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
   TString hname;
   hname=Form("hEtaPhi_parts_event%d",evcounter_);
   TH2D *mEtaPhi_p  = fs->make<TH2D>(hname.Data(), ";#eta;#phi",
                                     345, -3, 3,
                                     360, -TMath::Pi(), TMath::Pi());

   hname=Form("hEtaPhi_hits_event%d",evcounter_);
   TH2D *mEtaPhi_h  = fs->make<TH2D>(hname.Data(),  ";#eta;#phi",
                                     345, -3, 3,
                                     360, -TMath::Pi(), TMath::Pi());

   hname=Form("hEtaPhi_digis_event%d",evcounter_);
   TH2D *mEtaPhi_d  = fs->make<TH2D>(hname.Data(), ";#eta;#phi",
                                     345, -3, 3,
                                     360, -TMath::Pi(), TMath::Pi());

   hname=Form("hEtaPhi_digiparts_event%d",evcounter_);
   TH2D *mEtaPhi_s1 = fs->make<TH2D>(hname.Data(),  ";#eta;#phi",
                                     345, -3, 3,
                                     360, -TMath::Pi(), TMath::Pi());

   hname=Form("hEtaPhi_rechits_event%d",evcounter_);
   TH2D *mEtaPhi_r  = fs->make<TH2D>(hname.Data(),  ";#eta;#phi",
                                     345, -3, 3,
                                     360, -TMath::Pi(), TMath::Pi());

   hname=Form("hEtaPhi_rechitsmarked_event%d",evcounter_);
   TH2D *mEtaPhi_rm = fs->make<TH2D>(hname.Data(),  ";#eta;#phi",
                                     345, -3, 3,
                                     360, -TMath::Pi(), TMath::Pi());

   hname=Form("hEtaPhi_rechitparts_event%d",evcounter_);
   TH2D *mEtaPhi_s2 = fs->make<TH2D>(hname.Data(),  ";#eta;#phi",
                                     345, -3, 3,
                                     360, -TMath::Pi(), TMath::Pi());

   hname=Form("hEtaPhi_islandclusters_event%d",evcounter_);
   TH2D *mEtaPhi_ic = fs->make<TH2D>(hname.Data(),  ";#eta;#phi",
                                     345, -3, 3,
                                     360, -TMath::Pi(), TMath::Pi());

   hname=Form("hEtaPhi_superclusters_event%d",evcounter_);
   TH2D *mEtaPhi_sc = fs->make<TH2D>(hname.Data(),  ";#eta;#phi",
                                     345, -3, 3,
                                     360, -TMath::Pi(), TMath::Pi());

   // get hepmc event
   getHepMC(iEvent,iSetup);
   // get simulated tracks
   getSimTracks(iEvent,iSetup);
   // get simulated vertices
   getSimVertices(iEvent,iSetup);
   // get EB hits
   getEBCaloHits(iEvent,iSetup);
   // get EE hits
   getEECaloHits(iEvent,iSetup);
   // get EB digis
   getEBDigis(iEvent,iSetup);
   // get EE digis
   getEEDigis(iEvent,iSetup);
   // get EB rec hits
   getEBRecHits(iEvent,iSetup);
   // get EE rec hits
   getEERecHits(iEvent,iSetup);
   // get EB clusters
   getEBIslandClusters(iEvent,iSetup);
   // get EB shapes
   getEBClusterShapes(iEvent,iSetup);
   // get EB super clusters
   getEBSuperClusters(iEvent,iSetup);
   // get EE clusters
   getEEIslandClusters(iEvent,iSetup);
   // get EE shapes
   getEEClusterShapes(iEvent,iSetup);
   // get EE super clusters
   getEESuperClusters(iEvent,iSetup);
   // create my simtrack map
   simtrackmap_ = createSimTrackMap();
   // create my multi map
   hitmap_ = createHitMMap();
   // create my used map
   stusedmap_ = createSTUsedMap();

   // get main vertex
   zvertex_=0;
   rvertex_=0;
   if(hmcevent_) {
      using namespace HepMC;

      Double_t x=0,y=0,z=0;
      Int_t counter=0;

      for(GenEvent::vertex_const_iterator vit = hmcevent_->vertices_begin(); 
          vit != hmcevent_->vertices_end(); ++vit) {
      
         const GenVertex *v = *vit;
         if(v->particles_in_size()!=0)
            continue;
         x+=v->position().x();
         y+=v->position().y();
         z+=v->position().z();
         counter++;
      }
      x/=counter/10;
      y/=counter/10;
      z/=counter/10;
      zvertex_=z;
      rvertex_=TMath::Sqrt(x*x+y*y);
   }
   
   // loop over sim tracks
   for(SimTrackContainer::const_iterator itp = Particles_->begin(); 
       itp != Particles_->end(); ++itp ) {
      if(itp->momentum().et()>minetpart_) {
         float eta=ecalEta(itp->momentum().eta(),zvertex_,rvertex_);
         mEtaPhi_p->Fill(eta, itp->momentum().phi(), itp->momentum().et());
         std::cout << "Large et track: " << itp->momentum().et() << " charge " 
                   << itp->charge() << " pid " << *itp << std::endl;
      }
   }

   // loop over EB hits
   if (EBhits_) { 
      for(PCaloHitContainer::const_iterator itht = EBhits_->begin(); itht != EBhits_->end(); ++itht) {
         fillEt(mEtaPhi_h, EBDetId(itht->id()), itht->energyEM());
      }
   }

   // loop over EE hits
   if (EEhits_) { 
      for(PCaloHitContainer::const_iterator itht = EEhits_->begin(); itht != EEhits_->end(); ++itht) {
         fillEt(mEtaPhi_h, EEDetId(itht->id()), itht->energyEM());
      }
   }

   // reset map
   resetSTUsedMap(stusedmap_);

   // loop over EB digis
   if (EBdigis_) {
      for(EBDigiCollection::const_iterator itdg = EBdigis_->begin(); 
          itdg != EBdigis_->end(); ++itdg) {

         int val=0; //search the maximum adc value
         int size=itdg->size();
         for(int i=0;i<size;i++) {
            int adc = itdg->sample(i).adc();
            if (adc>val) val=adc;
         }
         fillH2(mEtaPhi_d, itdg->id(), val);
         fillTrackEt(mEtaPhi_s1, itdg->id().rawId());
      }
   }

   // loop over EE digis
   if (EEdigis_) {
      for(EEDigiCollection::const_iterator itdg = EEdigis_->begin(); 
          itdg != EEdigis_->end(); ++itdg) {
         
         int val=0; //search the maximum adc value
         int size=itdg->size();
         for(int i=0;i<size;i++) {
            int adc = itdg->sample(i).adc();
            if (adc>val) val=adc;
         }
         fillH2(mEtaPhi_d, itdg->id(), val);
         fillTrackEt(mEtaPhi_s1, itdg->id().rawId());
      }
   }

   // reset map
   resetSTUsedMap(stusedmap_);

   // loop over EB rec hits
   if (EBrechits_) {
      for(EBRecHitCollection::const_iterator itht = EBrechits_->begin(); 
          itht != EBrechits_->end(); ++itht) {

         fillEt(mEtaPhi_r,  itht->id(), itht->energy());
         fillEt(mEtaPhi_rm, itht->id(), itht->energy());
         fillTrackEt(mEtaPhi_s2, itht->id().rawId());
      }
   }

   // loop over EE rec hits
   if (EErechits_) {
      for( EERecHitCollection::const_iterator itht = EErechits_->begin(); 
           itht != EErechits_->end(); ++itht) {

         fillEt(mEtaPhi_r, itht->id(), itht->energy());
         fillEt(mEtaPhi_rm, itht->id(), itht->energy());
         fillTrackEt(mEtaPhi_s2, itht->id().rawId());
      }
   }

   // loop over EB island clusters
   if (EBclusters_) {
      for(reco::BasicClusterCollection::const_iterator iclu = EBclusters_->begin();
          iclu != EBclusters_->end(); ++iclu) {
         double et = iclu->energy()*sin(iclu->position().theta());
         if(et>minet_)
            mEtaPhi_ic->Fill(iclu->eta(), iclu->phi(), et);
      }
   }

   // loop over EE island clusters
   if (EEclusters_) {
      for(reco::BasicClusterCollection::const_iterator iclu = EEclusters_->begin();
          iclu != EEclusters_->end(); ++iclu) {
         double et = iclu->energy()*sin(iclu->position().theta());
         if(et>minet_)
            mEtaPhi_ic->Fill(iclu->eta(), iclu->phi(), et);
      }
   }

   // loop over EB super clusters
   if (EBsclusters_) {
      for(reco::SuperClusterCollection::const_iterator sclu = EBsclusters_->begin();
          sclu != EBsclusters_->end(); ++sclu) {
         double et = sclu->energy()*sin(sclu->position().theta());
         if(et>minet_)
            mEtaPhi_sc->Fill(sclu->eta(), sclu->phi(), et);
      }
   }

   // loop over EE super clusters
   if (EEsclusters_) {
      for(reco::SuperClusterCollection::const_iterator sclu = EEsclusters_->begin();
          sclu != EEsclusters_->end(); ++sclu) {
         double et = sclu->energy()*sin(sclu->position().theta());
         if(et>minet_)
            mEtaPhi_sc->Fill(sclu->eta(), sclu->phi(), et);
      }
   }
}

// ------------ method called once each job just before starting event loop  ------------
void 
EcalAna::beginJob(const edm::EventSetup &iSetup) {

   using namespace edm;

   ESHandle<CaloGeometry> pG; 
   iSetup.get<IdealGeometryRecord>().get(pG); 
   cgeo_ = pG.product();
}

// ------------ method called once each job just after ending the event loop  ------------
void 
EcalAna::endJob() {
}

// ------------ filling methods ------------
void 
EcalAna::fillH2(TH2 *h, const DetId &id, double weight) {
   const GlobalPoint& pos=cgeo_->getPosition(id);
   h->Fill(pos.eta(), pos.phi(), weight);
}

void 
EcalAna::fillEt(TH2 *h, const DetId &id, double energy) {
   const GlobalPoint& pos=cgeo_->getPosition(id);
   double et = energy*sin(pos.theta());
   if(et>minet_)
      h->Fill(pos.eta(), pos.phi(), et);
}

void 
EcalAna::fillTrackEt(TH2 *h, unsigned int rawid) {

   for (CaloHitMMap::const_iterator pos = hitmap_->lower_bound(rawid);
        pos != hitmap_->upper_bound(rawid); ++pos) {
      unsigned int gtid = pos->second->geantTrackId();
      const SimTrack *tr = (*simtrackmap_)[gtid];
      if(tr) {
         (*stusedmap_)[tr]++;

         if(tr->momentum().et()>minetpart_) continue;
   
         if((*stusedmap_)[tr]==1) {
            float eta=ecalEta(tr->momentum().eta(),zvertex_,rvertex_);
            h->Fill(eta, tr->momentum().phi(), tr->momentum().et());
         } else {
            if(verbose_) std::cout << *tr << " got " << (*stusedmap_)[tr] << " calls." << std::endl;
         }
      }
   }
}

// ------------ setup methods ------------
void 
EcalAna::getHepMC(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

   using namespace edm;

   Handle<HepMCProduct> pHepMC;
   //const HepMC::GenEvent *hmcevent = 0;
   try {
      iEvent.getByLabel(hmcsrc_, pHepMC);
      hmcevent_ = pHepMC->GetEvent();
      edm::LogInfo("EcalAna") << "total # HepMCTracks: " << hmcevent_->particles_size() 
                              << ", HepMCVertices: "     <<  hmcevent_->vertices_size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << hmcsrc_.encode();
   }
}

void 
EcalAna::getSimTracks(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

   using namespace edm;

   Handle<SimTrackContainer> pParticles;
   try {
      iEvent.getByLabel(simsrc_, pParticles);
      Particles_ = pParticles.product(); 
      edm::LogInfo("EcalAna") << "total # SimTracks: " << Particles_->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << simsrc_.encode();
   }
}

void 
EcalAna::getSimVertices(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

   using namespace edm;

   Handle<SimVertexContainer> pVertices;
   try {
      iEvent.getByLabel(simsrc_, pVertices);
      Vertices_ = pVertices.product(); 
      edm::LogInfo("EcalAna") << "total # SimVertices: " << Vertices_->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << simsrc_.encode();
   }
}

void 
EcalAna::getEBCaloHits(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

   using namespace edm;

   Handle<PCaloHitContainer> pEBhits;
   try {
      iEvent.getByLabel(ebhsrc_, pEBhits);
      EBhits_ = pEBhits.product(); 
      edm::LogInfo("EcalAna") << "total # EBhits: " << EBhits_->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << ebhsrc_.encode();
   }
}

void 
EcalAna::getEECaloHits(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

   using namespace edm;

   Handle<PCaloHitContainer> pEEhits;
   try {
      iEvent.getByLabel(eehsrc_, pEEhits);
      EEhits_ = pEEhits.product(); 
      edm::LogInfo("EcalAna") << "total # EEhits: " << EEhits_->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << eehsrc_.encode();
   }
}

void 
EcalAna::getEBDigis(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

   using namespace edm;

   Handle<EBDigiCollection> pEBdigis;
   try {
      iEvent.getByLabel(ebdsrc_, pEBdigis);
      EBdigis_ = pEBdigis.product(); 
      edm::LogInfo("EcalAna") << "total # EBdigis: " << EBdigis_->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << ebdsrc_.encode();
   }
}

void 
EcalAna::getEEDigis(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

   using namespace edm;

   Handle<EEDigiCollection> pEEdigis;
   try {
      iEvent.getByLabel(eedsrc_, pEEdigis);
      EEdigis_ = pEEdigis.product(); 
      edm::LogInfo("EcalAna") << "total # EEdigis: " << EEdigis_->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << eedsrc_.encode();
   }
}


void 
EcalAna::getEBRecHits(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

   using namespace edm;

   Handle<EBRecHitCollection> pEBrechits;
   try {
      iEvent.getByLabel(ebrsrc_, pEBrechits);
      EBrechits_ = pEBrechits.product(); 
      edm::LogInfo("EcalAna") << "total # EBrechits: " << EBrechits_->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << ebrsrc_.encode();
   }
}

void 
EcalAna::getEERecHits(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

   using namespace edm;

   Handle<EERecHitCollection> pEErechits;
   try {
      iEvent.getByLabel(ebrsrc_, pEErechits);
      EErechits_ = pEErechits.product(); 
      edm::LogInfo("EcalAna") << "total # EBrechits: " << EBrechits_->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << ebrsrc_.encode();
   }
}

void 
EcalAna::getEBIslandClusters(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

   using namespace edm;

   Handle<reco::BasicClusterCollection> pEBclusters;
   try {
      iEvent.getByLabel(ebcsrc_, pEBclusters);
      EBclusters_ = pEBclusters.product(); 
      edm::LogInfo("EcalAna") << "total # EBclusters: " << EBclusters_->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << ebcsrc_.encode();
   }
}

void 
EcalAna::getEEIslandClusters(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

   using namespace edm;

   Handle<reco::BasicClusterCollection> pEEclusters;
   try {
      iEvent.getByLabel(ebcsrc_, pEEclusters);
      EEclusters_ = pEEclusters.product(); 
      edm::LogInfo("EcalAna") << "total # EEclusters: " << EEclusters_->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << eecsrc_.encode();
   }
}

void 
EcalAna::getEBClusterShapes(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

   using namespace edm;

   Handle<reco::ClusterShapeCollection> pEBshapes;
   try {
      iEvent.getByLabel(ebssrc_, pEBshapes);
      EBshapes_ = pEBshapes.product(); 
      edm::LogInfo("EcalAna") << "total # EBshapes: " << EBshapes_->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << ebssrc_.encode();
   }
}

void 
EcalAna::getEEClusterShapes(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

   using namespace edm;

   Handle<reco::ClusterShapeCollection> pEEshapes;
   try {
      iEvent.getByLabel(eessrc_, pEEshapes);
      EEshapes_ = pEEshapes.product(); 
      edm::LogInfo("EcalAna") << "total # EEshapes: " << EEshapes_->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << eessrc_.encode();
   }
}

void 
EcalAna::getEBSuperClusters(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

   using namespace edm;

   Handle<reco::SuperClusterCollection> pEBsclusters;
   try {
      iEvent.getByLabel(ebscsrc_, pEBsclusters);
      EBsclusters_ = pEBsclusters.product(); 
      edm::LogInfo("EcalAna") << "total # EBsclusters: " << EBsclusters_->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << ebscsrc_.encode();
   }
}

void 
EcalAna::getEESuperClusters(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

   using namespace edm;

   Handle<reco::SuperClusterCollection> pEEsclusters;
   try {
      iEvent.getByLabel(ebscsrc_, pEEsclusters);
      EEsclusters_ = pEEsclusters.product(); 
      edm::LogInfo("EcalAna") << "total # EEsclusters: " << EEsclusters_->size();
   } catch (...) {
      edm::LogError("EcalAna") << "Error! can't get the product " << eescsrc_.encode();
   }
}

#ifdef notnow      
template <class T> T* 
EcalAna::getData(const edm::Event&, const edm::EventSetup&) {

   using namespace edm;

   Handle<T> pHandle;
   const T *ret = 0;
   try {
      iEvent.getByLabel(ebrsrc_, p);
      ret = pHandle.product(); 
      //edm::LogInfo("EcalAna") << "total # EBrechits: " << EBrechits_->size();
   } 
   return ret;
}
#endif

// ------------ make maps ------------

EcalAna::SimTrackMap*
EcalAna::createSimTrackMap() {

   using namespace edm;

   SimTrackMap *ret = new SimTrackMap;
   
   for(SimTrackContainer::const_iterator itp = Particles_->begin(); 
       itp != Particles_->end(); ++itp ) {
      if(verbose_) std::cout << "Simtest: " << *itp;
      int vind = itp->vertIndex();
      if(vind>=0) {
         const SimVertex &sv = (*Vertices_)[vind];
         if(verbose_) std::cout << " vertex "<< sv;
      }
      int gind = itp->genpartIndex();
      if(gind>=0) {
         HepMC::GenParticle *gpart = hmcevent_->barcode_to_particle(gind);
         const HepMC::GenVertex *gvert = gpart->production_vertex();
         if(verbose_) std::cout << " hepmc " << gpart->pdg_id() << " " << gvert->id() << std::endl;
         if(gpart->pdg_id()!=itp->type()) {
            Fatal("EcalAna","Particle pid do not match: %d %d", gpart->pdg_id(), itp->type());
         }
      }
      if(verbose_) std::cout << std::endl;
      ret->insert(std::make_pair(itp->trackId(),&(*itp)));
   }

   return ret;
}

EcalAna::STUsedMap*
EcalAna::createSTUsedMap() {

   using namespace edm;

   STUsedMap *ret = new STUsedMap;
   for(SimTrackContainer::const_iterator itp = Particles_->begin(); 
       itp != Particles_->end(); ++itp ) {
     ret->insert(std::make_pair(&(*itp),0));
   }

   return ret;
}

void 
EcalAna::resetSTUsedMap(EcalAna::STUsedMap *map) {

   for (STUsedMap::iterator pos = map->begin();
        pos != map->end(); ++pos) {
      pos->second=0;
   }
}

EcalAna::CaloHitMMap*
EcalAna::createHitMMap() {

   using namespace edm;

   CaloHitMMap *ret = new CaloHitMMap;
   if (EBhits_) { // loop over EB hits
      for(PCaloHitContainer::const_iterator itht = EBhits_->begin(); itht != EBhits_->end(); ++itht) {
         ret->insert(std::make_pair(itht->id(),&(*itht)));
      }
   }
   if (EEhits_) { // loop over EE hits
      for(PCaloHitContainer::const_iterator itht = EEhits_->begin(); itht != EEhits_->end(); ++itht) {
         ret->insert(std::make_pair(itht->id(),&(*itht)));
      }
   }
   return ret;
}

// ------------ transform coordinates ------------
float EcalAna::ecalEta(float EtaParticle, float Zvertex, float plane_Radius)
{  
   const float R_ECAL           = 136.5;
   const float Z_Endcap         = 328.0;
   const float etaBarrelEndcap  = 1.479;
 
   if(EtaParticle != 0.) {
      float Theta = 0.0  ;
      float ZEcal = (R_ECAL-plane_Radius)*sinh(EtaParticle)+Zvertex;
 
      if(ZEcal != 0.0) Theta = atan(R_ECAL/ZEcal);
      if(Theta<0.0) Theta = Theta+Geom::pi() ;
      
      float ETA = - log(tan(0.5*Theta));
 
      if( fabs(ETA) > etaBarrelEndcap )
      {
         float Zend = Z_Endcap ;
         if(EtaParticle<0.0 )  Zend = -Zend ;
         float Zlen = Zend - Zvertex ;
         float RR = Zlen/sinh(EtaParticle);
         Theta = atan((RR+plane_Radius)/Zend);
         if(Theta<0.0) Theta = Theta+Geom::pi() ;
         ETA = - log(tan(0.5*Theta));
      }
                 
      return ETA;
   } else {
      edm::LogWarning("EcalAna") << "Warning: Eta equals to zero, not correcting" ;
      return EtaParticle;
   }
}
