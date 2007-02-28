// $Id: SimInvMassAna.h,v 1.1 2007/02/25 23:38:52 loizides Exp $

#ifndef InvMassAna_SimInvMassAna_h
#define InvMassAna_SimInvMassAna_h

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "TH1D.h"
#include "TH2D.h"

class SimInvMassAna : public edm::EDAnalyzer {
   public:
      explicit SimInvMassAna(const edm::ParameterSet&);
      ~SimInvMassAna();

   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob();

      // ----------member data ---------------------------
      edm::InputTag src_;
      int pdgId1_;
      int pdgId2_;
      double ptmin_;
      double etamax_;
      TH1D *m_InvMass;
      TH2D *m_PtInvMass;
      TH2D *m_EtaInvMass;
      TH1D *m_Pt1;
      TH1D *m_Pt2;
      TH1D *m_Eta1;
      TH1D *m_Eta2;
      TH1D *m_Entries1;
      TH1D *m_Entries2;
};

#endif
