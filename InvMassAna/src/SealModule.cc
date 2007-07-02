// $Id: SealModule.cc,v 1.4 2007/05/17 20:16:11 loizides Exp $

#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "CLoizides/InvMassAna/interface/MCParticleAna.h"
#include "CLoizides/InvMassAna/interface/SimParticleAna.h"
#include "CLoizides/InvMassAna/interface/MCInvMassAna.h"
#include "CLoizides/InvMassAna/interface/SimInvMassAna.h"
#include "CLoizides/InvMassAna/interface/MuonInvMassAna.h"
#include "CLoizides/InvMassAna/interface/MuonHLTInvMassAna.h"
#include "CLoizides/InvMassAna/interface/MCChainAna.h"

DEFINE_ANOTHER_FWK_MODULE(MCParticleAna);
DEFINE_ANOTHER_FWK_MODULE(SimParticleAna);
DEFINE_ANOTHER_FWK_MODULE(MCInvMassAna);
DEFINE_ANOTHER_FWK_MODULE(SimInvMassAna);
DEFINE_ANOTHER_FWK_MODULE(MuonInvMassAna);
DEFINE_ANOTHER_FWK_MODULE(MuonHLTInvMassAna);
DEFINE_ANOTHER_FWK_MODULE(MCChainAna);
