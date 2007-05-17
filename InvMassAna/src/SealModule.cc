// $Id: SealModule.cc,v 1.3 2007/03/01 04:15:53 loizides Exp $

#include "PluginManager/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "CLoizides/InvMassAna/interface/MCParticleAna.h"
#include "CLoizides/InvMassAna/interface/SimParticleAna.h"
#include "CLoizides/InvMassAna/interface/MCInvMassAna.h"
#include "CLoizides/InvMassAna/interface/SimInvMassAna.h"
#include "CLoizides/InvMassAna/interface/MuonInvMassAna.h"
#include "CLoizides/InvMassAna/interface/MuonHLTInvMassAna.h"
#include "CLoizides/InvMassAna/interface/MCChainAna.h"

DEFINE_SEAL_MODULE();
DEFINE_ANOTHER_FWK_MODULE(MCParticleAna);
DEFINE_ANOTHER_FWK_MODULE(SimParticleAna);
DEFINE_ANOTHER_FWK_MODULE(MCInvMassAna);
DEFINE_ANOTHER_FWK_MODULE(SimInvMassAna);
DEFINE_ANOTHER_FWK_MODULE(MuonInvMassAna);
DEFINE_ANOTHER_FWK_MODULE(MuonHLTInvMassAna);
DEFINE_ANOTHER_FWK_MODULE(MCChainAna);
