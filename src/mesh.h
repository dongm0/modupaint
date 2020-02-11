#pragma once
#include "resource.h"
#include "algvec.h"

class Polygon {
public:
    uint8_t polytype;
    vector<uint8_t> bottomface;
    map<uint8_t, vector<uint8_t> > otherpoints;

    Polygon();
    ~Polygon();
};

class Mesh {
public:
    Mesquite::MeshImpl msqmesh;
    map<uint8_t, Mesquite::Mesh::VertexHandle> vertices;
    vector<vector<uint8_t> > cells;
    set<set<uint8_t> > initedfaces;

    //真public
    Mesh();
    ~Mesh();
    void ReadVtk(string filename);
    void WriteVtk(string filename);
    void BuildGeom();
    void InitCellpos(uint8_t cellnumber);
    void UntangleforNewCell(Polygon);
    void Optimize_local(uint8_t cellnumber);
    void Optimize_global();
    //pair<uint8_t, Polygon> is_tangled(uint8_t, vector<uint8_t>);
private:
    Mesquite::Vector3D ProjecttoPlane(Mesquite::Vector3D, Mesquite::Vector3D, Mesquite::Vector3D);
    Mesquite::Vector3D FinePoint(uint8_t, Polygon);
};

vector<uint8_t> fullcell(vector<uint8_t> oc);
