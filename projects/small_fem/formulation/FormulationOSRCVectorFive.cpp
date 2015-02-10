#include "SmallFem.h"
#include "Exception.h"
#include "FormulationOSRCVectorFive.h"

using namespace std;

FormulationOSRCVectorFive::FormulationOSRCVectorFive(void){
}

FormulationOSRCVectorFive::
FormulationOSRCVectorFive(const GroupOfElement& domain,
                          const FunctionSpace& field,
                          const FunctionSpace& test,
                          const TermGradGrad<double>& localGG){
  // Save Data //
  this->ffield  = &field;
  this->ttest   = &test;
  this->ddomain = &domain;
  this->localGG = &localGG;
}

FormulationOSRCVectorFive::~FormulationOSRCVectorFive(void){
}

Complex FormulationOSRCVectorFive::weak(size_t dofI, size_t dofJ,
                                        size_t elementId) const{

  return localGG->getTerm(dofI, dofJ, elementId);
}

Complex FormulationOSRCVectorFive::rhs(size_t equationI,
                                       size_t elementId) const{
  return Complex(0, 0);
}

const FunctionSpace& FormulationOSRCVectorFive::field(void) const{
  return *ffield;
}

const FunctionSpace& FormulationOSRCVectorFive::test(void) const{
  return *ttest;
}

const GroupOfElement& FormulationOSRCVectorFive::domain(void) const{
  return *ddomain;
}

bool FormulationOSRCVectorFive::isBlock(void) const{
  return true;
}
