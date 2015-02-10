#include "SmallFem.h"
#include "ScatteringHelper.h"
#include "Timer.h"

#include "GroupOfElement.h"
#include "FunctionSpaceVector.h"
#include "FormulationSteadyWave.h"
#include "System.h"
#include "FEMSolution.h"

#include <iostream>

using namespace std;

void compute(const Options& option){
  // Timers
  Timer full;
  Timer timer;

  // Get Domains //
  full.start();
  timer.start();
  cout << "Reading... " << flush;

  Mesh msh(option.getValue("-msh")[1]);

  GroupOfElement PMLxyz   = msh.getFromPhysical(1000);
  GroupOfElement PMLxz    = msh.getFromPhysical(1001);
  GroupOfElement PMLyz    = msh.getFromPhysical(1002);
  GroupOfElement PMLxy    = msh.getFromPhysical(1003);
  GroupOfElement PMLz     = msh.getFromPhysical(1004);
  GroupOfElement PMLy     = msh.getFromPhysical(1005);
  GroupOfElement PMLx     = msh.getFromPhysical(1006);
  GroupOfElement Scat_In  = msh.getFromPhysical(1008);
  GroupOfElement Scat_Out = msh.getFromPhysical(1007);

  // Full Domain
  vector<const GroupOfElement*> All_domains(9);
  All_domains[0] = &Scat_In;
  All_domains[1] = &Scat_Out;
  All_domains[2] = &PMLxyz;
  All_domains[3] = &PMLxz;
  All_domains[4] = &PMLyz;
  All_domains[5] = &PMLxy;
  All_domains[6] = &PMLz;
  All_domains[7] = &PMLy;
  All_domains[8] = &PMLx;

  // Full Domain (for Visu)
  GroupOfElement All_domains_visu(msh);
  All_domains_visu.add(Scat_In);
  All_domains_visu.add(Scat_Out);
  All_domains_visu.add(PMLxyz);
  All_domains_visu.add(PMLxz);
  All_domains_visu.add(PMLyz);
  All_domains_visu.add(PMLxy);
  All_domains_visu.add(PMLz);
  All_domains_visu.add(PMLy);
  All_domains_visu.add(PMLx);

  timer.stop();
  cout << "done in " << timer.time() << "[" << timer.unit() << "]! "
       << endl << flush;

  // FunctionSpace //
  const size_t order = atoi(option.getValue("-o")[1].c_str());
  FunctionSpaceVector fs(All_domains, order);

  // Formulation //
  const double k = (Constant::omega0 / Constant::cel);

  timer.start();
  cout << "Assembling... " << flush;

  FormulationSteadyWave<Complex> in(Scat_In, fs, k,
                                    Material::In::Nu,
                                    Material::In::Epsilon,
                                    Signal::In::source);

  FormulationSteadyWave<Complex> out(Scat_Out, fs, k,
                                     Material::Out::Nu,
                                     Material::Out::Epsilon,
                                     Signal::Out::source);

  FormulationSteadyWave<Complex> xyz(PMLxyz, fs, k,
                                     Material::XYZ::Nu,
                                     Material::XYZ::Epsilon,
                                     Signal::PML::source);

  FormulationSteadyWave<Complex> xz(PMLxz, fs, k,
                                    Material::XZ::Nu,
                                    Material::XZ::Epsilon,
                                    Signal::PML::source);

  FormulationSteadyWave<Complex> yz(PMLyz, fs, k,
                                    Material::YZ::Nu,
                                    Material::YZ::Epsilon,
                                    Signal::PML::source);

  FormulationSteadyWave<Complex> xy(PMLxy, fs, k,
                                    Material::XY::Nu,
                                    Material::XY::Epsilon,
                                    Signal::PML::source);

  FormulationSteadyWave<Complex> x(PMLx, fs, k,
                                   Material::X::Nu,
                                   Material::X::Epsilon,
                                   Signal::PML::source);

  FormulationSteadyWave<Complex> y(PMLy, fs, k,
                                   Material::Y::Nu,
                                   Material::Y::Epsilon,
                                   Signal::PML::source);

  FormulationSteadyWave<Complex> z(PMLz, fs, k,
                                   Material::Z::Nu,
                                   Material::Z::Epsilon,
                                   Signal::PML::source);
  // System //
  System<Complex> sys;

  sys.addFormulation(in);
  sys.addFormulation(out);
  sys.addFormulation(xyz);
  sys.addFormulation(xz);
  sys.addFormulation(yz);
  sys.addFormulation(xy);
  sys.addFormulation(x);
  sys.addFormulation(y);
  sys.addFormulation(z);

  sys.assemble();

  timer.stop();
  cout << "done in " << timer.time() << "[" << timer.unit() << "]! "
       << endl << flush;

  timer.start();
  cout << "Solving: " << sys.getSize() << "... " << flush;

  sys.solve();

  timer.stop();
  cout << "done in " << timer.time() << "[" << timer.unit() << "]! "
       << endl << flush;

  // Post-Pro //
  try{
    option.getValue("-nopos");
  }
  catch(...){
    timer.start();
    cout << "Drawing... " << flush;

    FEMSolution<Complex> feSol;
    sys.getSolution(feSol, fs, All_domains_visu);
    feSol.write("scat");

    timer.stop();
    cout << "done in " << timer.time() << "[" << timer.unit() << "]! "
         << endl << flush;
  }

  // Full time
  full.stop();
  cout << "Elapsed time: " << full.time() << "[" << full.unit() << "]"
       << endl << flush;
}

int main(int argc, char** argv){
  // Init SmallFem //
  SmallFem::Keywords("-msh,-o,-nopos");
  SmallFem::Initialize(argc, argv);

  compute(SmallFem::getOptions());

  SmallFem::Finalize();
  return 0;
}
