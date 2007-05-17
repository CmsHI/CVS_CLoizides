// $Id: MCChainAna.h,v 1.1 2007/02/25 23:38:52 loizides Exp $

#ifndef InvMassAna_MCChainAna_h
#define InvMassAna_MCChainAna_h

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class MCChainAna : public edm::EDAnalyzer {
   public:
      explicit MCChainAna(const edm::ParameterSet&);
      ~MCChainAna();

   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob();

      // ----------member data ---------------------------
      edm::InputTag src_;
};

#endif
