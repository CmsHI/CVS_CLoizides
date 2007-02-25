// $Id: THistFileService.cc,v 1.1 2007/02/22 17:12:26 loizides Exp $

#include "CLoizides/Utils/interface/THistFileService.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/ActivityRegistry.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <TH1D.h>
#include <TString.h>
#include <TRegexp.h>

#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>

THistFileService::THistFileService(const edm::ParameterSet & cfg, 
                                   edm::ActivityRegistry & r ) :
   TFileService(cfg, r) {
}

THistFileService::~THistFileService() {
}

const std::string & THistFileService::getParName(const std::string &pname,
                                                 const edm::ParameterSet &cfg) const {

  std::vector<std::string> params = cfg.getParameterNames();

  std::vector<std::string>::const_iterator  it = 
     find(params.begin(), params.end(), pname);

  if (it != params.end()) {
    return pname;
  }

  uint regexplen=0;
  std::vector<std::string>::const_iterator 
     regexpmatch = params.end();

  TString namestr(pname);
  TString namesuf(namestr(namestr.Last('_')+1,namestr.Length()));

  for(it = params.begin(); it != params.end(); it++) {

     TString regexpstr(*it);
     if(regexpstr.EndsWith(namesuf)==kFALSE) 
        continue;

     Bool_t wildcard=0;
     if(regexpstr.Contains("_")) {
        regexpstr.ReplaceAll("_","*");
        wildcard=1;
     } 

     TRegexp e(regexpstr,wildcard);
     if(namestr.Index(e)!=-1) {
        if(it->size() > regexplen) {
           regexplen=it->size();
           regexpmatch=it;
        } 
     }
  }

  if(regexpmatch!=params.end())
     return *regexpmatch;
  else return pname;
}
