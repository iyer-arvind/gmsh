set(src
  FormulationHelper.cpp

  FormulationPoisson.cpp
  FormulationImpedance.cpp
  FormulationSilverMuller.cpp
  FormulationSteadySlow.cpp
  FormulationPML.cpp

  FormulationLagrange.cpp
  FormulationLagrangeOne.cpp
  FormulationLagrangeTwo.cpp

  FormulationEMDA.cpp
  FormulationUpdateEMDA.cpp

  FormulationOO2.cpp
  FormulationUpdateOO2.cpp

  FormulationOSRCScalar.cpp
  FormulationOSRCScalarOne.cpp
  FormulationOSRCScalarTwo.cpp
  FormulationOSRCScalarThree.cpp
  FormulationOSRCScalarFour.cpp
  FormulationUpdateOSRCScalar.cpp

  FormulationOSRCVector.cpp
  FormulationOSRCVectorOne.cpp
  FormulationOSRCVectorTwo.cpp
  FormulationOSRCVectorThree.cpp
  FormulationOSRCVectorFour.cpp
  FormulationOSRCVectorFive.cpp
  FormulationOSRCVectorSix.cpp
  FormulationOSRCVectorSeven.cpp
  FormulationOSRCVectorEight.cpp
  FormulationOSRCVectorNine.cpp
  FormulationUpdateOSRCVector.cpp

  FormulationOSRCHelper.cpp

  FormulationJFLee.cpp
  FormulationJFLeeOne.cpp
  FormulationJFLeeTwo.cpp
  FormulationJFLeeThree.cpp
  FormulationJFLeeFour.cpp
  FormulationJFLeeFive.cpp
  FormulationJFLeeSix.cpp
  FormulationJFLeeSeven.cpp
  FormulationJFLeeEight.cpp
  FormulationUpdateJFLee.cpp

  TermProjectionField.cpp
  TermProjectionGrad.cpp

  Quadrature.cpp
)

add_sources(formulation "${src}")
