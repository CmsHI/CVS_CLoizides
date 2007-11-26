// $Id: JetAna.cc,v 1.9 2007/11/25 15:31:01 loizides Exp $

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
#include "DataFormats/JetReco/interface/BasicJetCollection.h"

#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TNtuple.h"

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
      double deltaphi(double phi1, double phi2) const;

      edm::InputTag hmcsrc_;
      edm::InputTag jetsrc_;
      std::string resfilename_;

      int evcounter_;
      const HepMC::GenEvent *hmcevent_;
      const reco::CaloJetCollection *cjets_;
      const reco::BasicJetCollection *bjets_;
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
                            "jet:jphi:jeta:jmat:nmat:amat:"
                            "isnear:dr:pid:pet:pphi:peta:"
                            "trgid:trget:trgphi:trgeta:"
                            "drtrg:drnear:draway:"
                            "dphitrg:dphinear:dphiaway");
   resntuple2_ = new TNtuple("cpartons","cpartons",
                             "pid:pet:pphi:peta:"
                             "trgid:trget:trgphi:trgeta:"
                             "pmat:nmat:amat:isnear:"
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
   if(bjets_==0 && cjets_==0) return;

   //find matches to near and away side
   double jetnet=0,jetneta=0,jetnphi=0;
   int jetncounter=-1;
   double dbnear=1e12;
   double jetaet=0,jetaeta=0,jetaphi=0;
   int jetacounter=-1;
   double dbaway=1e12;

   if(cjets_) {
      int counter=0;
      for(CaloJetCollection::const_iterator jitr = cjets_->begin();
          jitr != cjets_->end(); jitr++) {
         
         const CaloJet jet = *jitr;
         double eta=jet.eta();
         double phi=jet.phi();

         double dnear = deltaR(phi,near->momentum().phi(),eta,near->momentum().eta());
         if(dnear<dbnear) {
            dbnear=dnear;
            jetncounter=counter;
            jetnet=jet.et();
            jetnphi=phi;
            jetneta=eta;
         }
         double daway = deltaR(phi,away->momentum().phi(),eta,away->momentum().eta());
         if(daway<dbaway) {
            dbaway=daway;
            jetacounter=counter;
            jetaet=jet.et();
            jetaphi=phi;
            jetaeta=eta;
         }
         ++counter;
      }
   } else {
      int counter=0;
      for(BasicJetCollection::const_iterator jitr = bjets_->begin();
          jitr != bjets_->end(); jitr++) {
         
         const BasicJet jet = *jitr;
         double eta=jet.eta();
         double phi=jet.phi();

         double dnear = deltaR(phi,near->momentum().phi(),eta,near->momentum().eta());
         if(dnear<dbnear) {
            dbnear=dnear;
            jetncounter=counter;
            jetnet=jet.et();
            jetnphi=phi;
            jetneta=eta;
         }
         double daway = deltaR(phi,away->momentum().phi(),eta,away->momentum().eta());
         if(daway<dbaway) {
            dbaway=daway;
            jetacounter=counter;
            jetaet=jet.et();
            jetaphi=phi;
            jetaeta=eta;
         }
         ++counter;
      }
   }

   if(dbnear>0.3) jetncounter=-1;
   if(dbaway>0.3) jetacounter=-1;

   if(cjets_) {
      int counter=0;
      for(CaloJetCollection::const_iterator jitr = cjets_->begin();
          jitr != cjets_->end(); jitr++) {
         
         Int_t fn = 0;
         float nvals[100];
         memset(nvals,0,100*sizeof(float));

         const CaloJet jet = *jitr;

         nvals[fn++] = jet.et();
         nvals[fn++] = jet.phi();
         nvals[fn++] = jet.eta();
         if(counter==jetncounter) {
            nvals[fn++] = 1; //ismatched
            nvals[fn++] = 1; //near matched
            nvals[fn++] = jetacounter>-1 ? 1:0; 
            nvals[fn++] = 1; //isnear
            nvals[fn++] = dbnear;
            nvals[fn++] = near->pdg_id();
            nvals[fn++] = near->momentum().perp();         
            nvals[fn++] = near->momentum().phi();
            nvals[fn++] = near->momentum().eta();
            nvals[fn++] = trpa->pdg_id();
            nvals[fn++] = trpa->momentum().perp();         
            nvals[fn++] = trpa->momentum().phi();
            nvals[fn++] = trpa->momentum().eta();
         } else if (counter==jetacounter) {
            nvals[fn++] = 1; //ismatched
            nvals[fn++] = jetncounter>-1 ? 1:0; 
            nvals[fn++] = 1; //away matched
            nvals[fn++] = 0; //isaway
            nvals[fn++] = dbaway;
            nvals[fn++] = away->pdg_id();
            nvals[fn++] = away->momentum().perp();         
            nvals[fn++] = away->momentum().phi();
            nvals[fn++] = away->momentum().eta();
            nvals[fn++] = trpa->pdg_id();
            nvals[fn++] = trpa->momentum().perp();         
            nvals[fn++] = trpa->momentum().phi();
            nvals[fn++] = trpa->momentum().eta();
         } else {
            nvals[fn++] = 0; 
            nvals[fn++] = jetncounter>-1 ? 1:0; 
            nvals[fn++] = jetacounter>-1 ? 1:0; 
            nvals[fn++] = 2; 
            nvals[fn++] = 1e12;
            nvals[fn++] = near->pdg_id();
            nvals[fn++] = near->momentum().perp();         
            nvals[fn++] = near->momentum().phi();
            nvals[fn++] = near->momentum().eta();
            nvals[fn++] = away->pdg_id();
            nvals[fn++] = away->momentum().perp();         
            nvals[fn++] = away->momentum().phi();
            nvals[fn++] = away->momentum().eta();
         }
         nvals[fn++]=deltaR(jet.phi(),trpa->momentum().phi(),
                            jet.eta(),trpa->momentum().eta());
         nvals[fn++]=deltaR(jet.phi(),near->momentum().phi(),
                            jet.eta(),near->momentum().eta());
         nvals[fn++]=deltaR(jet.phi(),away->momentum().phi(),
                            jet.eta(),away->momentum().eta());
         nvals[fn++]=deltaphi(jet.phi(),trpa->momentum().phi());
         nvals[fn++]=deltaphi(jet.phi(),near->momentum().phi());
         nvals[fn++]=deltaphi(jet.phi(),away->momentum().phi());

         resntuple_->Fill(nvals);
         ++counter;
      }
   } else {
      int counter=0;
      for(BasicJetCollection::const_iterator jitr = bjets_->begin();
          jitr != bjets_->end(); jitr++) {
         
         Int_t fn = 0;
         float nvals[100];
         memset(nvals,0,100*sizeof(float));

         const BasicJet jet = *jitr;

         nvals[fn++] = jet.et();
         nvals[fn++] = jet.phi();
         nvals[fn++] = jet.eta();
         if(counter==jetncounter) {
            nvals[fn++] = 1; //ismatched
            nvals[fn++] = 1; //near matched
            nvals[fn++] = jetacounter>-1 ? 1:0; 
            nvals[fn++] = 1; //isnear
            nvals[fn++] = dbnear;
            nvals[fn++] = near->pdg_id();
            nvals[fn++] = near->momentum().perp();         
            nvals[fn++] = near->momentum().phi();
            nvals[fn++] = near->momentum().eta();
            nvals[fn++] = trpa->pdg_id();
            nvals[fn++] = trpa->momentum().perp();         
            nvals[fn++] = trpa->momentum().phi();
            nvals[fn++] = trpa->momentum().eta();
         } else if (counter==jetacounter) {
            nvals[fn++] = 1; //ismatched
            nvals[fn++] = jetncounter>-1 ? 1:0; 
            nvals[fn++] = 1; //away matched
            nvals[fn++] = 0; //isaway
            nvals[fn++] = dbaway;
            nvals[fn++] = away->pdg_id();
            nvals[fn++] = away->momentum().perp();         
            nvals[fn++] = away->momentum().phi();
            nvals[fn++] = away->momentum().eta();
            nvals[fn++] = trpa->pdg_id();
            nvals[fn++] = trpa->momentum().perp();         
            nvals[fn++] = trpa->momentum().phi();
            nvals[fn++] = trpa->momentum().eta();
         } else {
            nvals[fn++] = 0; 
            nvals[fn++] = jetncounter>-1 ? 1:0; 
            nvals[fn++] = jetacounter>-1 ? 1:0; 
            nvals[fn++] = 2; 
            nvals[fn++] = 1e12;
            nvals[fn++] = near->pdg_id();
            nvals[fn++] = near->momentum().perp();         
            nvals[fn++] = near->momentum().phi();
            nvals[fn++] = near->momentum().eta();
            nvals[fn++] = away->pdg_id();
            nvals[fn++] = away->momentum().perp();         
            nvals[fn++] = away->momentum().phi();
            nvals[fn++] = away->momentum().eta();
         }
         nvals[fn++]=deltaR(jet.phi(),trpa->momentum().phi(),
                            jet.eta(),trpa->momentum().eta());
         nvals[fn++]=deltaR(jet.phi(),near->momentum().phi(),
                            jet.eta(),near->momentum().eta());
         nvals[fn++]=deltaR(jet.phi(),away->momentum().phi(),
                            jet.eta(),away->momentum().eta());
         nvals[fn++]=deltaphi(jet.phi(),trpa->momentum().phi());
         nvals[fn++]=deltaphi(jet.phi(),near->momentum().phi());
         nvals[fn++]=deltaphi(jet.phi(),away->momentum().phi());

         resntuple_->Fill(nvals);
         ++counter;
      }
   }

   if(1) {
      int nfound=jetncounter>-1 ? 1:0;
      int afound=jetacounter>-1 ? 1:0;

      float nvals[100];
      Int_t fn = 0;
      memset(nvals,0,100*sizeof(float));
      nvals[fn++]=near->pdg_id();
      nvals[fn++]=near->momentum().perp();
      nvals[fn++]=near->momentum().phi();
      nvals[fn++]=near->momentum().eta();
      nvals[fn++]=trpa->pdg_id();
      nvals[fn++]=trpa->momentum().perp();
      nvals[fn++]=trpa->momentum().phi();
      nvals[fn++]=trpa->momentum().eta();
      if(jetncounter>=0) {
         nvals[fn++]=1;
         nvals[fn++]=1;
         nvals[fn++]=afound;
         nvals[fn++]=1;
         nvals[fn++]=dbnear;
         nvals[fn++]=jetnet;
         nvals[fn++]=jetnphi;
         nvals[fn++]=jetneta;
      } else {
         nvals[fn++]=0;
         nvals[fn++]=0;
         nvals[fn++]=afound;
         nvals[fn++]=1;
         nvals[fn++]=1e12;
         nvals[fn++]=1e12;
         nvals[fn++]=1e12;
         nvals[fn++]=1e12;
      }
      resntuple2_->Fill(nvals);

      fn = 0;
      memset(nvals,0,100*sizeof(float));
      nvals[fn++]=away->pdg_id();
      nvals[fn++]=away->momentum().perp();
      nvals[fn++]=away->momentum().phi();
      nvals[fn++]=away->momentum().eta();
      nvals[fn++]=trpa->pdg_id();
      nvals[fn++]=trpa->momentum().perp();
      nvals[fn++]=trpa->momentum().phi();
      nvals[fn++]=trpa->momentum().eta();
      if(jetacounter>=0) {
         nvals[fn++]=1;
         nvals[fn++]=nfound;
         nvals[fn++]=1;
         nvals[fn++]=0;
         nvals[fn++]=dbaway;
         nvals[fn++]=jetaet;
         nvals[fn++]=jetaphi;
         nvals[fn++]=jetaeta;
      } else {
         nvals[fn++]=0;
         nvals[fn++]=nfound;
         nvals[fn++]=0;
         nvals[fn++]=0;
         nvals[fn++]=1e12;
         nvals[fn++]=1e12;
         nvals[fn++]=1e12;
         nvals[fn++]=1e12;
      }
      resntuple2_->Fill(nvals);
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

   bjets_=0;
   cjets_=0;

   Handle<reco::CaloJetCollection> pcJets;
   try {
      iEvent.getByLabel(jetsrc_, pcJets);
      cjets_ = pcJets.product();
      edm::LogInfo("JetAna") << "total # CaloJets: " << cjets_->size();
   } catch (...) {
      Handle<reco::BasicJetCollection> pbJets;
      try {
         iEvent.getByLabel(jetsrc_, pbJets);
         bjets_ = pbJets.product();
         edm::LogInfo("JetAna") << "total # BasicJets: " << bjets_->size();
      } catch (...) {
         edm::LogError("JetAna") << "Error! can't get the product " << jetsrc_.encode();
      }
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

double JetAna::deltaphi(double phi1, double phi2) const {
   double diff1 = phi1-phi2;
   while (diff1 >= TMath::Pi()) diff1 -= 2.*TMath::Pi();
   while (diff1 < -TMath::Pi()) diff1 += 2.*TMath::Pi();
   return TMath::Abs(diff1);
}

DEFINE_FWK_MODULE( JetAna );
#endif
