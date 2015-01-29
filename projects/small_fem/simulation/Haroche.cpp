#include <sys/resource.h>
#include <unistd.h>
#include <iostream>
#include <cstdio>

#include "Mesh.h"
#include "SystemEigen.h"
#include "SystemHelper.h"

#include "FormulationStiffness.h"
#include "FormulationMass.h"

#include "HarocheHelper.h"
#include "MPIOStream.h"
#include "SmallFem.h"

using namespace std;

void dump(string filename, fullVector<Complex>& eig){
  FILE* file = fopen(filename.c_str(), "w");
  double pi  = 4 * atan(1);

  fprintf(file, "Eig\tReal(Omega^2)\tImag(Omega^2)\tf[MHz]\tt[ms]\n");
  for(int i = 0; i < eig.size(); i++)
    fprintf(file, "%d:\t%.16e\t%.16e\t%.16e\t%.16e\n",
            i,
            eig(i).real(), eig(i).imag(),
            sqrt(eig(i)).real()  / (2 * pi) * 1e-9,
            1.0 / (sqrt(eig(i)).imag() / (2 * pi)) * 1e3);

  fclose(file);
}

fullVector<Complex> fZero(fullVector<double>& xyz){
  fullVector<Complex> f(3);

  f(0) = Complex(0, 0);
  f(1) = Complex(0, 0);
  f(2) = Complex(0, 0);

  return f;
}

double getMemory(void){
  long  rss = 0;
  FILE*  fp = NULL;

  if ((fp = fopen("/proc/self/statm", "r")) == NULL)
    return 0;
  if (fscanf(fp, "%*s%ld", &rss) != 1){
    fclose(fp);
    return 0;
  }

  fclose(fp);
  return (double)((size_t)rss * (size_t)sysconf(_SC_PAGESIZE)) / (double)(1e9);
}

void compute(const Options& option){
  // MPI std::cout //
  MPIOStream cout(0, std::cout);
  int        myProc;
  int        nProcs;

  MPI_Comm_rank(MPI_COMM_WORLD, &myProc);
  MPI_Comm_size(MPI_COMM_WORLD, &nProcs);

  // Get Domains //
  cout << "Reading domain... " << flush;
  Mesh msh(option.getValue("-msh")[1]);

  GroupOfElement* Air;

  GroupOfElement* PMLx;
  GroupOfElement* PMLxy;
  GroupOfElement* PMLy;
  GroupOfElement* PMLz;
  GroupOfElement* PMLxyz;
  GroupOfElement* PMLxz;
  GroupOfElement* PMLyz;

  GroupOfElement* Mirror;
  GroupOfElement* SurfYZ;
  GroupOfElement* SurfXZ;
  GroupOfElement* SurfXY;

  if(nProcs != 1){
    Air    = new GroupOfElement(msh.getFromPhysical(138, myProc + 1));
    PMLx   = new GroupOfElement(msh.getFromPhysical(139, myProc + 1));
    PMLxy  = new GroupOfElement(msh.getFromPhysical(140, myProc + 1));
    PMLy   = new GroupOfElement(msh.getFromPhysical(141, myProc + 1));
    PMLz   = new GroupOfElement(msh.getFromPhysical(142, myProc + 1));
    PMLxyz = new GroupOfElement(msh.getFromPhysical(143, myProc + 1));
    PMLxz  = new GroupOfElement(msh.getFromPhysical(144, myProc + 1));
    PMLyz  = new GroupOfElement(msh.getFromPhysical(145, myProc + 1));
    Mirror = new GroupOfElement(msh.getFromPhysical(148, myProc + 1));
    SurfYZ = new GroupOfElement(msh.getFromPhysical(147, myProc + 1));
    SurfXZ = new GroupOfElement(msh.getFromPhysical(146, myProc + 1));
    SurfXY = new GroupOfElement(msh.getFromPhysical(149, myProc + 1));
  }

  else{
    Air    = new GroupOfElement(msh.getFromPhysical(138));
    PMLx   = new GroupOfElement(msh.getFromPhysical(139));
    PMLxy  = new GroupOfElement(msh.getFromPhysical(140));
    PMLy   = new GroupOfElement(msh.getFromPhysical(141));
    PMLz   = new GroupOfElement(msh.getFromPhysical(142));
    PMLxyz = new GroupOfElement(msh.getFromPhysical(143));
    PMLxz  = new GroupOfElement(msh.getFromPhysical(144));
    PMLyz  = new GroupOfElement(msh.getFromPhysical(145));
    Mirror = new GroupOfElement(msh.getFromPhysical(148));
    SurfYZ = new GroupOfElement(msh.getFromPhysical(147));
    SurfXZ = new GroupOfElement(msh.getFromPhysical(146));
    SurfXY = new GroupOfElement(msh.getFromPhysical(149));
  }

  // Full Domain
  vector<const GroupOfElement*> All_domains(12);
  All_domains[0]  = Air;

  All_domains[1]  = PMLx;
  All_domains[2]  = PMLxy;
  All_domains[3]  = PMLy;
  All_domains[4]  = PMLz;
  All_domains[5]  = PMLxyz;
  All_domains[6]  = PMLxz;
  All_domains[7]  = PMLyz;

  All_domains[8]  = Mirror;
  All_domains[9]  = SurfYZ;
  All_domains[10] = SurfXZ;
  All_domains[11] = SurfXY;

  // Full Surface
  vector<const GroupOfElement*> All_surfaces(3);
  All_surfaces[0] = SurfYZ;
  All_surfaces[1] = SurfXZ;
  All_surfaces[2] = SurfXY;
  cout << "Done! (" << getMemory() << " GB)" << endl << flush;

  // FunctionSpace //
  cout << "FunctionSpace... " << flush;
  const size_t order = atoi(option.getValue("-o")[1].c_str());
  FunctionSpaceVector fs(All_domains, order);

  cout << "Done! (" << getMemory() << " GB)" << endl << flush;

  // Formulation //
  cout << "Formulations... " << flush;

  Formulation<Complex>* stifAir = new FormulationStiffness<Complex>
                                      (*Air,    fs, fs, Material::Air::Nu);
  Formulation<Complex>* stifXYZ = new FormulationStiffness<Complex>
                                      (*PMLxyz, fs, fs, Material::XYZ::Nu);
  Formulation<Complex>* stifXY  = new FormulationStiffness<Complex>
                                      (*PMLxy,  fs, fs,  Material::XY::Nu);
  Formulation<Complex>* stifYZ  = new FormulationStiffness<Complex>
                                      (*PMLyz,  fs, fs,  Material::YZ::Nu);
  Formulation<Complex>* stifXZ  = new FormulationStiffness<Complex>
                                      (*PMLxz,  fs, fs,  Material::XZ::Nu);
  Formulation<Complex>* stifX   = new FormulationStiffness<Complex>
                                      (*PMLx,   fs, fs,   Material::X::Nu);
  Formulation<Complex>* stifY   = new FormulationStiffness<Complex>
                                      (*PMLy,   fs, fs,   Material::Y::Nu);
  Formulation<Complex>* stifZ   = new FormulationStiffness<Complex>
                                      (*PMLz,   fs, fs,   Material::Z::Nu);

  Formulation<Complex>* massAir = new FormulationMass<Complex>
                                      (*Air,    fs, fs, Material::Air::Epsilon);
  Formulation<Complex>* massXYZ = new FormulationMass<Complex>
                                      (*PMLxyz, fs, fs, Material::XYZ::Epsilon);
  Formulation<Complex>* massXY  = new FormulationMass<Complex>
                                      (*PMLxy,  fs, fs,  Material::XY::Epsilon);
  Formulation<Complex>* massYZ  = new FormulationMass<Complex>
                                      (*PMLyz,  fs, fs,  Material::YZ::Epsilon);
  Formulation<Complex>* massXZ  = new FormulationMass<Complex>
                                      (*PMLxz,  fs, fs,  Material::XZ::Epsilon);
  Formulation<Complex>* massX   = new FormulationMass<Complex>
                                      (*PMLx,   fs, fs,   Material::X::Epsilon);
  Formulation<Complex>* massY   = new FormulationMass<Complex>
                                      (*PMLy,   fs, fs,   Material::Y::Epsilon);
  Formulation<Complex>* massZ   = new FormulationMass<Complex>
                                      (*PMLz,   fs, fs,   Material::Z::Epsilon);

  /*
  Formulation<Complex>* stifAir = new FormulationStiffness<Complex>
                                    (*Air,    fs, fs, Material::Air::OverMuEps);
  Formulation<Complex>* stifXYZ = new FormulationStiffness<Complex>
                                    (*PMLxyz, fs, fs, Material::XYZ::OverMuEps);
  Formulation<Complex>* stifXY  = new FormulationStiffness<Complex>
                                    (*PMLxy,  fs, fs,  Material::XY::OverMuEps);
  Formulation<Complex>* stifYZ  = new FormulationStiffness<Complex>
                                    (*PMLyz,  fs, fs,  Material::YZ::OverMuEps);
  Formulation<Complex>* stifXZ  = new FormulationStiffness<Complex>
                                    (*PMLxz,  fs, fs,  Material::XZ::OverMuEps);
  Formulation<Complex>* stifX   = new FormulationStiffness<Complex>
                                    (*PMLx,   fs, fs,   Material::X::OverMuEps);
  Formulation<Complex>* stifY   = new FormulationStiffness<Complex>
                                    (*PMLy,   fs, fs,   Material::Y::OverMuEps);
  Formulation<Complex>* stifZ   = new FormulationStiffness<Complex>
                                    (*PMLz,   fs, fs,   Material::Z::OverMuEps);

  Formulation<Complex>* massAir = new FormulationMass<Complex>(*Air,    fs, fs);
  Formulation<Complex>* massXYZ = new FormulationMass<Complex>(*PMLxyz, fs, fs);
  Formulation<Complex>* massXY  = new FormulationMass<Complex>(*PMLxy,  fs, fs);
  Formulation<Complex>* massYZ  = new FormulationMass<Complex>(*PMLyz,  fs, fs);
  Formulation<Complex>* massXZ  = new FormulationMass<Complex>(*PMLxz,  fs, fs);
  Formulation<Complex>* massX   = new FormulationMass<Complex>(*PMLx,   fs, fs);
  Formulation<Complex>* massY   = new FormulationMass<Complex>(*PMLy,   fs, fs);
  Formulation<Complex>* massZ   = new FormulationMass<Complex>(*PMLz,   fs, fs);
  */
  cout << "Done! (" << getMemory() << " GB)" << endl << flush;

  // System //
  cout << "System... " << flush;
  SystemEigen sys;

  sys.addFormulation(*stifAir);
  sys.addFormulation(*stifXYZ);
  sys.addFormulation(*stifXY);
  sys.addFormulation(*stifYZ);
  sys.addFormulation(*stifXZ);
  sys.addFormulation(*stifX);
  sys.addFormulation(*stifY);
  sys.addFormulation(*stifZ);

  sys.addFormulationB(*massAir);
  sys.addFormulationB(*massXYZ);
  sys.addFormulationB(*massXY);
  sys.addFormulationB(*massYZ);
  sys.addFormulationB(*massXZ);
  sys.addFormulationB(*massX);
  sys.addFormulationB(*massY);
  sys.addFormulationB(*massZ);
  cout << "Done! (" << getMemory() << " GB)" << endl << flush;

  // Dirichlet //
  // Mirror
  cout << "Dirichlet... " << flush;
  SystemHelper<Complex>::dirichlet(sys, fs, *Mirror, fZero);

  // Symmetry
  try{
    int sym = atoi(option.getValue("-sym")[1].c_str());

    if(sym == 0)
      SystemHelper<Complex>::dirichlet(sys, fs, *SurfYZ, fZero);

    else
      SystemHelper<Complex>::dirichlet(sys, fs, *SurfXZ, fZero);
  }

  catch(...){
    // If no symmetry given, use YZ
    cout << "No symmetry given: defaulting to YZ" << endl;
    SystemHelper<Complex>::dirichlet(sys, fs, *SurfYZ, fZero);
  }
  cout << "Done! (" << getMemory() << " GB)" << endl << flush;

  // Assemble //
  cout << "True assembling... " << endl << flush;
  sys.assemble();
  cout << "Done! (" << getMemory() << " GB)" << endl << flush;

  // Free formulations //
  cout << "Clearing..." << endl << flush;
  delete stifAir;
  delete stifXYZ;
  delete stifXY;
  delete stifYZ;
  delete stifXZ;
  delete stifX;
  delete stifY;
  delete stifZ;

  delete massAir;
  delete massXYZ;
  delete massXY;
  delete massYZ;
  delete massXZ;
  delete massX;
  delete massY;
  delete massZ;
  cout << "Done! (" << getMemory() << " GB)" << endl << flush;

  // Solve //
  cout << "Solving: " << sys.getSize() << endl << flush;

  // Set number of eigenvalue (if any, else default)
  try{
    sys.setNumberOfEigenValues(atoi(option.getValue("-n")[1].c_str()));
  }

  catch(...){
  }

  // Set shift (if any, else default)
  try{
    const double shift = atof(option.getValue("-shift")[1].c_str());

    sys.setWhichEigenpairs("target_magnitude");
    sys.setTarget(Complex(shift, 0));
  }

  catch(...){
  }

  // Set tolerance (if any, else default)
  try{
    sys.setTolerance(atof(option.getValue("-tol")[1].c_str()));
  }

  catch(...){
  }

  // Set maximun iteration number (if any, else default)
  try{
    sys.setMaxIteration(atoi(option.getValue("-maxit")[1].c_str()));
  }

  catch(...){
  }

  // Do what you have to do !
  sys.setProblem("gen_non_hermitian");
  sys.solve();
  cout << "Done! (" << getMemory() << " GB)" << endl << flush;

  // Post-Pro //
  if(myProc == 0){
    // Display
    cout << "Post Pro" << endl << flush;
    const size_t nEigenValue = sys.getNComputedSolution();
    fullVector<Complex> eigenValue;

    sys.getEigenValues(eigenValue);

    cout << "Number of found Eigenvalues: " << nEigenValue
         << endl
         << endl
         << "Number\tEigen Value" << endl;

    for(size_t i = 0; i < nEigenValue; i++)
      cout << "#" << i + 1  << "\t"
           << std::scientific
           << eigenValue(i) << endl;

    // Dump on disk
    dump("harocheValues.txt", eigenValue);
  }

  // Draw
  try{
    option.getValue("-nopos");
  }

  catch(...){
    FEMSolution<Complex> feSol;
    sys.getSolution(feSol, fs, All_surfaces);

    feSol.setSaveMesh(false);
    feSol.setBinaryFormat(true);
    if(nProcs != 1)
      feSol.setParition(myProc + 1);

    feSol.write("harocheModes");
  }

  // Game over! //
  delete Air;
  delete PMLx;
  delete PMLxy;
  delete PMLy;
  delete PMLz;
  delete PMLxyz;
  delete PMLxz;
  delete PMLyz;
  delete Mirror;
  delete SurfYZ;
  delete SurfXZ;
  delete SurfXY;
}

int main(int argc, char** argv){
  // Init SmallFem //
  SmallFem::Keywords("-msh,-o,-n,-shift,-sym,-tol,-maxit,-nopos");
  SmallFem::Initialize(argc, argv);

  compute(SmallFem::getOptions());

  SmallFem::Finalize();
  return 0;
}
