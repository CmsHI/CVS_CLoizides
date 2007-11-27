#ifndef CaloTowersCreator_MyCaloTowerCands_h
#define CaloTowersCreator_MyCaloTowerCands_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <string>

class MyCaloTowerCands : public edm::EDProducer {
 public:
  /// constructor from parameter set
  MyCaloTowerCands( const edm::ParameterSet & );
  /// destructor
  ~MyCaloTowerCands();

 private:
  /// process one event
  void produce( edm::Event& e, const edm::EventSetup& );
  /// verbosity
  int mVerbose;
  /// label of source collection
  edm::InputTag mSource;
  /// ET threshold
  double mEtThreshold;
  /// E threshold
  double mEThreshold;
  int maxevents;
  int eventcounter;
  bool usearea;
  double phisize(double eta);
  double etasize(double eta);
};

#endif
