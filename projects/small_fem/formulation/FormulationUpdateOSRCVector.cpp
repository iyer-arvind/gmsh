#include "Exception.h"

#include "FormulationHelper.h"
#include "FormulationOSRCHelper.h"

#include "FormulationOSRCVector.h"
#include "FormulationUpdateOSRCVector.h"

using namespace std;

FormulationUpdateOSRCVector::
FormulationUpdateOSRCVector(DDMContextOSRCVector& context){
  // Save DDMContext //
  this->context = &context;

  // Get Domain and FunctionSpace from DDMContext //
  fspaceG = &(context.getFunctionSpaceG()); // For DDM Field: test and unknown
  fR      = &(context.getRFunctionSpace());
  fPhi    = &(context.getPhiFunctionSpace());
  ddomain = &(context.getDomain());

  // Check GroupOfElement Stats: Uniform Mesh //
  pair<bool, size_t> uniform = ddomain->isUniform();
  size_t               eType = uniform.second;

  if(!uniform.first)
    throw Exception("FormulationUpdateOSRCVector needs a uniform mesh");

  // Pade //
  double theta = context.getRotation();
  NPade        = context.getNPade();
  A.resize(NPade);
  B.resize(NPade);

  twoR0 = FormulationOSRCHelper::padeR0(NPade, theta) * Complex(2, 0);

  for(int j = 0; j < NPade; j++)
    A[j] = FormulationOSRCHelper::padeA(j + 1, NPade, theta);

  for(int j = 0; j < NPade; j++)
    B[j] = FormulationOSRCHelper::padeB(j + 1, NPade, theta);

  // Basis //
  basis = &fspaceG->getBasis(eType);

  // Gaussian Quadrature //
  gauss = new Quadrature(eType, basis->getOrder(), 2);

  // Pre-evalution //
  const fullMatrix<double>& gC = gauss->getPoints();
  basis->preEvaluateFunctions(gC);

  // Jacobian //
  jac = new GroupOfJacobian(*ddomain, gC, "invert");

  // Init Auxiliary Solution //
  solPhi.resize(NPade);
  FormulationHelper::initDofMap(*fR,   *ddomain, solR);
  FormulationHelper::initDofMap(*fPhi, *ddomain, solPhi);

  // Local Terms //
  lGout = new TermGradGrad<double>(*jac, *basis, *gauss);

  // NB: lGin, lR and lPhi will be computed at update, when vol Sys is available
  lGin = NULL;
  lR   = NULL;
  lPhi = new TermProjectionGrad<Complex>*[NPade];

  for(int i = 0; i < NPade; i++)
    lPhi[i] = NULL;
}

FormulationUpdateOSRCVector::~FormulationUpdateOSRCVector(void){
  if(lGin)
    delete lGin;

  if(lR)
    delete lR;

  for(int i = 0; i < NPade; i++)
    if(lPhi[i])
      delete lPhi[i];

  delete[] lPhi;
  delete   lGout;
  delete   jac;
  delete   gauss;
}

Complex FormulationUpdateOSRCVector::weak(size_t dofI, size_t dofJ,
                                          size_t elementId) const{
  return
    Complex(lGout->getTerm(dofI, dofJ, elementId), 0);
}

Complex FormulationUpdateOSRCVector::rhs(size_t equationI,
                                         size_t elementId) const{
  // Pade Sum //
  Complex sum = Complex(0, 0);
  Complex two = Complex(2, 0);

  for(int i = 0; i < NPade; i++)
    sum += two * A[i] / B[i] * lPhi[i]->getTerm(equationI, 0, elementId);

  // FEM Term //
  return
    lGin->getTerm(equationI, 0, elementId)         -
    lR->getTerm(equationI, 0, elementId) * twoR0 + sum;
    /*
    lPhi->getTerm(equationI, 0, elementId);
    */
}

const FunctionSpace& FormulationUpdateOSRCVector::field(void) const{
  return *fspaceG;
}

const FunctionSpace& FormulationUpdateOSRCVector::test(void) const{
  return *fspaceG;
}

const GroupOfElement& FormulationUpdateOSRCVector::domain(void) const{
  return *ddomain;
}

bool FormulationUpdateOSRCVector::isBlock(void) const{
  return true;
}

void FormulationUpdateOSRCVector::update(void){
  // Delete RHS (lGin, lR, lPhi)
  if(lGin)
    delete lGin;
  if(lR)
    delete lR;

  for(int i = 0; i < NPade; i++)
    if(lPhi[i])
      delete lPhi[i];

  // Get DDM Dofs and auxiliary solutions (at border) from DDMContext //
  const map<Dof, Complex>& ddm = context->getDDMDofs(); // ddm
  context->getSystem().getSolution(solR, 0);            // solR

  for(int i = 0; i < NPade; i++)
    context->getSystem().getSolution(solPhi[i], 0);     // solPhi[i]
  /*
  // Compute all solPhi[:] contributions and put them into solPhi[0]
  getAllPhi();

  // Get References
  map<Dof, Complex>&           alPhi = solPhi[0];
  const FunctionSpaceVector& phiZero = *(*fPhi)[0];
  */
  // Pre-evalution
  const fullMatrix<double>& gC = gauss->getPoints();
  basis->preEvaluateFunctions(gC);

  // New RHS
  lGin = new TermProjectionGrad<Complex>(*jac, *basis, *gauss, *fspaceG, ddm);
  lR   = new TermProjectionGrad<Complex>(*jac, *basis, *gauss, *fR     , solR);
  /*
  lPhi = new TermProjectionGrad<Complex>(*jac, *basis, *gauss,  phiZero, alPhi);
  */
  for(int i = 0; i < NPade; i++)
    lPhi[i] = new TermProjectionGrad<Complex>(*jac, *basis, *gauss,
                                              *(*fPhi)[i], solPhi[i]);
}
/*
void FormulationUpdateOSRCVector::getAllPhi(void){
  // Sum all contributions into solPhi[0] //
  map<Dof, Complex>::iterator endZero = solPhi[0].end();
  map<Dof, Complex>::iterator itZero;
  map<Dof, Complex>::iterator itI;

  // Firt multiply by A[0] / B[0]
  itZero  = solPhi[0].begin();

  for(; itZero != endZero; itZero++)
    itZero->second = itZero->second * Complex(2, 0) * A[0] / B[0];

  // The sum other contributions
  for(int i = 1; i < NPade; i++){
    itZero  = solPhi[0].begin();
    itI     = solPhi[i].begin();

    for(; itZero != endZero; itZero++, itI++)
      itZero->second = itZero->second + itI->second * Complex(2,0)* A[i] / B[i];
  }
}
*/
