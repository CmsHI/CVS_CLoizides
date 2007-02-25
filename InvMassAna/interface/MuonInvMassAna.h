// $Id: MuonInvMassAna.cc,v 1.2 2007/02/22 19:21:00 loizides Exp $

#ifndef InvMassAna_MuonInvMassAna_h
#define InvMassAna_MuonInvMassAna_h

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "TH1D.h"

class MuonInvMassAna : public edm::EDAnalyzer {
   public:
      explicit MuonInvMassAna(const edm::ParameterSet&);
      ~MuonInvMassAna();

   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      // ----------member data ---------------------------
      edm::InputTag src_;
      TH1D *m_InvMass;
};

#endif
