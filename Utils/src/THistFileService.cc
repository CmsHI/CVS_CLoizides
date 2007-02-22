// $Id:$

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/ActivityRegistry.h"
#include "CLoizides/Utils/interface/THistFileService.h"

THistFileService::THistFileService(const edm::ParameterSet & cfg, 
                                   edm::ActivityRegistry & r ) :
   TFileService(cfg, r) {
}

THistFileService::~THistFileService() {
}
