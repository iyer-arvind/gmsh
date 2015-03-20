// Gmsh - Copyright (C) 1997-2014 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to the public mailing list <gmsh@geuz.org>.

#ifndef _BACKGROUND_MESH2D_H_
#define _BACKGROUND_MESH2D_H_


#include <string>
#include <map>
#include <vector>
#include "Pair.h"
#include "STensor3.h"

#include "BGMBase.h"

class MTriangle;

using namespace std;

/*
   The backgroundMesh2D creates a bunch of triangles on the parametric space of a GFace.
   Those triangles are local to the backgroundMesh2D so that
   they do not depend on the actual mesh that can be deleted.
   It extends the sizefield from the edges.
 */
class backgroundMesh2D : public BGMBase {
  protected:
    virtual void propagateValues(DoubleStorageType &dirichlet, simpleFunction<double> &eval_diffusivity, bool in_parametric_plane = false);
    virtual void computeSizeField();

    virtual inline unsigned int getNumMeshElements()const{return elements.size();}
    virtual const MElement* getElement(unsigned int i)const;
    
    virtual GPoint get_GPoint_from_MVertex(const MVertex *) const;

    // only 2D:
    void updateSizes();
    // copy the mesh stored in GFace in local
    void create_mesh_copy();
    // creates a mesh of GFace and store it in local !!!, does not store the mesh in GFace !
    void create_face_mesh();

    double sizeFactor;

  
    std::vector<MTriangle*> tempTR;
    vector<MElement*> elements;
    vector<MVertex*> vertices;

    map<MVertex*,MVertex*> _3Dto2D;
    map<MVertex*,MVertex*> _2Dto3D;


  public:
    backgroundMesh2D(GFace *, bool erase_2D3D=true);
    virtual ~backgroundMesh2D();
    
    virtual MElementOctree* getOctree();

    // TODO: only 2D
    virtual void reset(bool erase_2D3D=true);// deletes everything and rebuild with GFace*
    virtual void unset();// deletes everything... called by destructor.

    // not used !!!! TODO !!!
    void setSizeFactor (double s) {sizeFactor = s;}
    
    
    virtual vector<MVertex*>::iterator beginvertices(){return vertices.begin();}
    virtual vector<MVertex*>::iterator endvertices(){return vertices.end();}
    virtual vector<MElement*>::iterator beginelements(){return elements.begin();}
    virtual vector<MElement*>::iterator endelements(){return elements.end();}
    virtual vector<MVertex*>::const_iterator beginvertices()const{return vertices.begin();}
    virtual vector<MVertex*>::const_iterator endvertices()const{return vertices.end();}
    virtual vector<MElement*>::const_iterator beginelements()const{return elements.begin();}
    virtual vector<MElement*>::const_iterator endelements()const{return elements.end();}

};

//------------------------------------------------------------------------

class RK_form{// informations for RK at one point
  public:
    RK_form(){
    }
//    RK_form(const RK_form &other){
//      t1 = other.t1;
//      t2 = other.t2;
//      h.first = other.h.first;
//      h.second = other.h.second;
//      paramh.first = other.paramh.first;
//      paramh.second = other.paramh.second;
//      paramt1 = other.paramt1;
//      paramt2 = other.paramt2;
//      angle = other.angle;
//    }
  
    SMetric3 metricField;
    SVector3 t1, t2;// 3D cross field directions
    SVector3 normal;// 3D cross field directions
    pair<double,double> h;// 3D sizes
    pair<double,double> paramh;// sizes in parametric domain
    SPoint2 paramt1,paramt2;
    double angle,localsize;
};

//------------------------------------------------------------------------


class frameFieldBackgroundMesh2D : public backgroundMesh2D{
  private:
    // specification for cross field
    DoubleStorageType angles;  // an attached angle
    DoubleStorageType smoothness;

    void computeCrossField(simpleFunction<double> &eval_diffusivity);
    void computeSmoothness();

  public:
    frameFieldBackgroundMesh2D(GFace *_gf);
    virtual ~frameFieldBackgroundMesh2D();

    virtual void reset(bool erase_2D3D=true);

    double angle(double u, double v); 
    double angle(MVertex *v);

    double get_smoothness(double u, double v); 
    double get_smoothness(MVertex *v);

    void eval_crossfield(double u, double v, STensor3 &cf);
    void eval_crossfield(MVertex *vert, STensor3 &cf);

    void exportCrossField(const string &filename);
    Pair<SVector3, SVector3> compute_crossfield_directions(double u,double v, double angle_current);
    bool compute_RK_infos(double u,double v, double x, double y, double z,RK_form &infos);

    inline void exportSmoothness(const string &filename) const{export_scalar(filename,smoothness);};
};

#endif
