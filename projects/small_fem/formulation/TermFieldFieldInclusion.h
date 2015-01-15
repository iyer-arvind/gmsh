///////////////////////////////////////////////////
// Templates Implementations for TermFieldField: //
// Inclusion compilation model                   //
//                                               //
// Damn you gcc: we want 'export' !              //
///////////////////////////////////////////////////

#include "Exception.h"
#include "ReferenceSpaceManager.h"

template<typename scalar>
TermFieldField<scalar>::TermFieldField(const GroupOfJacobian& goj,
                                       const Basis& basis,
                                       const Quadrature& quadrature){
  // Dummy Pre evaluation //
  preEvalDummy(goj, quadrature);

  // Init //
  init(goj, basis, basis, quadrature);
}

template<typename scalar>
TermFieldField<scalar>::TermFieldField(const GroupOfJacobian& goj,
                                       const Basis& field,
                                       const Basis& test,
                                       const Quadrature& quadrature){
  // Dummy Pre evaluation //
  preEvalDummy(goj, quadrature);

  // Init //
  init(goj, field, test, quadrature);
}

template<typename scalar>
TermFieldField<scalar>::TermFieldField(const GroupOfJacobian& goj,
                                       const Basis& basis,
                                       const Quadrature& quadrature,
                                       scalar (*f)(fullVector<double>& xyz)){
  // Function Pre evaluation //
  preEvalF(goj, quadrature, f);

  // Init //
  init(goj, basis, basis, quadrature);
}

template<typename scalar>
void TermFieldField<scalar>::init(const GroupOfJacobian& goj,
                                  const Basis& field,
                                  const Basis& test,
                                  const Quadrature& quadrature){
  // Basis Check //
  if(field.getForm() != 0)
    throw Exception("A Field Field Term must use a 0form basis");

  if(test.getForm() != 0)
    throw Exception("A Field Field Term must use a 0form basis");

  // Type //
  int eType = test.getType();

  if((int)(field.getType()) != eType)
    throw Exception("A Field Field Term must use basis functions"
                    "defined on the same geomtrical type for test and field");

  // Orientations & Functions //
  this->orientationStat = &goj.getAllElements().getOrientationStats(eType);
  this->nOrientation    = ReferenceSpaceManager::getNOrientation(eType);
  this->nFunctionField  = field.getNFunction();
  this->nFunctionTest   =  test.getNFunction();

  // Get Integration Data
  //const fullMatrix<double>& gC = quadrature.getPoints();
  const fullVector<double>& gW = quadrature.getWeights();

  // Compute //
  fullMatrix<scalar>** cM;
  fullMatrix<scalar>** bM;

  computeC(field, test, gW, cM);
  computeB(goj, gW.size(), bM);

  this->allocA(this->nFunctionField * this->nFunctionTest);
  this->computeA(bM, cM);

  // Clean up //
  this->clean(bM, cM);
}

template<typename scalar>
TermFieldField<scalar>::~TermFieldField(void){
}

template<typename scalar>
void TermFieldField<scalar>::computeC(const Basis& field,
                                      const Basis& test,
                                      const fullVector<double>& gW,
                                      fullMatrix<scalar>**& cM){
  const size_t nG = gW.size();
  size_t l;

  // Alloc //
  cM = new fullMatrix<scalar>*[this->nOrientation];

  for(size_t s = 0; s < this->nOrientation; s++)
    if((*this->orientationStat)[s] != 0)
      // If there is at least one element with orientation 's'
      cM[s] = new fullMatrix<scalar>(nG, this->nFunctionField *
                                         this->nFunctionTest);
    else
      // Else: empty matrix
      cM[s] = new fullMatrix<scalar>(0, 0);

  // Fill //
  for(size_t s = 0; s < this->nOrientation; s++){
    // If there is at least one element with this orientation
    if((*this->orientationStat)[s] != 0){

      // Get functions for this Orientation
      const fullMatrix<double>& phiTest  =  test.getPreEvaluatedFunctions(s);
      const fullMatrix<double>& phiField = field.getPreEvaluatedFunctions(s);

      // Loop on Gauss Points
      for(size_t g = 0; g < nG; g++){
        // Loop on Functions
        l = 0;

        for(size_t i = 0; i < this->nFunctionTest; i++){
          for(size_t j = 0; j < this->nFunctionField; j++){
            (*cM[s])(g, l) = gW(g) * phiTest(i, g) * phiField(j, g);
            l++;
          }
        }
      }
    }
  }
}

template<typename scalar>
void TermFieldField<scalar>::computeB(const GroupOfJacobian& goj,
                                      size_t nG,
                                      fullMatrix<scalar>**& bM){
  size_t offset = 0;
  size_t j;

  // Alloc //
  bM = new fullMatrix<scalar>*[this->nOrientation];

  for(size_t s = 0; s < this->nOrientation; s++)
    bM[s] = new fullMatrix<scalar>((*this->orientationStat)[s], nG);

  // Fill //
  for(size_t s = 0; s < this->nOrientation; s++){
    // Loop On Element
    j = 0;

    for(size_t e = offset; e < offset + (*this->orientationStat)[s]; e++){
      // Get Jacobians
      const std::vector<const std::pair<const fullMatrix<double>*, double>*>&
        jacM = goj.getJacobian(e).getJacobianMatrix();

      // Loop on Gauss Points
      for(size_t g = 0; g < nG; g++)
        (*bM[s])(j, g) = alpha[e * nG + g] * fabs(jacM[g]->second);

      // Next Element in Orientation[s]
      j++;
    }

    // New Offset
    offset += (*this->orientationStat)[s];
  }
}

template<typename scalar>
void TermFieldField<scalar>::preEvalDummy(const GroupOfJacobian& goj,
                                          const Quadrature& quadrature){
  // Data //
  const size_t nPoint   = quadrature.getPoints().size1();
  const size_t nElement = goj.getAllElements().getAll().size();
  const size_t     size = nElement * nPoint;

  // Alloc //
  alpha.resize(size);

  // Populate //
  #pragma omp parallel for // First touch
  for(size_t i = 0; i < size; i++)
    alpha[i] = 1;
}

template<typename scalar>
void TermFieldField<scalar>::preEvalF(const GroupOfJacobian& goj,
                                      const Quadrature& quadrature,
                                      scalar (*f)(fullVector<double>& xyz)){
  // Data //
  const fullMatrix<double>&                gC = quadrature.getPoints();
  const std::vector<const MElement*>  element = goj.getAllElements().getAll();
  const size_t                       nPoint   = gC.size1();
  const size_t                       nElement = element.size();

  // Alloc //
  alpha.resize(nPoint * nElement);

  // Populate //
  #pragma omp parallel
  {
    // Temps (one for each thread) //
    fullVector<double>  xyz(3);
    double             pxyz[3];

    #pragma omp for
    for(size_t e = 0; e < nElement; e++){
      for(size_t g = 0; g < nPoint; g++){
        // Compute 'f' in the *physical* coordinate
        ReferenceSpaceManager::
          mapFromABCtoXYZ(*element[e], gC(g, 0), gC(g, 1), gC(g, 2), pxyz);

        xyz(0) = pxyz[0];
        xyz(1) = pxyz[1];
        xyz(2) = pxyz[2];

        alpha[e * nPoint + g] = f(xyz);
      }
    }
  }
}
