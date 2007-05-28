// $Id: EcalAna.h,v 1.2 2007/03/01 04:15:51 loizides Exp $

#ifndef EcalAna_EcalAna_h
#define EcalAna_EcalAna_h

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class EcalAna : public edm::EDAnalyzer {
   public:
      explicit EcalAna(const edm::ParameterSet&);
      ~EcalAna();

   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      // ----------member data ---------------------------
      edm::InputTag ebdsrc_;
      edm::InputTag eedsrc_;
      edm::InputTag ebhsrc_;
      edm::InputTag eehsrc_;
      edm::InputTag simsrc_;
      edm::InputTag hmcsrc_;
      int verbose_;
      int evcounter_;
};
#endif
