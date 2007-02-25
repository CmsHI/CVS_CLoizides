// $Id: MCInvMassAna.cc,v 1.2 2007/02/22 19:21:00 loizides Exp $

#ifndef InvMassAna_MCParticleAna_h
#define InvMassAna_MCParticleAna_h

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "TH1D.h"
#include "TH2D.h"

class MCParticleAna : public edm::EDAnalyzer {
   public:
      explicit MCParticleAna(const edm::ParameterSet&);
      ~MCParticleAna();

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
