#include "DDMContextOSRCScalar.h"

using namespace std;

DDMContextOSRCScalar::
DDMContextOSRCScalar(const GroupOfElement& domain,
                     vector<const GroupOfElement*>& dirichlet,
                     const FunctionSpace& fSpace,
                     const FunctionSpace& fSpaceG,
                     const vector<const FunctionSpaceScalar*>& phi,
                     double k, Complex keps,
                     int NPade, double theta){

  // Check if scalar //
  if(!fSpace.isScalar())
    throw Exception("DDMContextOSRCScalar: need a scalar function space");

  // Data for OSRCScalar //
  this->domain    = &domain;
  this->fSpace    = &fSpace;
  this->fSpaceG   = &fSpaceG;
  this->dirichlet = dirichlet;
  this->phi       = &phi;
  this->NPade     = NPade;
  this->theta     = theta;
  this->k         = k;
  this->keps      = keps;
}

DDMContextOSRCScalar::~DDMContextOSRCScalar(void){
}
