// $Id: JetAna.h,v 1.2 2007/06/05 15:20:41 loizides Exp $

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
};

JetAna::JetAna(const edm::ParameterSet& iConfig) :
   hmcsrc_(iConfig.getParameter<edm::InputTag>("hmcsrc")),
   jetsrc_(iConfig.getParameter<edm::InputTag>("jetsrc")),
   resfilename_(iConfig.getUntrackedParameter<std::string>("filename","ntupout.root")),
   evcounter_(0)
{
   // now do what ever initialization is needed

   resfile_  = TFile::Open(resfilename_.c_str(),"recreate");
   resfile_->cd();
   resntuple_ = new TNtuple("cjets","cjets",
                            "jet:jphi:jeta:"
                            "dtrg:trgid:trget:trgphi:trgeta:"
                            "dpa:paid:paet:paphi:paeta:"
                            "dpb:pbid:pbet:pbphi:pbeta");
   resntuple_->SetAutoSave();
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
   const GenParticle *par1 = 0;
   const GenParticle *par2 = 0;

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
   }

   // get calo jets
   getJets(iEvent,iSetup);

   if(jets_) {

      float nvals[100];
      for(int i = 0; i < 100; i++) nvals[i] = 0;

      for(CaloJetCollection::const_iterator jitr = jets_->begin ();
          jitr != jets_->end (); jitr++) {
         
         Int_t fn = 0;

         const CaloJet jet = *jitr;
        
         double eta=jet.eta();
         double phi=jet.phi();

         double diftrg = 1e12;
         if(trpa) 
            diftrg = deltaR(phi,trpa->momentum().phi(),eta,trpa->momentum().eta());

         double difp1 = 1e12;
         if(par1)
            difp1 = deltaR(phi,par1->momentum().phi(),eta,par1->momentum().eta());
               
         double difp2 = 1e12;
         if(par2)
            difp2 = deltaR(phi,par2->momentum().phi(),eta,par2->momentum().eta());

         nvals[fn++] = jet.et();
         nvals[fn++] = jet.phi();
         nvals[fn++] = jet.eta();
         nvals[fn++] = diftrg;
         if(trpa) {
            nvals[fn++] = trpa->pdg_id();
            nvals[fn++] = trpa->momentum().perp();         
            nvals[fn++] = trpa->momentum().phi();
            nvals[fn++] = trpa->momentum().eta();
         } else {
            nvals[fn++] = 0;
            nvals[fn++] = 0;
            nvals[fn++] = 0;
            nvals[fn++] = 0;
         }
         nvals[fn++] = difp1;
         if(par1) {
            nvals[fn++] = par1->pdg_id();
            nvals[fn++] = par1->momentum().perp();         
            nvals[fn++] = par1->momentum().phi();
            nvals[fn++] = par1->momentum().eta();
         } else {
            nvals[fn++] = 0;
            nvals[fn++] = 0;
            nvals[fn++] = 0;
            nvals[fn++] = 0;
         }
         nvals[fn++] = difp2;
         if(par2) {
            nvals[fn++] = par2->pdg_id();
            nvals[fn++] = par2->momentum().perp();         
            nvals[fn++] = par2->momentum().phi();
            nvals[fn++] = par2->momentum().eta();
         } else {
            nvals[fn++] = 0;
            nvals[fn++] = 0;
            nvals[fn++] = 0;
            nvals[fn++] = 0;
         }

         resntuple_->Fill(nvals);
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
      resntuple_->Write();
      resfile_->Close();
      delete resfile_;
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
