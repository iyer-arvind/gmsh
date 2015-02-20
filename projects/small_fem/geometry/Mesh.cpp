#include <vector>
#include <sstream>

#include "GeoExtractor.h"
#include "Exception.h"
#include "Mesh.h"

using namespace std;

Mesh::Mesh(const std::string fileName){
  // New Mode //
  model = new GModel("SmallFEM");

  // Read Mesh //
  if(!model->readMSH(fileName))
    throw Exception("Mesh: cannot open file: %s", fileName.c_str());

  // Get Entity //
  vector<GEntity*> entity;
  model->getEntities(entity);

  // Extract Element //
  pair<
    map<const MElement*, size_t, ElementComparator>*,
    multimap<int, const MElement*>*
    >
    elementsExtracted = GeoExtractor::extractElement(entity);

  element  = elementsExtracted.first;
  physical = elementsExtracted.second;

  // Extract Nodes //
  vertex = GeoExtractor::extractVertex(*element);

  // Extract Edges and Faces //
  // First, get them unsorted but such that they are unique
  std::set<const MEdge*, EdgeComparator>* edgeSet;
  std::set<const MFace*, FaceComparator>* faceSet;

  edgeSet = GeoExtractor::extractEdge(*element);
  faceSet = GeoExtractor::extractFace(*element);

  // Then, sort them
  doEdge(*edgeSet);
  doFace(*faceSet);

  // Finaly, clean Sets
  delete edgeSet;
  delete faceSet;

  // Number Geometry //
  nextId = 0;
  number();
}

Mesh::~Mesh(void){
  // Delete Elements //

  // WARNING
  // Mesh is *NOT* responsible for
  // Deleting MElement*
  delete element;
  delete physical;

  // Delete Vertices //

  // WARNING
  // Mesh is *NOT* responsible for
  // Deleting MVertex*
  delete vertex;

  // Delete Edges //
  const map<const MEdge*, size_t, EdgeSort>::iterator
    endE = edge->end();

  map<const MEdge*, size_t, EdgeSort>::iterator
    itE = edge->begin();

  for(; itE != endE; itE++)
    delete itE->first;

  delete edge;

  // Delete Faces //
  const map<const MFace*, size_t, FaceSort>::iterator
    endF = face->end();

  map<const MFace*, size_t, FaceSort>::iterator
    itF = face->begin();

  for(; itF != endF; itF++)
    delete itF->first;

  delete face;

  // Delete Model //
  delete model;
}

size_t Mesh::getGlobalId(const MElement& element) const{
  map<const MElement*, size_t, ElementComparator>::iterator
    it = this->element->find(&element);

  if(it == this->element->end())
    throw Exception("Mesh::getGlobalId(): element not found");

  return it->second;
}

size_t Mesh::getGlobalId(const MVertex& vertex) const{
  map<const MVertex*, size_t, VertexComparator>::iterator
    it = this->vertex->find(&vertex);

  if(it == this->vertex->end())
    throw Exception("Mesh::getGlobalId(): vertex not found");

  return it->second;
}

size_t Mesh::getGlobalId(const MEdge& edge) const{
  // Look for Edge //
  map<const MEdge*, size_t, EdgeSort>::iterator it = this->edge->find(&edge);

  if(it == this->edge->end())
    throw Exception("Mesh::getGlobalId(): edge not found");

  return it->second;
}

size_t Mesh::getGlobalId(const MFace& face) const{
  // Look for Face //
  map<const MFace*, size_t, FaceSort>::iterator it = this->face->find(&face);

  if(it == this->face->end())
    throw Exception("Mesh::getGlobalId(): face not found");

  return it->second;
}

bool Mesh::EdgeSort::operator()(const MEdge* a, const MEdge* b) const{
  if(a->getVertex(0)->getNum() == b->getVertex(0)->getNum())
    return a->getVertex(1)->getNum() < b->getVertex(1)->getNum();
  else
    return a->getVertex(0)->getNum() < b->getVertex(0)->getNum();
}

bool Mesh::FaceSort::operator()(const MFace* a, const MFace* b) const{
  const int sizeA = a->getNumVertices();
  const int sizeB = b->getNumVertices();

  // Quad Faces are *bigger* than Tri Face //
  if(sizeA < sizeB)
    return true;  // 'a' is a Tri and is smaller than 'b' (a quad)

  if(sizeA > sizeB)
    return false; // 'a' is a Quad and is bigger than 'b' (a tri)

  // Find first vertex with diffent Ids //
  const int sizeMinus = sizeA - 1;
        int         v = 0;
  while(a->getVertex(v)->getNum() == b->getVertex(v)->getNum() && v < sizeMinus)
    v++;

  // Compare //
  return a->getVertex(v)->getNum() < b->getVertex(v)->getNum();
}

void Mesh::doEdge(set<const MEdge*, EdgeComparator>& edgeSet){
  // Init //
  edge = new map<const MEdge*, size_t, EdgeSort>;

  // Insert edges from set in map (with new ordering) //
  set<const MEdge*, EdgeComparator>::iterator  it = edgeSet.begin();
  set<const MEdge*, EdgeComparator>::iterator end = edgeSet.end();
  for(; it != end; it++)
    edge->insert(pair<const MEdge*, size_t>(*it, 0));
}

void Mesh::doFace(set<const MFace*, FaceComparator>& faceSet){
  // Init //
  face = new map<const MFace*, size_t, FaceSort>;

  // Insert faces from set in map (with new ordering) //
  set<const MFace*, FaceComparator>::iterator  it = faceSet.begin();
  set<const MFace*, FaceComparator>::iterator end = faceSet.end();
  for(; it != end; it++)
    face->insert(pair<const MFace*, size_t>(*it, 0));
}

void Mesh::number(void){
  // Get Iterators //
  const map<const MElement*, size_t, ElementComparator>::iterator
    endEl = element->end();

  const map<const MVertex*, size_t, VertexComparator>::iterator
    endV = vertex->end();

  const map<const MEdge*, size_t, EdgeSort>::iterator
    endEd = edge->end();

  const map<const MFace*, size_t, FaceSort>::iterator
    endF = face->end();

  map<const MElement*, size_t, ElementComparator>::iterator
    itEl = element->begin();

  map<const MVertex*, size_t, VertexComparator>::iterator
    itV = vertex->begin();

  map<const MEdge*, size_t, EdgeSort>::iterator
    itEd = edge->begin();

  map<const MFace*, size_t, FaceSort>::iterator
    itF = face->begin();

  // Number Vertices //
  for(; itV != endV; itV++){
    itV->second = nextId;
    nextId++;
  }

  // Number Edges //
  for(; itEd != endEd; itEd++){
    itEd->second = nextId;
    nextId++;
  }

  // Number Faces //
  for(; itF != endF; itF++){
    itF->second = nextId;
    nextId++;
  }

  // Number Elements //
  for(; itEl != endEl; itEl++){
    itEl->second = nextId;
    nextId++;
  }
}

GroupOfElement Mesh::getFromPhysical(int physicalId) const{
  pair<multimap<int, const MElement*>::iterator,
       multimap<int, const MElement*>::iterator>
    p = physical->equal_range(physicalId);

  list<const MElement*> lst;

  for(; p.first != p.second; p.first++)
    lst.push_back(p.first->second);

  return GroupOfElement(lst, *this);
}

GroupOfElement Mesh::getFromPhysical(int physicalId, int partitionId) const{
  pair<multimap<int, const MElement*>::iterator,
       multimap<int, const MElement*>::iterator>
    p = physical->equal_range(physicalId);

  list<const MElement*> lst;

  for(; p.first != p.second; p.first++)
    if(p.first->second->getPartition() == partitionId)
      lst.push_back(p.first->second);

  return GroupOfElement(lst, *this);
}

string Mesh::toString(void) const{
  // Iterators //
  const map<const MElement*, size_t, ElementComparator>::iterator
    endEl = element->end();

  const map<const MVertex*, size_t, VertexComparator>::iterator
    endV = vertex->end();

  const map<const MEdge*, size_t, EdgeSort>::iterator
    endEd = edge->end();

  const map<const MFace*, size_t, FaceSort>::iterator
    endF = face->end();

  map<const MElement*, size_t, ElementComparator>::iterator
    itEl = element->begin();

  map<const MVertex*, size_t, VertexComparator>::iterator
    itV = vertex->begin();

  map<const MEdge*, size_t, EdgeSort>::iterator
    itEd = edge->begin();

  map<const MFace*, size_t, FaceSort>::iterator
    itF = face->begin();

  stringstream stream;


  // Header //
  stream << "***********************************************"
         << endl
         << "*                     Mesh                    *"
         << endl
         << "***********************************************"
         << endl;


  // Vertices //
  stream << "*                                             *"
         << endl
         << "* This mesh contains the following Vertex:    *"
         << endl;

  for(; itV != endV; itV++)
    stream << "*   -- Vertex "
           << showpos
           << getGlobalId(*itV->first)
           << ": {"
           << itV->first->x()
           << ", "
           << itV->first->y()
           << ", "
           << itV->first->z()
           << "}"
           << endl;

  stream << "*                                             *"
         << endl
         << "***********************************************"
         << endl;


  // Edges //
  stream << "*                                             *"
         << endl
         << "* This mesh contains the following Edges:     *"
         << endl;

  for(; itEd != endEd; itEd++)
    stream << "*   -- Edge "
           << getGlobalId(*itEd->first)
           << ": ["
           << getGlobalId(*itEd->first->getVertex(0))
           << ", "
           << getGlobalId(*itEd->first->getVertex(1))
           << "]"
           << endl;

  stream << "*                                             *"
         << endl
         << "***********************************************"
         << endl;

  // Faces //
  stream << "*                                             *"
         << endl
         << "* This mesh contains the following Faces:     *"
         << endl;

  for(; itF != endF; itF++){
    stream << "*   -- Face "
           << getGlobalId(*itF->first)
           << ": ["
           << getGlobalId(*itF->first->getVertex(0))
           << ", "
           << getGlobalId(*itF->first->getVertex(1))
           << ", "
           << getGlobalId(*itF->first->getVertex(2));

    if(itF->first->getNumVertices() == 4)
      stream << ", "
             << getGlobalId(*itF->first->getVertex(3));

    stream << "]"
           << endl;
  }

  stream << "*                                             *"
         << endl
         << "***********************************************"
         << endl;


  // Elements //
  stream << "*                                             *"
         << endl
         << "* This mesh contains the following Elements:  *"
         << endl;

  for(; itEl != endEl; itEl++){
    int nVertex =
      const_cast<MElement*>(itEl->first)->getNumPrimaryVertices();

    int nVertexMinus = nVertex - 1;

    stream << "*   -- Element "
           << getGlobalId(*itEl->first)
           << ": [";

    for(int i = 0; i < nVertexMinus; i++)
      stream << getGlobalId(*const_cast<MElement*>(itEl->first)->getVertex(i))
             << ", ";

    stream <<
      getGlobalId(*const_cast<MElement*>(itEl->first)->getVertex(nVertexMinus))
           << "]"
           << endl;
  }

  stream << "*                                             *"
         << endl
         << "***********************************************"
         << endl;

  // Retrun //
  return stream.str();
}
