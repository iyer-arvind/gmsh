#include "BasisGenerator.h"
#include "GroupOfJacobian.h"
#include "Quadrature.h"

#include "FormulationSteadyWaveVector.h"

using namespace std;

FormulationSteadyWaveVector::
FormulationSteadyWaveVector(GroupOfElement& goe,
                            const FunctionSpaceVector& fs,
                            double k){

  // Check GroupOfElement Stats: Uniform Mesh //
  const vector<size_t>& gType = goe.getTypeStats();
  const size_t nGType = gType.size();
  size_t eType = (size_t)(-1);

  for(size_t i = 0; i < nGType; i++)
    if((eType == (size_t)(-1)) && (gType[i] != 0))
      eType = i;
    else if((eType != (size_t)(-1)) && (gType[i] != 0))
      throw Exception("FormulationSteadyWaveVector needs a uniform mesh");

  // Wave Squared //
  kSquare = k * k;

  // Save FunctionSpace & Get Basis //
  const Basis& basis = fs.getBasis(eType);
  const size_t order = basis.getOrder();
  fspace             = &fs;

  // Gaussian Quadrature //
  Quadrature gaussCurlCurl(eType, order - 1, 2);
  Quadrature gaussFF(eType, order, 2);

  const fullMatrix<double>& gC1 = gaussCurlCurl.getPoints();
  const fullVector<double>& gW1 = gaussCurlCurl.getWeights();

  const fullMatrix<double>& gC2 = gaussFF.getPoints();
  const fullVector<double>& gW2 = gaussFF.getWeights();

  // Local Terms //
  basis.preEvaluateDerivatives(gC1);
  basis.preEvaluateFunctions(gC2);

  GroupOfJacobian jac1(goe, gC1, "jacobian");
  GroupOfJacobian jac2(goe, gC2, "invert");

  localTerms1 = new TermCurlCurl(jac1, basis, gW1);
  localTerms2 = new TermGradGrad(jac2, basis, gW2);
}

FormulationSteadyWaveVector::~FormulationSteadyWaveVector(void){

  delete localTerms1;
  delete localTerms2;
}

double FormulationSteadyWaveVector::weak(size_t dofI, size_t dofJ,
                                         size_t elementId) const{
  return
    localTerms1->getTerm(dofI, dofJ, elementId) -
    localTerms2->getTerm(dofI, dofJ, elementId) * kSquare;
}

double FormulationSteadyWaveVector::rhs(size_t equationI,
                                        size_t elementId) const{
  return 0;
}

bool FormulationSteadyWaveVector::isGeneral(void) const{
  return false;
}

double FormulationSteadyWaveVector::weakB(size_t dofI, size_t dofJ,
                                          size_t elementId) const{
  return 0;
}

const FunctionSpace& FormulationSteadyWaveVector::fs(void) const{
  return *fspace;
}
