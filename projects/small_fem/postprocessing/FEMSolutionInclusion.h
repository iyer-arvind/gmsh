////////////////////////////////////////////////
// Templates Implementations for FEMSolution: //
// Inclusion compilation model                //
//                                            //
// Damn you gcc: we want 'export' !           //
////////////////////////////////////////////////

#include "BasisLagrange.h"
#include "BasisGenerator.h"

template<typename scalar>
FEMSolution<scalar>::FEMSolution(void){
  isModulusPhase = false;
  saveMesh       = true;
  binary         = false;
  partition      = 0;
  pView          = new PViewDataGModel(PViewDataGModel::ElementNodeData);
}

template<typename scalar>
FEMSolution<scalar>::~FEMSolution(void){
  pView->destroyData();
  delete pView;
}

template<typename scalar>
void FEMSolution<scalar>::setModulusPhase(void){
  isModulusPhase = true;
}

template<typename scalar>
void FEMSolution<scalar>::setRealImaginary(void){
  isModulusPhase = false;
}

template<typename scalar>
void FEMSolution<scalar>::setSaveMesh(bool saveMesh){
  this->saveMesh = saveMesh;
}

template<typename scalar>
void FEMSolution<scalar>::setBinaryFormat(bool binary){
  this->binary = binary;
}

template<typename scalar>
void FEMSolution<scalar>::setParition(int partition){
  this->partition = partition;
}

template<typename scalar>
void FEMSolution<scalar>::clear(void){
  pView->destroyData();
}

template<typename scalar>
void FEMSolution<scalar>::write(std::string fileName) const{
  pView->setName(fileName);

  std::stringstream stream;
  stream << fileName;

  if(partition > 0)
    stream << "_part" << partition;

  stream << ".msh";
  pView->writeMSH(stream.str(),
                  2.2, binary, saveMesh, false, partition, true, false);
}

template<typename scalar>
void FEMSolution<scalar>::addCoefficients(size_t step,
                                          double time,
                                          const GroupOfElement& goe,
                                          const FunctionSpace& fs,
                                          const std::map<Dof, scalar>& coef){
  // Get Support and GModel //
  const std::vector<const MElement*>& element = goe.getAll();
  const size_t                       nElement = element.size();
  GModel&                               model = goe.getMesh().getModel();

  // Lagrange Basis & Interpolation matrices //
  // One lagrange basis per geo type //
  const std::vector<size_t> typeStat = goe.getTypeStats();
  const size_t              nGeoType = typeStat.size();

  std::vector<BasisLagrange*> lagrange(nGeoType, NULL);

  for(size_t i = 0; i < nGeoType; i++){
    if(typeStat[i]){

      lagrange[i] = static_cast<BasisLagrange*>
        (BasisGenerator::generate(i, 0, fs.getBasis(i).getOrder(), "lagrange"));

      pView->setInterpolationMatrices(i,
                                      lagrange[i]->getCoefficient(),
                                      lagrange[i]->getMonomial());
    }
  }

  // Map with (Element Id, Lagrange coefficients) //
  std::map<int, std::vector<scalar> > data;

  // Number of components: Scalar of Vectorial Field ? //
  size_t nComp;

  if(fs.isScalar())
    nComp = 1;
  else
    nComp = 3;

  // Iterate on Element //
           std::vector<Dof>                      dof;
  typename std::map<Dof, scalar>::const_iterator end = coef.end();
  typename std::map<Dof, scalar>::const_iterator it;

  for(size_t i = 0; i < nElement; i++){
    // Get Element Dofs
    fs.getKeys(*element[i], dof);
    const size_t size = dof.size();

    // Get Coef In FS Basis
    std::vector<scalar> fsCoef(size);
    for(size_t j = 0; j < size; j++){
      // Get Value of Dof 'j'
      it = coef.find(dof[j]);

      // If found in map
      if(it != end)
        fsCoef[j] = it->second;

      // Else
      else
        fsCoef[j] = 0;
    }

    // Get Coef In Lagrange Basis
    std::vector<scalar> lCoef;
    toLagrange(*element[i], lagrange, fsCoef, fs, lCoef);

    // Add in map
    data.insert(std::pair<int, std::vector<scalar> >
                (element[i]->getNum(), lCoef));
  }

  // Add to PView //
  toPView(model, data, step, time, 0, nComp);

  // Clean //
  for(size_t i = 0; i < nGeoType; i++)
    if(typeStat[i])
      delete lagrange[i];
}
