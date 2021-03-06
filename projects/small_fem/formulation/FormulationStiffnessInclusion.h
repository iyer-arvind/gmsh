/////////////////////////////////////////////////////////
// Templates Implementations for FormulationStiffness: //
// Inclusion compilation model                         //
//                                                     //
// Damn you gcc: we want 'export' !                    //
/////////////////////////////////////////////////////////

#include "GroupOfJacobian.h"
#include "TermGradGrad.h"
#include "TermCurlCurl.h"
#include "TermDummy.h"
#include "Quadrature.h"

template<typename scalar>
FormulationStiffness<scalar>::FormulationStiffness(const GroupOfElement& domain,
                                                   const FunctionSpace& field,
                                                   const FunctionSpace& test){
  // Save Data //
  ddomain = &domain;
  ffield  = &field;
  ttest   = &test;

  // Is domain empty ? //
  if(domain.isEmpty()){
    localTerms = new TermDummy<scalar>;
    return;
  }

  // Check domain stats: uniform mesh //
  std::pair<bool, size_t> uniform = domain.isUniform();
  size_t                    eType = uniform.second;

  if(!uniform.first)
    throw Exception("FormulationStiffness needs a uniform mesh");

  // Check if same forms //
  const size_t form = field.getForm();
  if(form != test.getForm())
    throw Exception
      ("FormulationStiffness needs function spaces of the same form");

  // Get Basis //
  const Basis& basis = test.getBasis(eType);
  const size_t order = basis.getOrder();

  // Gaussian Quadrature //
  Quadrature gauss(eType, order - 1, 2);
  const fullMatrix<double>& gC = gauss.getPoints();

  // Derivatives //
  basis.preEvaluateDerivatives(gC);

  // Local Terms //
  switch(form){
  // Gradiends
  case 0:{
    GroupOfJacobian jac(domain, gC, "invert");
    localTerms = new TermGradGrad<scalar>(jac, basis, gauss);
    break;
  }

  // Curls //
  case 1:{
    GroupOfJacobian jac(domain, gC, "jacobian");
    localTerms = new TermCurlCurl<scalar>(jac, basis, gauss);
    break;
  }

  // Else //
  default:
    throw Exception("FormulationStiffness does 0 and 1 forms only");
  }
}

template<typename scalar>
FormulationStiffness<scalar>::
FormulationStiffness(const GroupOfElement& domain,
                     const FunctionSpace& field,
                     const FunctionSpace& test,
                     void (*m)(fullVector<double>&, fullMatrix<scalar>&)){
  // Save Data //
  ddomain = &domain;
  ffield  = &field;
  ttest   = &test;

  // Is domain empty ? //
  if(domain.isEmpty()){
    localTerms = new TermDummy<scalar>;
    return;
  }

  // Check domain stats: uniform mesh //
  std::pair<bool, size_t> uniform = domain.isUniform();
  size_t                    eType = uniform.second;

  if(!uniform.first)
    throw Exception("FormulationStiffness needs a uniform mesh");

  // Check if same forms //
  const size_t form = field.getForm();
  if(form != test.getForm())
    throw Exception
      ("FormulationStiffness needs function spaces of the same form");

  // Get Basis //
  const Basis& basis = test.getBasis(eType);
  const size_t order = basis.getOrder();

  // Gaussian Quadrature //
  Quadrature gauss(eType, order - 1, 2);
  const fullMatrix<double>& gC = gauss.getPoints();

  // Derivatives //
  basis.preEvaluateDerivatives(gC);

  // Local Terms //
  switch(form){
  // Gradiends
  case 0:{
    GroupOfJacobian jac(domain, gC, "invert");
    localTerms = new TermGradGrad<scalar>(jac, basis, gauss, m);
    break;
  }

  // Curls //
  case 1:{
    GroupOfJacobian jac(domain, gC, "jacobian");
    localTerms = new TermCurlCurl<scalar>(jac, basis, gauss, m);
    break;
  }

  // Else //
  default:
    throw Exception("FormulationStiffness does 0 and 1 forms only");
  }
}

template<typename scalar>
FormulationStiffness<scalar>::~FormulationStiffness(void){
  delete localTerms;
}

template<typename scalar>
scalar FormulationStiffness<scalar>::
weak(size_t dofI, size_t dofJ, size_t elementId) const{

  return localTerms->getTerm(dofI, dofJ, elementId);
}

template<typename scalar>
scalar FormulationStiffness<scalar>::
rhs(size_t equationI, size_t elementId) const{

  return 0;
}

template<typename scalar>
const FunctionSpace& FormulationStiffness<scalar>::field(void) const{
  return *ffield;
}

template<typename scalar>
const FunctionSpace& FormulationStiffness<scalar>::test(void) const{
  return *ttest;
}

template<typename scalar>
const GroupOfElement& FormulationStiffness<scalar>::domain(void) const{
  return *ddomain;
}

template<typename scalar>
bool FormulationStiffness<scalar>::isBlock(void) const{
  return true;
}
