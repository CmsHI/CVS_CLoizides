// $Id: JetAna.cc,v 1.2 2007/11/19 17:25:50 loizides Exp $

#ifndef JetAna_JetAna_h
#define JetAna_JetAna_h

// system include files
#include <memory>
#include <vector>
#include <map>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h" 
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"

#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TNtuple.h"

#include "CLoizides/Utils/interface/THistFileService.h"

class TH2;

class JetAna : public edm::EDAnalyzer {
   public:
      explicit JetAna(const edm::ParameterSet&);
      ~JetAna();

   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      void getHepMC(const edm::Event&, const edm::EventSetup&);
      void getJets(const edm::Event&, const edm::EventSetup&);

      double  deltaR(double phi1, double phi2, double eta1, double eta2) const;
      double deltaR2(double phi1, double phi2, double eta1, double eta2) const;

      edm::InputTag hmcsrc_;
      edm::InputTag jetsrc_;
      std::string resfilename_;

      int evcounter_;
      const HepMC::GenEvent *hmcevent_;
      const reco::CaloJetCollection *jets_;
      TFile *resfile_;
      TNtuple *resntuple_;
      TNtuple *resntuple2_;
};

JetAna::JetAna(const edm::ParameterSet& iConfig) :
   hmcsrc_(iConfig.getParameter<edm::InputTag>("hmcsrc")),
   jetsrc_(iConfig.getParameter<edm::InputTag>("jetsrc")),
   resfilename_(iConfig.getUntrackedParameter<std::string>("filename","ntupout.root")),
   evcounter_(0)
{
   // now do what ever initialization is needed

   resntuple_ = new TNtuple("cjets","cjets",
                            "jet:jphi:jeta:jmat:"
                            "dr:pid:pet:pphi:peta:istrg:"
                            "drtrg:drp1:drp2");
   resntuple2_ = new TNtuple("cpartons","cpartons",
                             "pid:pet:pphi:peta:pmat:istrg:"
                             "dr:jet:jphi:jeta");
   resntuple_->SetDirectory(0);
   resntuple2_->SetDirectory(0);
}

JetAna::~JetAna()
{
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}

// ------------ method called to for each event  ------------
void
JetAna::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace reco;
   using namespace HepMC;

   // get hepmc event
   getHepMC(iEvent,iSetup);

   const GenParticle *trpa = 0;
   const GenParticle *away = 0;
   const GenParticle *near = 0;

   if(hmcevent_) {
      double ptmax=0;
      for (GenEvent::particle_const_iterator pitr=hmcevent_->particles_begin(); 
           pitr!=hmcevent_->particles_end(); pitr++) {

         const HepMC::GenParticle *part = *pitr; 
         if(part->status()==3) continue;
         int id = TMath::Abs(part->pdg_id());
         if((id!=221) && 
            (id!=331) && 
            (id!=223) && 
            (id!=111) && 
            (id!=211) &&
            (id!=22)) continue;

         if(part->momentum().perp()>ptmax) {
            trpa  = part;
            ptmax = part->momentum().perp();
         }
      }
      if(trpa==0) {
         edm::LogInfo("JetAna") << "Did not find trigger particle. Omitting event!";
         return;
      }

      const GenParticle *par1 = 0;
      const GenParticle *par2 = 0;

      Int_t pcounter=0;
      for (GenEvent::particle_const_iterator pitr=hmcevent_->particles_begin(); 
           pitr!=hmcevent_->particles_end(); pitr++) {

         const HepMC::GenParticle *part = *pitr; 
         pcounter++;
         if(pcounter==7) 
            par1 = part;
         else if(pcounter==8) 
            par2 = part;
         else if (pcounter>10) 
            break;
      }
      double dp1 = deltaR(trpa->momentum().phi(),par1->momentum().phi(),
                          trpa->momentum().eta(),par1->momentum().eta());
      double dp2 = deltaR(trpa->momentum().phi(),par2->momentum().phi(),
                          trpa->momentum().eta(),par2->momentum().eta());
      if(dp1<dp2) {
         near=par1;
         away=par2;
      } else {
         near=par2;
         away=par1;
      }
   }

   // get calo jets
   getJets(iEvent,iSetup);

   if(jets_) {

      //find matches to near and away side
      const CaloJet *jetn = 0;
      int jetncounter=-1;
      double dbnear=1e12;
      const CaloJet *jeta = 0;
      int jetacounter=-1;
      double dbaway=1e12;

      int counter=0;
      for(CaloJetCollection::const_iterator jitr = jets_->begin ();
          jitr != jets_->end (); jitr++) {
         
         const CaloJet jet = *jitr;
         double eta=jet.eta();
         double phi=jet.phi();

         double dtrig = deltaR(phi,trpa->momentum().phi(),eta,trpa->momentum().eta());
         if(dtrig<dbnear) {
            dbnear=dtrig;
            jetncounter=counter;
            jetn=&jet;
         }
         double daway = deltaR(phi,away->momentum().phi(),eta,away->momentum().eta());
         if(daway<dbaway) {
            dbaway=daway;
            jetacounter=counter;
            jeta=&jet;
         }

         ++counter;
      }

      if(dbnear>0.5) jetncounter=-1;
      if(dbaway>0.5) jetacounter=-1;

      counter=0;
      for(CaloJetCollection::const_iterator jitr = jets_->begin ();
          jitr != jets_->end (); jitr++) {
         
         Int_t fn = 0;
         float nvals[100];
         memset(nvals,0,100*sizeof(float));

         const CaloJet jet = *jitr;

         nvals[fn++] = jet.et();
         nvals[fn++] = jet.phi();
         nvals[fn++] = jet.eta();
         if(counter==jetncounter) {
            nvals[fn++] = 1;
            nvals[fn++] = dbnear;
            nvals[fn++] = trpa->pdg_id();
            nvals[fn++] = trpa->momentum().perp();         
            nvals[fn++] = trpa->momentum().phi();
            nvals[fn++] = trpa->momentum().eta();
            nvals[fn++] = 1;
         } else if (counter==jetacounter) {
            nvals[fn++] = 1;
            nvals[fn++] = dbaway;
            nvals[fn++] = away->pdg_id();
            nvals[fn++] = away->momentum().perp();         
            nvals[fn++] = away->momentum().phi();
            nvals[fn++] = away->momentum().eta();
            nvals[fn++] = 0;
         } else {
            fn+=7;
         }
         nvals[fn++]=deltaR(jet.phi(),trpa->momentum().phi(),
                            jet.eta(),trpa->momentum().eta());
         nvals[fn++]=deltaR(near->momentum().phi(),trpa->momentum().phi(),
                            near->momentum().eta(),trpa->momentum().eta());
         nvals[fn++]=deltaR(away->momentum().phi(),trpa->momentum().phi(),
                            away->momentum().eta(),trpa->momentum().eta());

         resntuple_->Fill(nvals);
         ++counter;
      }

      if(1) {
         if(jetncounter>=0)
            resntuple2_->Fill(trpa->pdg_id(), 
                              trpa->momentum().perp(),trpa->momentum().phi(),trpa->momentum().eta(),
                              1,1,dbnear,jetn->et(),jetn->phi(),jetn->eta());
         else 
            resntuple2_->Fill(trpa->pdg_id(), 
                              trpa->momentum().perp(),trpa->momentum().phi(),trpa->momentum().eta(),
                              0,1,1e12,1e12,1e12,1e12);
         if(jetacounter>=0)
            resntuple2_->Fill(away->pdg_id(), 
                              away->momentum().perp(),away->momentum().phi(),away->momentum().eta(),
                              1,0,dbaway,jeta->et(),jeta->phi(),jeta->eta());
         else 
            resntuple2_->Fill(away->pdg_id(), 
                              away->momentum().perp(),away->momentum().phi(),away->momentum().eta(),
                              0,0,1e12,1e12,1e12,1e12);
      }
   }

   // count events
   evcounter_++;
}

// ------------ method called once each job just before starting event loop  ------------
void 
JetAna::beginJob(const edm::EventSetup &iSetup) {
}

// ------------ method called once each job just after ending the event loop  ------------
void 
JetAna::endJob() {
   if(resntuple_) {
      TDirectory::TContext context(0);
      resfile_  = TFile::Open(resfilename_.c_str(),"recreate","",6);
      resntuple_->Write();
      resntuple2_->Write();
      resfile_->Close();
      delete resfile_;
      resntuple_=0;
      resfile_=0;
   }
}

// ------------ setup methods ------------
void 
JetAna::getHepMC(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

   using namespace edm;

   Handle<HepMCProduct> pHepMC;
   try {
      iEvent.getByLabel(hmcsrc_, pHepMC);
      hmcevent_ = pHepMC->GetEvent();
      edm::LogInfo("JetAna") << "total # HepMCTracks: " << hmcevent_->particles_size() 
                              << ", HepMCVertices: "     <<  hmcevent_->vertices_size();
   } catch (...) {
      edm::LogError("JetAna") << "Error! can't get the product " << hmcsrc_.encode();
   }
}

void 
JetAna::getJets(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

   using namespace edm;
   using namespace reco;

   Handle<reco::CaloJetCollection> pJets;
   try {
      iEvent.getByLabel(jetsrc_, pJets);
      jets_ = pJets.product();
      edm::LogInfo("JetAna") << "total # Jets: " << jets_->size();
   } catch (...) {
      edm::LogError("JetAna") << "Error! can't get the product " << jetsrc_.encode();
   }
}

// ------------ help methods ------------
double JetAna::deltaR2(double phi1, double phi2, double eta1, double eta2) const {

   double diff1 = phi1-phi2;
   while (diff1 >= TMath::Pi()) diff1 -= 2.*TMath::Pi();
   while (diff1 < -TMath::Pi()) diff1 += 2.*TMath::Pi();
   diff1*=diff1;

   double diff2 = eta1-eta2;
   diff2*=diff2;
   return diff1+diff2;
}

double JetAna::deltaR(double phi1, double phi2, double eta1, double eta2) const {
   return TMath::Sqrt(deltaR2(phi1,phi2,eta1,eta2));
}

DEFINE_FWK_MODULE( JetAna );
#endif
