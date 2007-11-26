#ifndef CaloTowersCreator_MyCaloTowerCands_h
#define CaloTowersCreator_MyCaloTowerCands_h

/** \class MyCaloTowerCands
 *
 * Framework module that produces a collection
 * of candidates with a CaloTowerCandidate compoment
 *
 * \author Luca Lista, INFN
 *
 * \version $Revision: 1.1 $
 *
 * $Id: MyCaloTowerCands.h,v 1.1 2007/03/07 19:54:50 mansj Exp $
 *
 */
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <string>
#define consti

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
#ifdef consti
  //debug
  int eventcounter;
  double phisize(double eta);
  double etasize(double eta);
#endif
};

#endif
