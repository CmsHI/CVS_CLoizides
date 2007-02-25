// $Id:$

#include "PluginManager/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "CLoizides/InvMassAna/interface/MCInvMassAna.h"
#include "CLoizides/InvMassAna/interface/MCParticleAna.h"
#include "CLoizides/InvMassAna/interface/MuonInvMassAna.h"

DEFINE_SEAL_MODULE();
DEFINE_ANOTHER_FWK_MODULE(MCInvMassAna);
DEFINE_ANOTHER_FWK_MODULE(MCParticleAna);
DEFINE_ANOTHER_FWK_MODULE(MuonInvMassAna);

