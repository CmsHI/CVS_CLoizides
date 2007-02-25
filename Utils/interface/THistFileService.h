// $Id: THistFileService.h,v 1.1 2007/02/22 17:12:26 loizides Exp $

#ifndef CLoizidesUtils_THistFileService_h
#define CLoizidesUtils_THistFileService_h
/* \class THistFileService
 *
 * \author Constantin Loizides, MIT
 *
 */
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include <string>
#include <TString.h>

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

private:

      const std::string & getParName(const std::string &pnpref, 
                                     const edm::ParameterSet &cfg) const;
};


  template<typename T>
  inline 
  T * THistFileService::makeHist(const std::string &namepref, 
                                 const edm::ParameterSet &cfg,
                                 const std::string &deftitle,
                                 int defnbinsx, 
                                 double defxmin, 
                                 double defxmax) const {
     
    int  nbinsx=cfg.template getUntrackedParameter<int>(
         getParName(Form("%s_nbinsx",namepref.c_str()), cfg), defnbinsx);
    double xmin=cfg.template getUntrackedParameter<double>(
       getParName(Form("%s_xmin",namepref.c_str()), cfg), defxmin);
    double xmax=cfg.template getUntrackedParameter<double>(
       getParName(Form("%s_xmax",namepref.c_str()), cfg), defxmax);
    std::string title(cfg.template getUntrackedParameter<std::string>( 
                         getParName(Form("%s_title",namepref.c_str()), cfg), deftitle));
    std::string defname(Form("%s_%d_%f_%f",namepref.c_str(),nbinsx,xmin,xmax));
    std::string name(cfg.template getUntrackedParameter<std::string>(
                     getParName(Form("%s_name",namepref.c_str()), cfg), defname));

    T *ret =  make<T>(name.c_str(),title.c_str(),nbinsx,xmin,xmax);

    if(!ret) {
      throw edm::Exception(edm::errors::NullPointerError, "THistFileService::makeHist()\n")
         << "Could not get pointer to ROOT histogram: " << name << " " << title << " " 
         << nbinsx << " " << xmin << " " << xmax << "\n";
    }
    
    return ret;
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

    int  nbinsx=cfg.template getUntrackedParameter<int>(
       getParName(Form("%s_nbinsx",namepref.c_str()), cfg), defnbinsx);
    double xmin=cfg.template getUntrackedParameter<double>(
       getParName(Form("%s_xmin",namepref.c_str()), cfg), defxmin);
    double xmax=cfg.template getUntrackedParameter<double>(
       getParName(Form("%s_xmax",namepref.c_str()), cfg), defxmax);
    int  nbinsy=cfg.template getUntrackedParameter<int>(
       getParName(Form("%s_nbinsy",namepref.c_str()), cfg), defnbinsx);
    double ymin=cfg.template getUntrackedParameter<double>(
       getParName(Form("%s_ymin",namepref.c_str()), cfg), defymin);
    double ymax=cfg.template getUntrackedParameter<double>(
       getParName(Form("%s_ymax",namepref.c_str()), cfg), defymax);
    std::string title(cfg.template getUntrackedParameter<std::string>(
                         getParName(Form("%s_title",namepref.c_str()), cfg), deftitle));
    std::string defname(Form("%s_%d_%f_%f",namepref.c_str(),nbinsx,xmin,xmax));
    std::string name(cfg.template getUntrackedParameter<std::string>(
                        getParName(Form("%s_name",namepref.c_str()), cfg), defname));

    T *ret =  make<T>(name.c_str(),title.c_str(),nbinsx,xmin,xmax,nbinsy,ymin,ymax);

    if(!ret) {
      throw edm::Exception(edm::errors::NullPointerError, "THistFileService::makeHist()\n")
         << "Could not get pointer to ROOT histogram: " << name << " " << title << " " 
         << nbinsx << " " << xmin << " " << xmax << " "
         << nbinsy << " " << ymin << " " << ymax << "\n";
    }

    return ret;
  }
#endif
