#ifndef _FORMULATIONOSRC_H_
#define _FORMULATIONOSRC_H_

#include <map>

#include "SmallFem.h"
#include "GroupOfElement.h"
#include "FunctionSpaceScalar.h"
#include "TermProjectionField.h"
#include "TermFieldField.h"
#include "TermGradGrad.h"
#include "FormulationCoupled.h"
#include "GroupOfJacobian.h"
#include "Quadrature.h"

#include "FormulationOSRCOne.h"
#include "DDMContextOSRC.h"

/**
   @class FormulationOSRC
   @brief OSRC Formulation for DDM

   OSRC Formulation for DDM
 */

class FormulationOSRCOne;

class FormulationOSRC: public FormulationCoupled<Complex>{
 private:
  // DDMContext //
  DDMContextOSRC* context;

  // Stuff for updating RHS //
  const Basis*         basis;
  const FunctionSpace* field;
  Quadrature*          gaussFF;
  GroupOfJacobian*     jacFF;
  FormulationOSRCOne*  formulationOne;

  // Local Terms //
  TermFieldField<double>*       localFF;
  TermGradGrad<double>*         localGG;
  TermProjectionField<Complex>* localPr;

  // Formulations //
  std::list<const FormulationBlock<Complex>*> fList;

 public:
  FormulationOSRC(DDMContextOSRC& context);

  virtual ~FormulationOSRC(void);

  virtual
    const std::list<const FormulationBlock<Complex>*>&
                                               getFormulationBlocks(void) const;

  virtual bool isBlock(void) const;
  virtual void update(void);
};

/**
   @fn FormulationOSRC::FormulationOSRC
   @param context A DDMContextOSRC

   Instantiates a new FormulationOSRC with the given DDMContextOSRC
   **

   @fn FormulationOSRC::~FormulationOSRC
   Deletes this FormulationOSRC
   **

   @fn FormulationOSRC::update
   Updates the DDM Dof%s values from the DDMContext given at construction time
*/

#endif
