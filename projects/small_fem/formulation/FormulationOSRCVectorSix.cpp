#include "SmallFem.h"
#include "Exception.h"
#include "FormulationOSRCVectorSix.h"

using namespace std;

FormulationOSRCVectorSix::FormulationOSRCVectorSix(void){
}

FormulationOSRCVectorSix::
FormulationOSRCVectorSix(const GroupOfElement& domain,
                         const FunctionSpace& field,
                         const FunctionSpace& test,
                         Complex kEps,
                         Complex Bi,
                         const TermGradGrad<double>& localGG,
                         const TermCurlCurl<double>& localCC){
  // Save Data //
  this->BiOverKEpsSquare = Bi / (kEps * kEps);
  this->ffield           = &field;
  this->ttest            = &test;
  this->ddomain          = &domain;
  this->localGG          = &localGG;
  this->localCC          = &localCC;
}

FormulationOSRCVectorSix::~FormulationOSRCVectorSix(void){
}

Complex FormulationOSRCVectorSix::weak(size_t dofI, size_t dofJ,
                                       size_t elementId) const{
  return
    Complex(-1, 0)   * localGG->getTerm(dofI, dofJ, elementId) +
    BiOverKEpsSquare * localCC->getTerm(dofI, dofJ, elementId);
}

Complex FormulationOSRCVectorSix::rhs(size_t equationI, size_t elementId) const{
  return Complex(0, 0);
}

const FunctionSpace& FormulationOSRCVectorSix::field(void) const{
  return *ffield;
}

const FunctionSpace& FormulationOSRCVectorSix::test(void) const{
  return *ttest;
}

const GroupOfElement& FormulationOSRCVectorSix::domain(void) const{
  return *ddomain;
}

bool FormulationOSRCVectorSix::isBlock(void) const{
  return true;
}
