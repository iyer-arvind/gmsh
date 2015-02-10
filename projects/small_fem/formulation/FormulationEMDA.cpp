#include "SmallFem.h"
#include "FormulationEMDA.h"

#include "TermProjectionField.h"
#include "TermProjectionGrad.h"
#include "TermFieldField.h"
#include "TermGradGrad.h"

using namespace std;

FormulationEMDA::FormulationEMDA(DDMContextEMDA& context){
  // Save DDMContext //
  this->context = &context;

  // Get Domain and FunctionSpace from DDMContext //
  fspace  = &context.getFunctionSpace();  // Testing Field and Unknown Field
  fspaceG = &context.getFunctionSpaceG(); // DDM Field
  ddomain = &context.getDomain();

  // Check GroupOfElement Stats: Uniform Mesh //
  pair<bool, size_t> uniform = ddomain->isUniform();
  size_t               eType = uniform.second;

  if(!uniform.first)
    throw Exception("FormulationEMDA needs a uniform mesh");

  // Wavenumber & Chi //
  this->k   = context.getWavenumber();
  this->chi = context.getChi();

  // Basis //
  basis = &fspace->getBasis(eType);

  // Gaussian Quadrature //
  gauss = new Quadrature(eType, basis->getOrder(), 2);

  // Pre-evalution //
  const fullMatrix<double>& gC = gauss->getPoints();
  basis->preEvaluateFunctions(gC);

  // Get DDM Dofs from DDMContext //
  const map<Dof, Complex>& ddm = context.getDDMDofs();

  // Local Terms //
  if(fspace->isScalar()){
    jac      = new GroupOfJacobian(*ddomain, gC, "jacobian");
    localLHS = new TermFieldField<double>(*jac, *basis, *gauss);
    localRHS =
      new TermProjectionField<Complex>(*jac, *basis, *gauss, *fspaceG, ddm);
  }

  else{
    jac      = new GroupOfJacobian(*ddomain, gC, "invert");
    localLHS = new TermGradGrad<double>(*jac, *basis, *gauss);
    localRHS =
      new TermProjectionGrad<Complex>(*jac, *basis, *gauss, *fspaceG, ddm);
  }
}

FormulationEMDA::~FormulationEMDA(void){
  delete localLHS;
  delete localRHS;
  delete jac;
  delete gauss;
}

Complex FormulationEMDA::weak(size_t dofI, size_t dofJ, size_t elementId) const{
  return Complex(chi, -k) * localLHS->getTerm(dofI, dofJ, elementId);
}

Complex FormulationEMDA::rhs(size_t equationI, size_t elementId) const{
  return localRHS->getTerm(equationI, 0, elementId);
}

const FunctionSpace& FormulationEMDA::field(void) const{
  return *fspace;
}

const FunctionSpace& FormulationEMDA::test(void) const{
  return *fspace;
}

const GroupOfElement& FormulationEMDA::domain(void) const{
  return *ddomain;
}

bool FormulationEMDA::isBlock(void) const{
  return true;
}

void FormulationEMDA::update(void){
  // Delete RHS
  delete localRHS;

  // Get DDM Dofs from DDMContext
  const map<Dof, Complex>& ddm = context->getDDMDofs();

  // Pre-evalution
  const fullMatrix<double>& gC = gauss->getPoints();
  basis->preEvaluateFunctions(gC);

  // New RHS
  if(fspace->isScalar())
    localRHS =
      new TermProjectionField<Complex>(*jac, *basis, *gauss, *fspaceG, ddm);

  else
    localRHS =
      new TermProjectionGrad<Complex>(*jac, *basis, *gauss, *fspaceG, ddm);
}
