#ifndef _MESH_H_
#define _MESH_H_

#include <map>
#include <string>

#include "Comparators.h"
#include "fullMatrix.h"

#include "GModel.h"

#include "GroupOfElement.h"
#include "MElement.h"
#include "MVertex.h"
#include "MEdge.h"
#include "MFace.h"

/**
   @class Mesh
   @brief Represents a mesh

   This class represents a mesh.

   This class is responsible of the handling mesh elements.

   It is also responsible of the numbering of those elements.
   Indeed, each element is granted a unique ID.

   A Mesh is instantiated thanks to a
   <a href="http://www.geuz.org/gmsh">gmsh</a>
   .msh file, wich discribes the mesh.
*/

class GroupOfElement;

class Mesh{
 private:
  class EdgeSort{
    public:
      bool operator()(const MEdge* a, const MEdge* b) const;
  };

  class FaceSort{
    public:
      bool operator()(const MFace* a, const MFace* b) const;
  };

 private:
  GModel* model;

  std::map<const MElement*, size_t, ElementComparator>* element;
  std::map<const MVertex*, size_t, VertexComparator>*   vertex;
  std::map<const MEdge*, size_t, EdgeSort>*             edge;
  std::map<const MFace*, size_t, FaceSort>*             face;

  std::multimap<int, const MElement*>* physical;

  size_t nextId;

 public:
   Mesh(const std::string fileName);
  ~Mesh(void);

  GModel& getModel(void) const;

  size_t getGlobalId(const MElement& element) const;
  size_t getGlobalId(const MVertex& vertex) const;
  size_t getGlobalId(const MEdge& edge) const;
  size_t getGlobalId(const MFace& face) const;

  size_t getElementNumber(void) const;
  size_t getVertexNumber(void) const;
  size_t getEdgeNumber(void) const;
  size_t getFaceNumber(void) const;

  GroupOfElement getFromPhysical(int physicalId, int partitionId) const;
  GroupOfElement getFromPhysical(int physicalId) const;

  std::string toString(void) const;

 private:
  static bool edgeSort(const MEdge* a, const MEdge* b);
  static bool faceSort(const MFace* a, const MFace* b);

 private:
  void doEdge(std::set<const MEdge*, EdgeComparator>& edgeSet);
  void doFace(std::set<const MFace*, FaceComparator>& faceSet);
  void number(void);
};


/**
   @fn Mesh::Mesh
   @param fileName The path to the .msh file discribing the Mesh

   Instanciates a new Mesh
   **

   @fn Mesh::~Mesh
   Deletes this Mesh
   **

   @fn Mesh::getModel
   @return Returns the GModel used for generating this Mesh
   **

   @fn size_t Mesh::getGlobalId(const MElement& element) const
   @param element A MElement
   @return Returns the global ID (in this Mesh) of the given MElement
   **

   @fn size_t Mesh::getGlobalId(const MVertex& vertex) const
   @param vertex A MVertex
   @return Returns the global ID (in this Mesh) of the given MVertex
   **

   @fn size_t Mesh::getGlobalId(const MEdge& edge) const
   @param edge A MEdge
   @return Returns the global ID (in this Mesh) of the given MEdge
   **

   @fn size_t Mesh::getGlobalId(const MFace& face) const
   @param face A MFace
   @return Returns the global ID (in this Mesh) of the given MFace
   **

   @fn Mesh::getElementNumber
   @return Returns the number of element in this Mesh

   By Element we mean Quads, Tets, etc:
   This excludes Vertices, Edges, Faces and Cells
   **

   @fn Mesh::getVertexNumber
   @return Returns the number of MVertices in this Mesh
   **

   @fn Mesh::getEdgeNumber
   @return Returns the number of MEdge%s in this Mesh
   **

   @fn Mesh::getFaceNumber
   @return Returns the number of MFace%s in this Mesh
   **

   @fn Mesh::getFromPhysical(int, int)
   @param physicalId A physical ID
   (see <a href="http://www.geuz.org/gmsh">gmsh</a> documentation)
   @param partitionId A partition ID
   (see <a href="http://www.geuz.org/gmsh">gmsh</a> documentation)

   @return Instantiate a new GroupOfElement,
   containing the MElements of the given physical and partition IDs
   **

   @fn Mesh::getFromPhysical(int)
   @param physicalId A physical ID
   (see <a href="http://www.geuz.org/gmsh">gmsh</a> documentation)

   @return Instantiate a new GroupOfElement,
   containing the MElements of the given physical ID
   **

   @fn Mesh::toString
   @return Returns a description of this Mesh
*/

//////////////////////
// Inline Functions //
//////////////////////

inline GModel& Mesh::getModel(void) const{
  return *model;
}

inline size_t Mesh::getElementNumber(void) const{
  return element->size();
}

inline size_t Mesh::getVertexNumber(void) const{
  return vertex->size();
}

inline size_t Mesh::getEdgeNumber(void) const{
  return edge->size();
}

inline size_t Mesh::getFaceNumber(void) const{
  return face->size();
}

#endif
