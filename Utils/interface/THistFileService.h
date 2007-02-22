// $Id:$

#ifndef CLoizidesUtils_THistFileService_h
#define CLoizidesUtils_THistFileService_h
/* \class THistFileService
 *
 * \author Constantin Loizides, MIT
 *
 */
#include <string>
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "TString.h"

namespace edm {
  class ActivityRegistry;
}

class THistFileService : public TFileService {
public:

  THistFileService(const edm::ParameterSet &, edm::ActivityRegistry &);
  ~THistFileService();

  template <typename T> T * makeHist(const std::string &namepref, 
                                     const edm::ParameterSet &cfg,
                                     const std::string &deftitle,
                                     int defnbinsx, 
                                     double defxmin, 
                                     double defxmax) const; 

  template <typename T> T * makeHist(const std::string &namepref, 
                                     const edm::ParameterSet &cfg,
                                     const std::string &deftitle,
                                     int defnbinsx, 
                                     double defxmin, 
                                     double defxmax,
                                     int defnbinsy, 
                                     double defymin, 
                                     double defymax) const; 
};

  template<typename T>
  inline 
  T * THistFileService::makeHist(const std::string &namepref, 
                                 const edm::ParameterSet &cfg,
                                 const std::string &deftitle,
                                 int defnbinsx, 
                                 double defxmin, 
                                 double defxmax) const {

    int  nbinsx=cfg.template getUntrackedParameter<int>    (Form("%s_nbinsx",namepref.c_str()), defnbinsx);
    double xmin=cfg.template getUntrackedParameter<double> (Form("%s_xmin",  namepref.c_str()), defxmin);
    double xmax=cfg.template getUntrackedParameter<double> (Form("%s_xmax",  namepref.c_str()), defxmax);
    std::string title(cfg.template getUntrackedParameter<std::string> 
                      (Form("%s_title",namepref.c_str()), deftitle));
    std::string defname(Form("%s_%d_%f_%f",namepref.c_str(),nbinsx,xmin,xmax));
    std::string name(cfg.template getUntrackedParameter<std::string>
                     (Form("%s_name",namepref.c_str()), defname));

    return make<T>(name.c_str(),title.c_str(),nbinsx,xmin,xmax);
  }

  template<typename T>
  inline 
  T * THistFileService::makeHist(const std::string &namepref, 
                                 const edm::ParameterSet &cfg,
                                 const std::string &deftitle,
                                 int defnbinsx, 
                                 double defxmin, 
                                 double defxmax,
                                 int defnbinsy, 
                                 double defymin, 
                                 double defymax) const {

    int  nbinsx=cfg.template getUntrackedParameter<int>    (Form("%s_nbinsx",namepref.c_str()), defnbinsx);
    double xmin=cfg.template getUntrackedParameter<double> (Form("%s_xmin",  namepref.c_str()), defxmin);
    double xmax=cfg.template getUntrackedParameter<double> (Form("%s_xmax",  namepref.c_str()), defxmax);
    int  nbinsy=cfg.template getUntrackedParameter<int>    (Form("%s_nbinsy",namepref.c_str()), defnbinsx);
    double ymin=cfg.template getUntrackedParameter<double> (Form("%s_ymin",  namepref.c_str()), defymin);
    double ymax=cfg.template getUntrackedParameter<double> (Form("%s_ymax",  namepref.c_str()), defymax);
    std::string title(cfg.template getUntrackedParameter<std::string> 
                      (Form("%s_title",namepref.c_str()), deftitle));
    std::string defname(Form("%s_%d_%f_%f",namepref.c_str(),nbinsx,xmin,xmax));
    std::string name(cfg.template getUntrackedParameter<std::string>
                     (Form("%s_name",namepref.c_str()), defname));

    return make<T>(name.c_str(),title.c_str(),nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  }

#endif
