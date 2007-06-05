// $Id: EcalAna.h,v 1.1 2007/05/28 10:31:20 loizides Exp $

#ifndef EcalAna_EcalAna_h
#define EcalAna_EcalAna_h

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "HepMC/GenEvent.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "SimDataFormats/CaloHit/interface/PCaloHitContainer.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/EgammaReco/interface/ClusterShapeFwd.h"

class TH2;

class EcalAna : public edm::EDAnalyzer {
   public:
      explicit EcalAna(const edm::ParameterSet&);
      ~EcalAna();

   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      typedef std::map<unsigned int, const SimTrack*> SimTrackMap;
      SimTrackMap *createSimTrackMap();

      typedef std::map<const SimTrack*, unsigned int> STUsedMap;
      STUsedMap *createSTUsedMap();
      void resetSTUsedMap(EcalAna::STUsedMap *map);

      typedef std::multimap<unsigned int, const PCaloHit*> CaloHitMMap;
      CaloHitMMap *createHitMMap();

      void fillH2(TH2 *h, const DetId &id, double weight);
      void fillEt(TH2 *h, const DetId &id, double energy);
      void fillTrackEt(TH2 *h, unsigned int rawid);
      void getHepMC(const edm::Event&, const edm::EventSetup&);
      void getSimTracks(const edm::Event&, const edm::EventSetup&);
      void getSimVertices(const edm::Event&, const edm::EventSetup&);
      void getEBCaloHits(const edm::Event& iEvent, const edm::EventSetup& iSetup);
      void getEECaloHits(const edm::Event& iEvent, const edm::EventSetup& iSetup);
      void getEBDigis(const edm::Event& iEvent, const edm::EventSetup& iSetup);
      void getEEDigis(const edm::Event& iEvent, const edm::EventSetup& iSetup);
      void getEBRecHits(const edm::Event& iEvent, const edm::EventSetup& iSetup);
      void getEERecHits(const edm::Event& iEvent, const edm::EventSetup& iSetup);
      void getEBIslandClusters(const edm::Event& iEvent, const edm::EventSetup& iSetup);
      void getEEIslandClusters(const edm::Event& iEvent, const edm::EventSetup& iSetup);
      void getEBClusterShapes(const edm::Event& iEvent, const edm::EventSetup& iSetup);
      void getEEClusterShapes(const edm::Event& iEvent, const edm::EventSetup& iSetup);
      void getEBSuperClusters(const edm::Event& iEvent, const edm::EventSetup& iSetup);
      void getEESuperClusters(const edm::Event& iEvent, const edm::EventSetup& iSetup);
      //template <class T> *T getData(const edm::Event&, const edm::EventSetup&);
      float ecalEta(float EtaParticle, float Zvertex, float plane_Radius);

      // ----------member data ---------------------------
      edm::InputTag ebdsrc_;
      edm::InputTag eedsrc_;
      edm::InputTag ebhsrc_;
      edm::InputTag eehsrc_;
      edm::InputTag ebrsrc_;
      edm::InputTag eersrc_;
      edm::InputTag ebcsrc_;
      edm::InputTag eecsrc_;
      edm::InputTag ebssrc_;
      edm::InputTag eessrc_;
      edm::InputTag ebscsrc_;
      edm::InputTag eescsrc_;
      edm::InputTag hmcsrc_;
      edm::InputTag simsrc_;
      double minet_;
      double minetpart_;
      int verbose_;
      int evcounter_;
      float zvertex_;
      float rvertex_;
      const CaloGeometry *cgeo_;
      const HepMC::GenEvent *hmcevent_;
      const edm::SimTrackContainer *Particles_;
      const edm::SimVertexContainer *Vertices_;
      const edm::PCaloHitContainer *EBhits_;
      const edm::PCaloHitContainer *EEhits_;
      const EBDigiCollection *EBdigis_;
      const EEDigiCollection *EEdigis_;
      const EBRecHitCollection *EBrechits_;
      const EBRecHitCollection *EErechits_;
      const reco::BasicClusterCollection *EBclusters_;
      const reco::BasicClusterCollection *EEclusters_;
      const reco::ClusterShapeCollection *EBshapes_;
      const reco::ClusterShapeCollection *EEshapes_;
      const reco::SuperClusterCollection *EBsclusters_;
      const reco::SuperClusterCollection *EEsclusters_;
      SimTrackMap *simtrackmap_;
      STUsedMap *stusedmap_;
      CaloHitMMap *hitmap_;
};
#endif
