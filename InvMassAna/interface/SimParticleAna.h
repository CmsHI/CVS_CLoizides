// $Id: SimParticleAna.h,v 1.1 2007/02/25 23:38:52 loizides Exp $

#ifndef InvMassAna_SimParticleAna_h
#define InvMassAna_SimParticleAna_h

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "TH1D.h"
#include "TH2D.h"

class SimParticleAna : public edm::EDAnalyzer {
   public:
      explicit SimParticleAna(const edm::ParameterSet&);
      ~SimParticleAna();

   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob();

      // ----------member data ---------------------------
      edm::InputTag src_;
      int pdgId_;
      TH1D *m_Pt;
      TH1D *m_Eta;
      TH1D *m_Entries;
};

#endif
