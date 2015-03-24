#include "SmallFem.h"
#include "Timer.h"
#include "Mesh.h"
#include "System.h"
#include "SystemHelper.h"

#include "FormulationSteadyWave.h"
#include "FormulationSilverMuller.h"

#include <iostream>
#include <complex>
#include <cmath>

using namespace std;

static const int scal = 0;
static const int vect = 1;

static double theta = 0;
static double k;

complex<double> fSourceScal(fullVector<double>& xyz){
  //double p = xyz(0) * cos(theta) + xyz(1) * sin(theta);
  //return complex<double>(cos(k * p), sin(k * p));

  return complex<double>(1, 0);
  //return complex<double>(fabs(xyz(1)), 0);
  //return complex<double>(exp(-((xyz(1) * 4.2) * (xyz(1) * 4.2) +
  //                             (xyz(2) * 4.2) * (xyz(2) * 4.2))), 0);
}

fullVector<complex<double> > fSourceVec(fullVector<double>& xyz){
  fullVector<complex<double> > f(3);
  /*
  double p = xyz(0) * cos(theta) + xyz(1) * sin(theta);
  f(0) = 0;
  f(1) = complex<double>(cos(k * p), sin(k * p));
  f(2) = 0;
  */
  f(0) = 0;
  f(1) = 1;
  f(2) = 0;

  return f;
}

void compute(const Options& option){
  // Start Timer //
  Timer timer, assemble, solve;
  timer.start();

  // Get Domains //
  Mesh msh(option.getValue("-msh")[1]);
  /*
  GroupOfElement volume     = msh.getFromPhysical(100);
  GroupOfElement source     = msh.getFromPhysical(1000);
  GroupOfElement freeSpace  = msh.getFromPhysical(2000);
  */

  GroupOfElement volume     = msh.getFromPhysical(7);
  GroupOfElement source     = msh.getFromPhysical(5);
  GroupOfElement freeSpace  = msh.getFromPhysical(6);

  /*
  GroupOfElement volume     = msh.getFromPhysical(100);
  GroupOfElement source     = msh.getFromPhysical(1000);
  GroupOfElement freeSpace  = msh.getFromPhysical(4000);
  */
  // Full Domain //
  vector<const GroupOfElement*> domain(3);
  domain[0] = &volume;
  domain[1] = &source;
  domain[2] = &freeSpace;

  // Get Parameters //
  k                  = atof(option.getValue("-k")[1].c_str());
  const size_t order = atoi(option.getValue("-o")[1].c_str());

  // Get Type //
  int type;

  if(option.getValue("-type")[1].compare("scalar") == 0){
    type = scal;
    cout << "Scalar ";
  }

  else if(option.getValue("-type")[1].compare("vector") == 0){
    type = vect;
    cout << "Vectorial ";
  }

  else
    throw Exception("Bad -type: %s", option.getValue("-type")[1].c_str());


  // Function Space //
  assemble.start();
  FunctionSpace* fs = NULL;

  if(type == scal)
    fs = new FunctionSpaceScalar(domain, order);
  else
    fs = new FunctionSpaceVector(domain, order);

  // Formulation //
  FormulationSteadyWave<complex<double> > wave(volume, *fs, k);
  FormulationSilverMuller                 silverMuller(freeSpace, *fs, k);

  // System //
  System<complex<double> > sys;
  sys.addFormulation(wave);
  sys.addFormulation(silverMuller);

  // Dirichlet //
  if(type == scal)
    SystemHelper<complex<double> >::dirichlet(sys, *fs, source, fSourceScal);
  else
    SystemHelper<complex<double> >::dirichlet(sys, *fs, source, fSourceVec);

  cout << "Free Space (Order: " << order
       << " --- Wavenumber: "    << k << ")" << endl;

  // Assemble //
  sys.assemble();
  assemble.stop();

  cout << "Assembled: " << sys.getSize() << " "
       << "(" << assemble.time() << assemble.unit() << ")" << endl << flush;

  // Solve //
  solve.start();
  sys.solve();
  solve.stop();

  cout << "Solved: " << solve.time() << solve.unit()
       << endl << flush;

  // Write Sol //
  try{
    option.getValue("-nopos");
  }
  catch(...){
    FEMSolution<complex<double> > feSol;
    sys.getSolution(feSol, *fs, volume);
    feSol.write("free");
  }

  // Clean //
  delete fs;

  // Timer -- Finalize -- Return //
  timer.stop();

  cout << "Elapsed Time: " << timer.time()
       << " s"             << endl;
}

int main(int argc, char** argv){
  // Init SmallFem //
  SmallFem::Keywords("-msh,-o,-k,-nopos,-type");
  SmallFem::Initialize(argc, argv);

  compute(SmallFem::getOptions());

  SmallFem::Finalize();
  return 0;
}
