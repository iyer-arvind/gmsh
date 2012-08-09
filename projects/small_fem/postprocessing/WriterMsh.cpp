#include <sstream>
#include "WriterMsh.h"

using namespace std;

WriterMsh::WriterMsh(void){
}

WriterMsh::~WriterMsh(void){
}

void WriterMsh::write(const std::string name) const{
  stringstream fileName; 
  fileName << name << ".msh";
  
  out = new ofstream;
  out->open(fileName.str().c_str());
  
  if(!hasDomain){
    *out << "No Domain has been given !" << endl;
  }
  
  else{
    writeHeader();
    writeNodes();
    writeElements();
    
    if(hasValue)
      writeNodalValues(name);  
  }

  out->close();
  delete out;
}

void WriterMsh::writeHeader(void) const{
  *out << "$MeshFormat" << endl
       << "2.2 0 8" << endl
       << "$EndMeshFormat" << endl; 
}

void WriterMsh::writeNodes(void) const{
  *out << "$Nodes" << endl
       << N << endl;

  for(int i = 0; i < N; i++){
    *out << (*node)[i]->getNum() << " "
	 << (*node)[i]->x()      << " "
	 << (*node)[i]->y()      << " "
	 << (*node)[i]->z()      << endl;
  }

  *out << "$EndNodes" << endl;
}

void WriterMsh::writeElements(void) const{
  *out << "$Elements" << endl
       << E << endl;
  
  for(int i = 0; i < E; i++){
    *out << (*element)[i]->getNum()        << " " 
	 << (*element)[i]->getTypeForMSH() 
	 << " 2 1 1" << " "; 
           // 2 Tags --> (1 physical entity, 1 elementary geometry) 

    const int M = (*element)[i]->getNumVertices();
    MElement* myElement = 
      const_cast<MElement*>((*element)[i]);

    for(int j = 0; j < M; j++)
      *out << myElement->getVertex(j)->getNum() << " ";
    
    *out << endl;
  }
  
  *out << "$EndElements" << endl;
}

void WriterMsh::writeNodalValues(const string name) const{
  *out << "$ElementNodeData"   << endl
       << "1"                  << endl  // 1 string tag
       << "\"" << name << "\"" << endl  // (name)
       << "1"                  << endl  // 1 real tag 
       << "0"                  << endl  // (time value)
       << "3"                  << endl  // 3 integer tag
       << "0"                  << endl; // (time step index)
    
  if(isScalar)
    *out << "1" << endl;                // (number of field -- scalar)
  else
    *out << "3" << endl;                // (number of field -- vector)
  
  *out << E << endl;                    // (number of element)
  
  for(int i = 0; i < E; i++){
    *out << (*element)[i]->getNum()         << " " 
	 << (*element)[i]->getNumVertices() << " ";
    
    const int M = (*element)[i]->getNumVertices();
    MElement* myElement = 
      const_cast<MElement*>((*element)[i]);

    for(int j = 0; j < M; j++){
      const int id = myElement->getVertex(j)->getNum() - 1;
      // Note: getNum() ranges from *1* to MAX
      //   --> we need to substract 1 !!

      if(isScalar)
	*out << (*nodalScalarValue)[id] << " ";
      else
	*out << (*nodalVectorValue)[id](0) << " "
	     << (*nodalVectorValue)[id](1) << " "
	     << (*nodalVectorValue)[id](2) << " ";
    }
    
    *out << endl;
  }
  *out << "$EndElementNodeData" << endl;
}
