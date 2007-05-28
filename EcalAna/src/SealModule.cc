// $Id: SealModule.cc,v 1.4 2007/05/17 20:16:11 loizides Exp $

#include "PluginManager/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "CLoizides/EcalAna/interface/EcalAna.h"

DEFINE_SEAL_MODULE();
DEFINE_ANOTHER_FWK_MODULE(EcalAna);
