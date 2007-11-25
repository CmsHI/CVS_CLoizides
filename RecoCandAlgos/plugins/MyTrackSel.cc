#ifndef RecoAlgos_MyTrackSel_h
#define RecoAlgos_MyTrackSel_h

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Math/interface/Vector3D.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/TrackExtra.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateFwd.h"
#include <TMath.h>

class MyTrackSel : public edm::EDProducer {
public:
  MyTrackSel( const edm::ParameterSet & cfg ) : 
     src_(cfg.getParameter<edm::InputTag>("src")),
     ptmin_(cfg.getUntrackedParameter<double>("ptmin",1.)),
     ptmax_(cfg.getUntrackedParameter<double>("ptmax",999.)),
     hitsmin_(cfg.getUntrackedParameter<int>("hitsmin",8)),
     chi2min_(cfg.getUntrackedParameter<double>("chi2min",0.01)),
     dcacut_(cfg.getUntrackedParameter<double>("dcacut",3)),
     zvtx_(cfg.getUntrackedParameter<double>("zvtx",0)),
     verbose_(cfg.getUntrackedParameter<bool>("verbose",0))
      {
         produces<reco::CandidateCollection>();
      }
  ~MyTrackSel() { }
  
private:
  void produce( edm::Event &, const edm::EventSetup & );
  edm::InputTag src_;
  double ptmin_;
  double ptmax_;
  double hitsmin_;
  double chi2min_;
  double dcacut_;
  double zvtx_;
  bool verbose_;
};

void MyTrackSel::produce( edm::Event & evt, const edm::EventSetup & ) {
  using namespace edm;
  using namespace reco;
  using namespace std;

  Handle<TrackCollection> ts;
  evt.getByLabel(src_, ts);

  auto_ptr<CandidateCollection> cands( new CandidateCollection );
  size_t size = ts->size();
  cands->reserve(size);

  const math::XYZPoint bs(0,0,zvtx_);
  for( size_t idx = 0; idx != size; ++ idx ) {

     const Track &trk = (*ts)[idx];
     if(trk.pt()<ptmin_) continue;
     if(trk.pt()>ptmax_) continue;
     if(trk.chi2()<chi2min_) continue;
     if(trk.hitPattern().numberOfValidHits()<hitsmin_) continue;
     double_t dca=TMath::Abs(-trk.dxy(bs)/trk.error(3));
     if(dca>dcacut_) continue;

     RecoChargedCandidate *c = new RecoChargedCandidate;
     c->setCharge(trk.charge());
     c->setVertex(trk.vertex());
     Track::Vector p = trk.momentum();
     double massSqr=0.139*0.139;
     double t = sqrt(massSqr + p.mag2());
     c->setP4(Candidate::LorentzVector(p.x(), p.y(), p.z(), t));
     //TrackRef trkref;
     //c->setTrack(trkref); //probles with TrackRef
     cands->push_back(c);
  }

  evt.put(cands);
}

#endif

DEFINE_FWK_MODULE(MyTrackSel);
