#include "BasisGenerator.h"
#include "GroupOfJacobian.h"
#include "GroupOfElement.h"
#include "Quadrature.h"

#include "FormulationProjectionVector.h"

using namespace std;

template<>
FormulationProjectionVector<double>::
FormulationProjectionVector(fullVector<double> (*f)(fullVector<double>& xyz),
                            FunctionSpaceVector& fs){
  // Domain //
  GroupOfElement& goe = fs.getSupport();

  // Check GroupOfElement Stats: Uniform Mesh //
  const vector<size_t>& gType = goe.getTypeStats();
  const size_t nGType = gType.size();
  size_t eType = (size_t)(-1);

  for(size_t i = 0; i < nGType; i++)
    if((eType == (size_t)(-1)) && (gType[i] != 0))
      eType = i;
    else if((eType != (size_t)(-1)) && (gType[i] != 0))
      throw Exception
        ("FormulationProjectionVector<real> needs a uniform mesh");

  // Save fspace //
  fspace = &fs;
  basis  = &fs.getBasis(eType);

  // Gaussian Quadrature //
  Quadrature gauss(eType, basis->getOrder(), 2);

  const fullMatrix<double>& gC = gauss.getPoints();
  const fullVector<double>& gW = gauss.getWeights();

  // Local Terms //
  basis->preEvaluateFunctions(gC);

  GroupOfJacobian jac(goe, gC, "invert");

  localTerms1 = new TermGradGrad(jac, *basis, gW);
  localTerms2 = new TermProjectionGrad(jac, *basis, gW, gC, f);
}

template<>
FormulationProjectionVector<double>::
~FormulationProjectionVector(void){
  delete localTerms1;
  delete localTerms2;
}

template<>
double FormulationProjectionVector<double>::
weak(size_t dofI, size_t dofJ, size_t elementId) const{

  return localTerms1->getTerm(dofI, dofJ, elementId);
}

template<>
double FormulationProjectionVector<double>::
rhs(size_t equationI, size_t elementId) const{

  return localTerms2->getTerm(0, equationI, elementId);
}

template<>
bool FormulationProjectionVector<double>::isGeneral(void) const{

  return false;
}

template<>
double FormulationProjectionVector<double>::
weakB(size_t dofI, size_t dofJ, size_t elementId) const{

  return 0;
}

template<>
const FunctionSpace& FormulationProjectionVector<double>::
fs(void) const{

  return *fspace;
}
