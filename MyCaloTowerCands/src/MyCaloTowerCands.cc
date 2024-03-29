// makes CaloTowerCandidates from CaloTowers
// original author: L.Lista INFN
// modifyed by: F.Ratnikov UMd
// $Id: MyCaloTowerCands.cc,v 1.2 2007/11/27 19:57:22 loizides Exp $
#include <cmath>
#include "DataFormats/RecoCandidate/interface/RecoCaloTowerCandidate.h"
#include "DataFormats/CaloTowers/interface/CaloTower.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "CLoizides/MyCaloTowerCands/interface/MyCaloTowerCands.h"
#include <TH2F.h>
#include <TString.h>
#include <TFile.h>
#include <TMath.h>

using namespace edm;
using namespace reco;
using namespace std;

MyCaloTowerCands::MyCaloTowerCands( const ParameterSet & p ) :
  mVerbose (p.getUntrackedParameter<int> ("verbose", 0)),
  mSource (p.getParameter<edm::InputTag> ("src")),
  mEtThreshold (p.getParameter<double> ("minimumEt")),
  mEThreshold (p.getParameter<double> ("minimumE")),
  maxevents (p.getUntrackedParameter<int> ("maxevents", 0)),
  usearea (p.getParameter<bool> ("usearea")),
  eventcounter(0)
{
  produces<CandidateCollection>();
  std::cout << "*************** MyCaloTowerCands ************** " << std::endl;
}

MyCaloTowerCands::~MyCaloTowerCands() {
}

double MyCaloTowerCands::phisize(double eta) {
   double ev=TMath::Abs(eta);
   const double ret=5./180*TMath::Pi();
   if(ev<=1.740)
      return ret;
   else if(ev<=4.716)
      return 2*ret;
   return 4*ret;
}

double MyCaloTowerCands::etasize(double eta) {
   double ev=TMath::Abs(eta);
   if(ev<=1.740)
      return 0.087;
   else if(ev<=0.183)
      return 0.090;
   else if(ev<=0.193)
      return 0.010;
   else if(ev<=2.043)
      return 0.113;
   else if(ev<=2.172)
      return 0.129;
   else if(ev<=2.322)
      return 0.150;
   else if(ev<=2.5)
      return 0.178;
   else if(ev<=2.650)
      return 0.150;
   else if(ev<=2.868)
      return 0.218;
   else if(ev<=3)
      return 0.132;
   return 0.175; //not completely correct;
}

void MyCaloTowerCands::produce( Event& evt, const EventSetup& ) {
  Handle<CaloTowerCollection> caloTowers;
  evt.getByLabel( mSource, caloTowers );

  const double basesize=phisize(0)*etasize(0);
  const int netabins=(int)(7/0.025);
  const int nphibins=(int)(7/0.025);
  TH2F *htest=new TH2F(Form("hTowers-%d",++eventcounter),";#phi;#eta",nphibins,-3.5,3.5,netabins,-3.5,3.5);
  htest->SetDirectory(0);
  
  auto_ptr<CandidateCollection> cands( new CandidateCollection );
  cands->reserve( caloTowers->size() );
  unsigned idx = 0;
  for (; idx < caloTowers->size (); idx++) {
    const CaloTower* cal = &((*caloTowers) [idx]);
    if (mVerbose >= 2) {
      std::cout << "MyCaloTowerCands::produce-> " << idx << " tower et/eta/phi/e: " 
		<< cal->et() << '/' << cal->eta() << '/' << cal->phi() << '/' << cal->energy() << " is...";
    }

    double factor=1;
    if(usearea) 
       factor=phisize(cal->eta())*etasize(cal->eta())/basesize;
    if (cal->et() >= mEtThreshold*factor && cal->energy() >= mEThreshold*factor ) {
      math::PtEtaPhiELorentzVector p( cal->et(), cal->eta(), cal->phi(), cal->energy() );
      htest->Fill(cal->phi(),cal->eta(),cal->et());

      RecoCaloTowerCandidate * c = 
	new RecoCaloTowerCandidate( 0, Candidate::LorentzVector( p ) );
      c->setCaloTower (CaloTowerRef( caloTowers, idx) );
      cands->push_back( c );
      if (mVerbose >= 2) std::cout << "accepted: pT/eta/phi:" << c->pt() << '/' << c->eta() <<  '/' << c->phi() <<std::endl;
    }
    else {
      if (mVerbose >= 2) std::cout << "rejected" << std::endl;
    }
  }
  if (mVerbose >= 1) {
    std::cout << "MyCaloTowerCands::produce-> " << cands->size () << " candidates created" << std::endl;
  }
  evt.put( cands );

  if(eventcounter<maxevents) {
     TFile f("towershist.root","update");
     TDirectory::TContext context(&f);
     htest->Write();
     f.Close();
  }
  delete htest;
}
