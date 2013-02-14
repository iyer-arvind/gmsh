#ifndef _FORMULATIONPROJECTIONSCALAR_H_
#define _FORMULATIONPROJECTIONSCALAR_H_

#include "FunctionSpaceScalar.h"
#include "fullMatrix.h"

#include "TermFieldField.h"
#include "TermProjectionField.h"

#include "Formulation.h"

/**
   @class FormulationProjectionScalar
   @brief Formulation for the Projection of a Scalar Function problem

   Scalar Formulation for the @em L2 @em Projection problem
 */

class FormulationProjectionScalar: public Formulation{
 private:
  // Function to Project //
  double (*f)(fullVector<double>& xyz);

  // Function Space & Basis //
  FunctionSpaceScalar* fspace;
  const Basis*         basis;

  // Local Terms //
  TermFieldField*      localTerms1;
  TermProjectionField* localTerms2;

 public:
  FormulationProjectionScalar(double (*f)(fullVector<double>& xyz),
			      FunctionSpaceScalar& fs);

  virtual ~FormulationProjectionScalar(void);

  virtual bool isGeneral(void) const;

  virtual double weak(unsigned int dofI, unsigned int dofJ,
		      unsigned int elementId) const;

  virtual double weakB(unsigned int dofI, unsigned int dofJ,
                       unsigned int elementId) const;

  virtual double rhs(unsigned int equationI,
		     unsigned int elementId) const;

  virtual const FunctionSpace& fs(void) const;
};

/**
   @fn FormulationProjectionScalar::FormulationProjectionScalar
   @param f The function to project
   @param fs A FunctionSpaceNode

   Instantiates a new FormulationProjectionScalar to project
   the given function@n

   FormulationProjectionScalar will use the given FunctionSpace
   for the projection
   **

   @fn FormulationProjectionScalar::~FormulationProjectionScalar
   Deletes the this FormulationProjectionScalar
   **
*/

#endif
